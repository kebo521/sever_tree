
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
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


//#include "ks_type.h"

//#include "gMem.h"

//#include "hardinfo.h"
//#include "app_sdk.h"
//#include "voucher.h"

//#include "Wallet.h"
#include "gfs_api.h"


#define PORT_NUMBER 	8888
#define BACKLOG 		20

#define RECV_BUFF_MAX 	(8*1024)
#define SEND_BUFF_MAX 	(8*1024)
#define DEBUG_BUFF_MAX 	(8*1024)
#define DEBUG_DATE_LEN 	(21)

//typedef void *(*pthreadFuntion)(void *); 

int out_fd = -1,err_fd = -1;

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

int OsGetTickCount(void)
{
	struct timeval time_tv;
	gettimeofday(&time_tv, NULL);
	return (time_tv.tv_sec*1000 + time_tv.tv_usec/1000);
}

int Exp_StrOut(char *pSendBuff,int sendSize,int timeMs,EXP_UNIT *pUnit)
{
	int ret,offset=0;
	while(pUnit)
	{
		if(pUnit->type==DATA_DEFINE_STR)
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->STR[%s]\r\n",timeMs,pUnit->pStr);
		if(pUnit->type==DATA_DEFINE_BUFF)
		{
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->BUFF[%d]:\r\n",timeMs,pUnit->Len);
			offset += ret;
			ret=gBcdtoStr_n(pUnit->pBuff,pUnit->Len,pSendBuff+offset,SEND_BUFF_MAX-offset);
		}
		else if(pUnit->type==DATA_DEFINE_TAG)
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->[%x[%d]]\r\n",timeMs,pUnit->Tag,pUnit->decimals);
		else if(pUnit->type==DATA_DEFINE_ASIC4)
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->Asi:%s\r\n",timeMs,pUnit->Asi);
		else if(pUnit->type==DATA_DEFINE_RULES)
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->%c%c\r\n",timeMs,pUnit->pRule->ExpChars[0],pUnit->pRule->ExpChars[1]);
		else
			ret=snprintf(pSendBuff+offset,sendSize-offset,"Result[%d]->type[%d],decimals[%d],Result[%d]\r\n",timeMs,pUnit->type,pUnit->decimals,pUnit->Result);
		if(ret > 0) offset += ret;
		else break;
		pUnit=pUnit->pNext;
	}
	return offset;
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
	offLog = DEBUG_DATE_LEN;
	offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"->Len8 connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
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
			fprintf(stderr,"recv_Head2 fd[%d]ret[%d] error:[%s]\r\n",new_fd,ret, strerror(errno)); 
			break; 
		} 
		len = pRecvBuff[0]*256 + pRecvBuff[1];
		if(len < 3  || len > RECV_BUFF_MAX)
		{ 
			fprintf(stderr,"fd[%d] recv_Len[%02X %02X]error\r\n",new_fd,pRecvBuff[0],pRecvBuff[1]); 
			break; 
		}
		offset = 0;
		while(1)
		{
			ret = recv(new_fd, pRecvBuff+offset, len-offset, 0); 
			if(ret <= 0)
			{ 
				fprintf(stderr,"fd[%d]l recvn[%d,%d] error:[%s]\r\n",new_fd,offset,len, strerror(errno)); 
				break;
			}
			offset += ret;
			if(offset >= len) break;
			usleep(10*1000);
		}
		
		if(len > 2)
		{
			int startimeMs;
			EXP_UNIT *pUnit;
			char *p=pRecvBuff;
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," ->lRecvBuff",(u8*)pRecvBuff,len);
			startimeMs = OsGetTickCount();
			pUnit=EXP_StrALL(&p,p+len);
			offset = Exp_StrOut(pSendBuff+2,SEND_BUFF_MAX-2,OsGetTickCount()-startimeMs,pUnit);
			EXP_FreeUNIT(pUnit,0);
			pSendBuff[0]=offset/256;
			pSendBuff[1]=offset&0xff;
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," ->lSendBuff",(u8*)pSendBuff,offset);
			send(new_fd, pSendBuff, offset+2, 0); 
			//Trace_PutMsg(pUnit);
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	free(pBuff);
	close(new_fd); 
	fsync(out_fd);
	fsync(err_fd);
	return NULL; 
}


