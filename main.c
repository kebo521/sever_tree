
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
#define DEBUG_BUFF_MAX 	(8*1024)
#define DEBUG_DATE_LEN 	(21)

//typedef void *(*pthreadFuntion)(void *); 


typedef struct
{
	int client_times;
	int new_fd; 
	struct sockaddr_in client;
}def_sockdata;

int OsGetTimeStr(char *pOutStr)
{
	time_t timer;//long
	struct tm *pblock;
	timer = time(NULL);
	pblock = localtime(&timer);
	return sprintf(pOutStr,"%04d-%02d-%02d %02d:%02d:%02d",pblock->tm_year + 1900,pblock->tm_mon+1,pblock->tm_mday,pblock->tm_hour,pblock->tm_min,pblock->tm_sec);
}

void* EXP_LenSwap(def_sockdata* pFd)
{
	int new_fd = pFd->new_fd;
	int ret,offset;
	int len,offLog;
	char *pRecvBuff,*pSendBuff,*plogBuff,*pBuff;
	struct timeval timeout; 
	pBuff = (char*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX+DEBUG_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	plogBuff = pSendBuff + SEND_BUFF_MAX;
	//----------------------------------------------------------------------------------------------
	offLog=gLog(plogBuff+DEBUG_DATE_LEN,DEBUG_BUFF_MAX-DEBUG_DATE_LEN,"->Len8 connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
	offLog += gLogHex(plogBuff+DEBUG_DATE_LEN+offLog,DEBUG_BUFF_MAX-DEBUG_DATE_LEN-offLog,"sa_data",(u8*)pFd->client.sin_zero,sizeof(pFd->client.sin_zero));
	{
		char datetime[20];
		OsGetTimeStr(datetime);
		//---DEBUG_DATE_LEN-----
		plogBuff[0]='\r';
		plogBuff[1]='\n';
		memcpy(plogBuff+2,datetime,DEBUG_DATE_LEN-2);
	}
	//----------------------------------------------------------------------------------------------
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
			usleep(10*1000);
		}
		
		if(len > 2)
		{
			EXP_UNIT *pUnit;
			char *p=pRecvBuff;
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"l->RecvBuff",(u8*)pRecvBuff,len);
			pUnit=EXP_StrALL(&p,p+len);
			offset = 2;
			while(pUnit)
			{
				if(pUnit->type==DATA_DEFINE_STR)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result->STR[%s]\r\n",pUnit->pStr);
				if(pUnit->type==DATA_DEFINE_BUFF)
				{
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result->BUFF[%d]:\r\n",pUnit->Len);
					offset += ret;
					ret=gBcdtoStr_n(pUnit->pBuff,pUnit->Len,pSendBuff+offset,SEND_BUFF_MAX-offset);
				}
				else if(pUnit->type==DATA_DEFINE_TAG)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result[%x[%d]]\r\n",pUnit->Tag,pUnit->decimals);
				else if(pUnit->type==DATA_DEFINE_ASIC4)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result Asi:%s\r\n",pUnit->Asi);
				else if(pUnit->type==DATA_DEFINE_RULES)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result:%c%c\r\n",pUnit->pRule->ExpChars[0],pUnit->pRule->ExpChars[1]);
				else
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result type[%d],decimals[%d],Result[%d]\r\n",pUnit->type,pUnit->decimals,pUnit->Result);
				if(ret > 0) offset += ret;
				else break;
				pUnit=pUnit->pNext;
			}
			EXP_FreeUNIT(pUnit,0);
			pSendBuff[0]=(offset-2)/256;
			pSendBuff[1]=(offset-2)&0xff;
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"l->SendBuff",(u8*)pSendBuff,offset);
			send(new_fd, pSendBuff, offset, 0); 
			//Trace_PutMsg(pUnit);
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	
	free(pBuff);
	close(new_fd); 
	return NULL; 
}


