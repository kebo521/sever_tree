
#include "comm.h"

#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


#include <sys/wait.h>
#include<sys/stat.h>



#include <fcntl.h>
#include <linux/input.h>
#include<sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>




#define PORT_NUMBER 	8888
#define BACKLOG 		20

#define RECV_BUFF_MAX 	(8*1024)
#define SEND_BUFF_MAX 	(8*1024)


int OsGetTimeStr(char *pOutStr)
{
/*
	timespec time;
	struct tm tblock;
	clock_gettime(CLOCK_REALTIME, &time); //获取相对于1970到现在的秒数
	localtime_r(&time.tv_sec, &tblock);
*/
	time_t timer;//long
	struct tm *pblock;
	timer = time(NULL);
	pblock = localtime(&timer);
	return sprintf(pOutStr,"%04d-%02d-%02d %02d:%02d:%02d",pblock->tm_year + 1900,pblock->tm_mon,pblock->tm_mday,pblock->tm_hour,pblock->tm_min,pblock->tm_sec);
}

void* EXP_StrSwap(void* pFd)
{
	int new_fd = *(int*)pFd; 
	int ret,offset;
	int len;
	char *pRecvBuff,*pSendBuff,*pBuff;
	struct timeval timeout; 
	pBuff = (char*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;
	TRACE("StrSwap fd[%d]",new_fd);
	while(1)
	{ 
		//设置接收超时
		setsockopt(new_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)); 
		/* recv */
		ret = recv(new_fd,pRecvBuff, 2, 0); 
		if(ret < 2) 
		{ 
			fprintf(stderr,"fd[%d] recv_Head2 error:%sna\r\n",new_fd, strerror(errno)); 
			break; 
		} 
		len = pRecvBuff[0]*256 + pRecvBuff[1];
		if(len > RECV_BUFF_MAX)
		{ 
			fprintf(stderr,"fd[%d] recv_Len[%d]error\r\n",new_fd,len); 
			break; 
		}
		offset = 0;
		while(1)
		{
			ret = recv(new_fd, pRecvBuff+offset, len-offset, 0); 
			if(ret <= 0)
			{ 
				fprintf(stderr,"fd[%d] recvn[%d,%d] error:%sna\r\n",new_fd,offset,len, strerror(errno)); 
				break;
			}
			offset += ret;
			if(offset >= len) break;
			usleep(10*1000);
		}
		TRACE_HEX("pRecvBuff",(u8*)pRecvBuff,len);
		if(len > 2)
		{
			EXP_UNIT *pUnit;
			char *p=pRecvBuff;
			pUnit=EXP_StrALL(&p,p+len);
			offset = 2;
			while(pUnit)
			{
				if(pUnit->type==DATA_DEFINE_STR)
					ret=sprintf(pSendBuff+offset,"TextPutMsg Result->STR[%s]",pUnit->pStr);
				if(pUnit->type==DATA_DEFINE_BUFF)
				{
					ret=sprintf(pSendBuff+offset,"TextPutMsg Result->BUFF[%d]:\r\n",pUnit->Len);
					offset += ret;
					ret=gBcdtoStr_n(pUnit->pBuff,pUnit->Len,pSendBuff+offset,SEND_BUFF_MAX-offset);
					offset += ret;
				}
				else if(pUnit->type==DATA_DEFINE_TAG)
					ret=sprintf(pSendBuff+offset,"TextPutMsg Result[%x[%d]]",pUnit->Tag,pUnit->decimals);
				else if(pUnit->type==DATA_DEFINE_ASIC4)
					ret=sprintf(pSendBuff+offset,"TextPutMsg Asi:%s",pUnit->Asi);
				else if(pUnit->type==DATA_DEFINE_RULES)
					ret=sprintf(pSendBuff+offset,"TextPutMsg Result:%c%c",pUnit->pRule->ExpChars[0],pUnit->pRule->ExpChars[1]);
				else
					ret=sprintf(pSendBuff+offset,"TextPutMsg pUnit->type[%d],decimals[%d],Result[%d]",pUnit->type,pUnit->decimals,pUnit->Result);
				if(ret > 0) offset += ret;
				if(offset >= sizeof(pSendBuff))
				{
					offset = sizeof(pSendBuff);
					break;
				}
				pUnit=pUnit->pNext;
			}
			EXP_FreeUNIT(pUnit,0);
			pSendBuff[0]=offset/256;
			pSendBuff[1]=offset&0xff;
			send(new_fd, pSendBuff, offset+2, 0); 
			//Trace_PutMsg(pUnit);
		}
		else break;
	}
	free(pBuff);
	close(new_fd); 
	return NULL; 
}

int main(int argc, char* argv[]) 
{
	/* socket->bind->listen->accept->send/recv->close*/
	int sock_fd, new_fd; 
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	//struct sockaddr_in client_addr;
	int ret; 
	socklen_t addr_len; 
	pthread_t threadID;
	int client_num = 0; 
	TRACE("In Main argv[%s]",argv[0]);
	/*
	{
		int fd;
		char sName[64],datetime[24];
		GetDateTimeStr(datetime);
		sprintf(sName,"err%s.log",datetime);
		fd = open(sName, O_CREAT|O_RDWR, 0666);
		if (fd != -1)
		{
			dup2(fd, STDERR_FILENO);
			close(fd);
		}
		sprintf(sName,"out%s.log",datetime);
		fd = open(sName, O_CREAT|O_RDWR, 0666);
		if (fd != -1)
		{
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		
		fd = open("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2(fd, STDIN_FILENO);
			if (fd > 2) close(fd);
		}
	}
*/
	signal(SIGCHLD,SIG_IGN); 
	
	/* socket->bind->listen->accept->send/recv->close*/

	/* socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET:IPV4;SOCK_STREAM:TCP
	if( -1== sock_fd) 
	{ 
		fprintf( stderr, "socket error:%sna", strerror(errno)); 
		exit( 1); 
	} 

	/* set server sockaddr_in */
	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //clear
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY:This machine all IP
	server_addr.sin_port = htons(PORT_NUMBER); 

	/* bind */
	ret = bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr)); 
	if( -1== ret) 
	{ 
		fprintf( stderr,"bind error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 
	/* listen */
	ret = listen(sock_fd, BACKLOG); 
	if( -1== ret) 
	{ 
		fprintf(stderr, "listen error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_len); //(struct sockaddr *)
		if( -1 == new_fd) 
		{ 
			fprintf( stderr, "accept error:%sna", strerror(errno)); 
			close(sock_fd); 
			exit( 1); 
		} 
		client_num++; 
		{
			char datetime[24];
			OsGetTimeStr(datetime);
			TRACE("Server[%s]connet[%d]times[%d] sa[%d]Addr[%x,%d]",datetime,new_fd,client_num, \
				client_addr.sin_family,client_addr.sin_addr.s_addr,client_addr.sin_port);
			TRACE_HEX("sa_data",(u8*)client_addr.sin_zero,sizeof(client_addr.sin_zero));
		}
		//fprintf(stderr, "Server get connetion form client%d: %sn", client_num, inet_ntoa(client_addr.sin_addr)); 
		ret=pthread_create(&threadID,NULL,&EXP_StrSwap,&new_fd);
		if(ret)
		{
			close(new_fd);
			fprintf(stderr,"pthread_create[%d] error:%sna \r\n",ret, strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}