void* EXP_StrSwap(def_sockdata* pFd)
{
	int new_fd = pFd->new_fd;
	int offset;
	int offLog;
	char *pRecvBuff,*pSendBuff,*plogBuff,*pBuff;
	struct timeval timeout; 
	pBuff = (char*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX+DEBUG_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	plogBuff = pSendBuff + SEND_BUFF_MAX;
	//----------------------------------------------------------------------------------------------
	offLog = DEBUG_DATE_LEN;
	offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"->Str6 connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
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
	//设置接收超时
	setsockopt(new_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)); 
	while(1)
	{ 
		/* recv */
		offset = recv(new_fd, pRecvBuff, RECV_BUFF_MAX, 0); 
		if(offset <= 0)
		{ 
			fprintf(stderr,"fd[%d]s recvn[%d] error:[%s]\r\n",new_fd,offset, strerror(errno)); 
			break;
		}
	
		if(offset > 2)
		{
			int startimeMs;
			EXP_UNIT *pUnit;
			char *p=pRecvBuff;
			pRecvBuff[offset]='\0';
			offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," ->sRecvBuff:%s\r\n",pRecvBuff);
			startimeMs = OsGetTickCount();
			pUnit=EXP_StrALL(&p,p+offset);
			offset = Exp_StrOut(pSendBuff,SEND_BUFF_MAX,OsGetTickCount()-startimeMs,pUnit);
			EXP_FreeUNIT(pUnit,0);
			offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," ->sSendBuff:%s",pSendBuff);
			send(new_fd, pSendBuff, offset+1, 0); 
			//Trace_PutMsg(pUnit);
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	free(pBuff);
	close(new_fd); 
	fsync(out_fd);
	fsync(err_fd);
	return NULL; 
}

extern void WalletDataInit(void);
extern int WalletDataParseApdu(u8 *pAPDU,u8 *pOut);

extern void SetRecvWallet(void);
extern void SetPayWallet(void);


void* EXP_ApduSwapRecv(def_sockdata* pFd)
{
	int new_fd = pFd->new_fd;
	int offset,ret;
	int offLog;
	u8 *pRecvBuff,*pSendBuff,*pBuff;
	char *plogBuff;
	struct timeval timeout; 
	pBuff = (u8*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX+DEBUG_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	plogBuff = (char*)pSendBuff + SEND_BUFF_MAX;
	//----------------------------------------------------------------------------------------------
	offLog = DEBUG_DATE_LEN;
	offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"->APDU connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
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
	//设置接收超时
	setsockopt(new_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)); 
	while(1)
	{ 
		/* recv */
		offset = recv(new_fd, pRecvBuff, RECV_BUFF_MAX, 0); 
		if(offset <= 0)
		{ 
			fprintf(stderr,"fd[%d]s recvn[%d] error:[%s]\r\n",new_fd,offset, strerror(errno)); 
			break;
		}
	
		if(offset > 2)
		{
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," APDU->sRecvBuff",(u8*)pRecvBuff,offset);
			if(pRecvBuff[0] == 0x80)
			{
				SetRecvWallet();
				ret=WalletDataParseApdu(pRecvBuff,pSendBuff);
				offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," APDU->sSendBuff",pSendBuff,ret);
				send(new_fd, pSendBuff, ret, 0);
			}
			else
			{
				strcpy((char*)pSendBuff,"不可接收APDU");
				send(new_fd, pSendBuff, 12, 0); 
			}
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	free(pBuff);
	close(new_fd); 
	fsync(out_fd);
	fsync(err_fd);
	return NULL; 
}

