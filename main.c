
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
#define BACKLOG 		10

#define RECV_BUFF_MAX 	(4*1024)
#define SEND_BUFF_MAX 	(4*1024)


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
			usleep(1000);
		}
		
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

/* socket->bind->listen->accept->send/recv->close*/
int Handle_sever(void)
{ 
	int sock_fd, new_fd; 
	struct sockaddr_in server_addr;
	struct sockaddr client_addr;
	//struct sockaddr_in client_addr;
	int ret; 
	socklen_t addr_len; 
	pthread_t threadID;
	int client_num = -1; 

	signal(SIGCHLD,SIG_IGN); 

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
		new_fd = accept(sock_fd, &client_addr, &addr_len); //(struct sockaddr *)
		if( -1 == new_fd) 
		{ 
			fprintf( stderr, "accept error:%sna", strerror(errno)); 
			close(sock_fd); 
			exit( 1); 
		} 
		client_num++; 
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



int main(int argc, char* argv[]) 
{
//	int ret;
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return -1;
	}
	TRACE("1->InitProcessPool getpid[%d]fork[%d]\r\n",getpid(),pid);
	if(pid != 0)
		return 1;
	pid=setsid();
	TRACE("2->InitProcessPool getpid[%d]setsid[%d]\r\n",getpid(),pid);
	if(pid < 0)
	{
		TRACE("setsid failed\n");
		return 2;
	}
	
	TRACE("Create a shared memory Init\r\n");
	//----------------------------------------------------------------
//	umask(0);	//子进程权限	
	while(1) 
	{
		pid=fork();
		if (pid < 0)
		{
			perror("fork");
			return -2;
		}
		if (pid != 0)
		{//---守护进程-------
			pid_t _pid_t;
			_pid_t = wait(&pid); //---等待孙进程结束------
			printf("------reRunAPP-[%d,%d]---------\n",_pid_t,pid);
		} 
		else break; //孙进程，跳出外面执行
	} 
	//----------------------------------------------------------------
	//umask(0); //应用进程权限
	chdir("/");
	{
		int fd,efb,ofb;
		fd = open("/dev/null", O_RDWR, 0);
		efb = open("errlog", O_RDWR|O_CREAT, 0666);
		ofb = open("outLog", O_RDWR|O_CREAT, 0666);
		if (fd != -1)
		{
			dup2 (fd, STDIN_FILENO);
			if (fd > 2) close(fd);
			dup2 (ofb, STDOUT_FILENO);
			dup2 (efb, STDERR_FILENO);
		}
		close(efb);
		close(ofb);
	}
	return Handle_sever();
}