void* EXP_StrSwap(def_sockdata* pFd)
{
	int new_fd = pFd->new_fd;
	int ret,offset;
	int offLog;
	char *pRecvBuff,*pSendBuff,*plogBuff,*pBuff;
	struct timeval timeout; 
	pBuff = (char*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX+DEBUG_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	plogBuff = pSendBuff + SEND_BUFF_MAX;
	//----------------------------------------------------------------------------------------------
	offLog=gLog(plogBuff+DEBUG_DATE_LEN,DEBUG_BUFF_MAX-DEBUG_DATE_LEN,"->Str6 connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
	offLog += gLogHex(plogBuff+DEBUG_DATE_LEN+offLog,DEBUG_BUFF_MAX-DEBUG_DATE_LEN-offLog,"sa_data",(u8*)pFd->client.sin_zero,sizeof(pFd->client.sin_zero));
	{
		char datetime[20];
		OsGetTimeStr(datetime);
		//---DEBUG_DATE_LEN-----
		plogBuff[0]='\r';
		plogBuff[1]='\n';
		memcpy(plogBuff+2,datetime,DEBUG_DATE_LEN-2);
	}
	//----------------------------------------------------------------------------------------------	
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;
	while(1)
	{ 
		//设置接收超时
		setsockopt(new_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)); 
		/* recv */
		offset = recv(new_fd, pRecvBuff, RECV_BUFF_MAX, 0); 
		if(offset <= 0)
		{ 
			fprintf(stderr,"fd[%d] recvn[%d] error:%sna\r\n",new_fd,offset, strerror(errno)); 
			break;
		}
	
		if(offset > 2)
		{
			EXP_UNIT *pUnit;
			char *p=pRecvBuff;
			
			pRecvBuff[offset]='\0';
			offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"s->RecvBuff[%s]",pRecvBuff);
			
			pUnit=EXP_StrALL(&p,p+offset);
			offset = 0;
			while(pUnit)
			{
				if(pUnit->type==DATA_DEFINE_STR)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result->STR[%s]\r\n",pUnit->pStr);
				if(pUnit->type==DATA_DEFINE_BUFF)
				{
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result->BUFF[%d]:\r\n",pUnit->Len);
					offset += ret;
					ret=gBcdtoStr_n(pUnit->pBuff,pUnit->Len,pSendBuff+offset,SEND_BUFF_MAX-offset);
				}
				else if(pUnit->type==DATA_DEFINE_TAG)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result[%x[%d]]\r\n",pUnit->Tag,pUnit->decimals);
				else if(pUnit->type==DATA_DEFINE_ASIC4)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result Asi:%s\r\n",pUnit->Asi);
				else if(pUnit->type==DATA_DEFINE_RULES)
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result:%c%c\r\n",pUnit->pRule->ExpChars[0],pUnit->pRule->ExpChars[1]);
				else
					ret=snprintf(pSendBuff+offset,SEND_BUFF_MAX-offset,"Result type[%d],decimals[%d],Result[%d]\r\n",pUnit->type,pUnit->decimals,pUnit->Result);
				if(ret > 0) offset += ret;
				else break;
				pUnit=pUnit->pNext;
			}
			EXP_FreeUNIT(pUnit,0);
			offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"s->SendBuff[%s]",pSendBuff);
			send(new_fd, pSendBuff, offset+1, 0); 
			//Trace_PutMsg(pUnit);
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	free(pBuff);
	close(new_fd); 
	return NULL; 
}

void* Handle_6666Sever(void* pFd)
{
	/* socket->bind->listen->accept->send/recv->close*/
	int sock_fd; 
	struct sockaddr_in server_addr;
	def_sockdata tSockData;
	//struct sockaddr_in client_addr;
	socklen_t addr_len; 
	pthread_t threadID;
	TRACE("Handle 6666Sever");
	tSockData.client_times = 0;
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
	server_addr.sin_port = htons(6666); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept error:%sna", strerror(errno)); 
			close(sock_fd); 
			exit( 1); 
		}
		tSockData.client_times ++ ;
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_StrSwap,&tSockData))	//(void * (*)(void *))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create error:%sna \r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}


void* Handle_8888Sever(void* pFd)
{
	/* socket->bind->listen->accept->send/recv->close*/
	int sock_fd; 
	struct sockaddr_in server_addr;
	def_sockdata tSockData;
	//struct sockaddr_in client_addr;
	socklen_t addr_len; 
	pthread_t threadID;
	TRACE("Handle 8888Sever");
	tSockData.client_times = 0;	
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
	server_addr.sin_port = htons(8888); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen error:%sna \r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 1); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept error:%sna", strerror(errno)); 
			close(sock_fd); 
			exit( 1); 
		} 
		tSockData.client_times++; 
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_LenSwap,&tSockData))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create error:%sna \r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}



int main(int argc, char* argv[]) 
{
	/* socket->bind->listen->accept->send/recv->close*/
	pthread_t t6ID,t8ID;
	int ret;
	{
		char buff[20];
		OsGetTimeStr(buff);
		TRACE("In Main argv[%s]-[%s]",argv[0],buff);
	}	
	{
		int fd;
		/*
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
		*/
		fd = open("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2(fd, STDIN_FILENO);
			if (fd > 2) close(fd);
		}
	}
	signal(SIGCHLD,SIG_IGN); 
	
	ret=pthread_create(&t6ID,NULL,&Handle_6666Sever,NULL);
	TRACE("create Handle 6666Sever[%d],id[%d]",ret,t6ID);
	if(ret)
	{
		fprintf(stderr,"pthread6create[%d] error:%sna \r\n",ret, strerror(errno)); 
	}
	ret=pthread_create(&t8ID,NULL,&Handle_8888Sever,NULL);
	TRACE("create Handle 8888Sever[%d],id[%d]",ret,t8ID);
	if(ret)
	{
		fprintf(stderr,"pthread8create[%d] error:%sna \r\n",ret, strerror(errno)); 
	}
	
	/* 等待线程pthread释放 */
	pthread_join(t8ID, NULL);
	pthread_join(t6ID, NULL);
	
	exit( 0); 
}