void* EXP_ApduSwapPay(def_sockdata* pFd)
{
	int new_fd = pFd->new_fd;
	int offset,ret;
	int offLog;
	u8 *pRecvBuff,*pSendBuff,*pBuff;
	char *plogBuff;
	struct timeval timeout; 
	pBuff = (u8*)malloc(RECV_BUFF_MAX+SEND_BUFF_MAX+DEBUG_BUFF_MAX);
	pRecvBuff = pBuff;
	pSendBuff = pBuff + RECV_BUFF_MAX;
	plogBuff = (char*)pSendBuff + SEND_BUFF_MAX;
	//----------------------------------------------------------------------------------------------
	offLog = DEBUG_DATE_LEN;
	offLog += gLog(plogBuff+offLog,DEBUG_BUFF_MAX-offLog,"->APDU connet[%d]times[%d] sa[%d]Addr[%x,%d]\r\n",new_fd,pFd->client_times, \
		pFd->client.sin_family,pFd->client.sin_addr.s_addr,pFd->client.sin_port);
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
	//设置接收超时
	setsockopt(new_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)); 
	while(1)
	{ 
		/* recv */
		offset = recv(new_fd, pRecvBuff, RECV_BUFF_MAX, 0); 
		if(offset <= 0)
		{ 
			fprintf(stderr,"fd[%d]s recvn[%d] error:[%s]\r\n",new_fd,offset, strerror(errno)); 
			break;
		}
	
		if(offset > 2)
		{
			offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," APDU->sRecvBuff",(u8*)pRecvBuff,offset);
			if(pRecvBuff[0] == 0x80)
			{
				SetPayWallet();
				ret=WalletDataParseApdu(pRecvBuff,pSendBuff);
				offLog += gLogHex(plogBuff+offLog,DEBUG_BUFF_MAX-offLog," APDU->sSendBuff",pSendBuff,ret);
				send(new_fd, pSendBuff, ret, 0);
			}
			else
			{
				strcpy((char*)pSendBuff,"不可接收APDU");
				send(new_fd, pSendBuff, 12, 0); 
			}
		}
		else break;
	}
	plogBuff[offLog]='\0'; puts(plogBuff);
	free(pBuff);
	close(new_fd); 
	fsync(out_fd);
	fsync(err_fd);
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
		fprintf( stderr, "socket6 error:[%s]\r\n", strerror(errno)); 
		exit( 61); 
	} 
	/* set server sockaddr_in */
	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //clear
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY:This machine all IP
	server_addr.sin_port = htons(6666); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind6 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 62); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen6 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 63); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept6 error:[%d][%s]\r\n",tSockData.client_times, strerror(errno)); 
			close(sock_fd); 
			exit( 64); 
		}
		tSockData.client_times ++ ;
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_StrSwap,&tSockData))	//(void * (*)(void *))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create6 error:[%s]\r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 60); 
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
		fprintf( stderr, "socket8 error:[%s]\r\n", strerror(errno)); 
		exit( 81); 
	} 
	/* set server sockaddr_in */
	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //clear
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY:This machine all IP
	server_addr.sin_port = htons(8888); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind8 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 82); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen8 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 83); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept8 error:[%d][%s]\r\n",tSockData.client_times,strerror(errno)); 
			close(sock_fd); 
			exit( 84); 
		} 
		tSockData.client_times++; 
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_LenSwap,&tSockData))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create8 error:[%s]\r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}

void* Handle_ApduSeverRecv(void* pFd)
{
	/* socket->bind->listen->accept->send/recv->close*/
	int sock_fd; 
	struct sockaddr_in server_addr;
	def_sockdata tSockData;
	//struct sockaddr_in client_addr;
	socklen_t addr_len; 
	pthread_t threadID;
	TRACE("Handle ApduSever");
	tSockData.client_times = 0;	
	/* socket->bind->listen->accept->send/recv->close*/

	/* socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET:IPV4;SOCK_STREAM:TCP
	if( -1== sock_fd) 
	{ 
		fprintf( stderr, "socket5 error:[%s]\r\n", strerror(errno)); 
		exit( 81); 
	} 
	/* set server sockaddr_in */
	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //clear
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY:This machine all IP
	server_addr.sin_port = htons(5555); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind5 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 82); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen5 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 83); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept5 error:[%d][%s]\r\n",tSockData.client_times,strerror(errno)); 
			close(sock_fd); 
			exit( 84); 
		} 
		tSockData.client_times++; 
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_ApduSwapRecv,&tSockData))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create5 error:[%s]\r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}


void* Handle_ApduSeverPay(void* pFd)
{
	/* socket->bind->listen->accept->send/recv->close*/
	int sock_fd; 
	struct sockaddr_in server_addr;
	def_sockdata tSockData;
	//struct sockaddr_in client_addr;
	socklen_t addr_len; 
	pthread_t threadID;
	TRACE("Handle ApduSeverPay");
	tSockData.client_times = 0;	
	/* socket->bind->listen->accept->send/recv->close*/

	/* socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET:IPV4;SOCK_STREAM:TCP
	if( -1== sock_fd) 
	{ 
		fprintf( stderr, "socket3 error:[%s]\r\n", strerror(errno)); 
		exit( 81); 
	} 
	/* set server sockaddr_in */
	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //clear
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY:This machine all IP
	server_addr.sin_port = htons(3333); 
	/* bind */
	if( -1== bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) 
	{ 
		fprintf( stderr,"bind3 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 82); 
	} 
	/* listen */
	if( -1 == listen(sock_fd, BACKLOG)) 
	{ 
		fprintf(stderr, "listen3 error:[%s]\r\n", strerror(errno)); 
		close(sock_fd); 
		exit( 83); 
	} 

	/* accept */
	while( 1) 
	{ 
		addr_len = sizeof(struct sockaddr); 
		tSockData.new_fd = accept(sock_fd, (struct sockaddr *)&tSockData.client, &addr_len); //(struct sockaddr *)
		if( -1 == tSockData.new_fd) 
		{ 
			fprintf( stderr, "accept3 error:[%d][%s]\r\n",tSockData.client_times,strerror(errno)); 
			close(sock_fd); 
			exit( 84); 
		} 
		tSockData.client_times++; 
		/* send/recv */
		if(pthread_create(&threadID,NULL,(void * (*)(void *))&EXP_ApduSwapPay,&tSockData))
		{
			close(tSockData.new_fd);
			fprintf(stderr,"pthread_create3 error:[%s]\r\n", strerror(errno)); 
			continue;
		}
	} 
	/* close */
	close(sock_fd); 
	exit( 0); 
}


void UI_ShowMemMsg(char* pTitle,char* pMsg,int Mpar)
{
	char showErr[24];
	sprintf(showErr,pMsg,Mpar);
	TRACE(showErr);
}

#include "gMem.h"

int main(int argc, char* argv[]) 
{
	/* socket->bind->listen->accept->send/recv->close*/
	pthread_t t6ID,t8ID,t5ID,t3ID;
	u32 *pMemBuff;
	int ret;
	{
		char buff[20];
		OsGetTimeStr(buff);
		TRACE("In Main argv[%s]-[%s]",argv[0],buff);
		/*
		char sbuff[128];
		int offset;
		offset=gLog(sbuff,sizeof(sbuff),"In herr 111->\r\n");
		offset += gLogHex(sbuff+offset,sizeof(sbuff)-offset,"buff",(u8*)buff,sizeof(buff));
		gLog(sbuff+offset,sizeof(sbuff)-offset,"out herr OK \r\n");
		puts(sbuff);
		return 1;
		*/
	}	
	
	{
		int fd;
		char sName[64],datetime[20];
		time_t timer;//long
		struct tm *pblock;
		timer = time(NULL);
		pblock = localtime(&timer);
		sprintf(datetime,"%02d%02d%02d%02d%02d%02d",pblock->tm_year - 100,pblock->tm_mon+1,pblock->tm_mday,pblock->tm_hour,pblock->tm_min,pblock->tm_sec);
		sprintf(sName,"err%s.log",datetime);
		err_fd = open(sName, O_CREAT|O_RDWR, 0666);
		if (err_fd != -1)
		{
			dup2(err_fd, STDERR_FILENO);
		}
		sprintf(sName,"out%s.log",datetime);
		out_fd = open(sName, O_CREAT|O_RDWR, 0666);
		if (out_fd != -1)
		{
			dup2(out_fd, STDOUT_FILENO);
		}
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
		fprintf(stderr,"pthread6create[%d] error:[%s]\r\n",ret, strerror(errno)); 
	}
	ret=pthread_create(&t8ID,NULL,&Handle_8888Sever,NULL);
	TRACE("create Handle 8888Sever[%d],id[%d]",ret,t8ID);
	if(ret)
	{
		fprintf(stderr,"pthread8create[%d] error:[%s]\r\n",ret, strerror(errno)); 
	}

	pMemBuff= (u32 *)malloc(16*1024);
	gMemAllocInit(pMemBuff,16*1024,4);
	gLoadFunAllocShowMsg(UI_ShowMemMsg);
	SPI_Flash_init();
	api_gFileInit();
	WalletDataInit();
	
	ret=pthread_create(&t5ID,NULL,&Handle_ApduSeverRecv,NULL);
	TRACE("create Handle 555Sever[%d],id[%d]",ret,t5ID);
	if(ret)
	{
		fprintf(stderr,"pthread5create[%d] error:[%s]\r\n",ret, strerror(errno)); 
	}
	
	ret=pthread_create(&t3ID,NULL,&Handle_ApduSeverPay,NULL);
	TRACE("create Handle 555Sever[%d],id[%d]",ret,t3ID);
	if(ret)
	{
		fprintf(stderr,"pthread5create[%d] error:[%s]\r\n",ret, strerror(errno)); 
	}

	/* 等待线程pthread释放 */
	pthread_join(t8ID, NULL);
	pthread_join(t6ID, NULL);
	pthread_join(t5ID, NULL);
	pthread_join(t3ID, NULL);
	
	free(pMemBuff);
	TRACE("\r\n>>>>>>>sever main Handle out<<<<<<\r\n",ret,t8ID);
	if (err_fd != -1) close(err_fd);
	if (out_fd != -1) close(out_fd);
	exit( 0); 
}



