
#include "gfs_api.h"



#ifdef WINDOW_FLASH_SOFT

#include "comm.h"

//--------------4M-32*4096---28----------------------------
#define GFS_COUNTER_ADDR			(4*256-32)
#define GFS_COUNTER_SIZE			(16+12)
//-----------------终端参数区--4---------------------------------
#define GFS_PAR_ADDR				(4*256-4)
#define GFS_PAR_SIZE				(2)



DfBalancedRecord gAccountHead={0};

u32		gCountdata[32];
u32		gAcountdata[32];

void APP_ShowSta(char* pTitle,char* msg)
{
	TRACE("ShowSta->[%s]--->%s\r\n",pTitle,msg);
}

int gAccount_Init(char* pTitle)
{
	int ret;
	u32 data[32];
	APP_ShowSta(pTitle,"BalancedAccountInit");
	ret=BalancedAccountInit(&gAccountHead,GFS_COUNTER_ADDR,GFS_COUNTER_SIZE,sizeof(gAcountdata));
	if(ret > 0)
	{
		ret=BalancedAccountRead(&gAccountHead,0,data);
		if(data[0]==gAcountdata[0] && data[15]==gAcountdata[15])
		{
			TRACE("gAcountdata[%d][%d] OK.[%d]..Init",data[0],data[15],ret);
		}
		else
		{
			TRACE("###gAcountdata[%d,%d][%d,%d] Err[%d]**Init*",gAcountdata[0],data[0],gAcountdata[15],data[15],ret);
		}
	}
	else
	{
		gAcountdata[0]=0;
		gAcountdata[1]=10;
		gAcountdata[2]=100;
		gAcountdata[3]=1000;
		gAcountdata[4]=10000;
		gAcountdata[15]=100000;
		TRACE("Init Acountdata NULL****");
	}
	return 0;
}

int gAccount_Run(int Max)
{
	int ret,times,errs=0;
	for(times=0;times<Max;times++)
	{
		{
			gAcountdata[0]++;
			gAcountdata[1]++;
			gAcountdata[2]++;
			gAcountdata[3]++;
			gAcountdata[4]++;
			gAcountdata[15]+=7;
			ret=BalancedAccountSave(&gAccountHead,gAcountdata);
		}

		{
			u32 data[32];
			ret=BalancedAccountRead(&gAccountHead,0,data);
			if(data[0]==gAcountdata[0] && data[15]==gAcountdata[15])
			{
				TRACE("gAcountdata[%d][%d] OK.[%d]...",data[0],data[15],ret);
			}
			else
			{
				TRACE("###gAcountdata[%d,%d][%d,%d] Err[%d]**run*",gAcountdata[0],data[0],gAcountdata[15],data[15],ret);
				errs++;
			}
		}
	}
	TRACE("gTreeFile_Run Err[%d] End\r\n",errs);
	return 0;
}


EXP_UNIT* EXP_TreeFileInit(EXP_UNIT* pInPar)
{
	gAccount_Init("AcFile_Init");
	
	return pInPar;
}




EXP_UNIT* EXP_TreeFileRun(EXP_UNIT* pInPar)
{
	int Num=10;
	if(pInPar)
	{
		Num = pInPar->uData;
		//--------填充结果------------------------
		pInPar->type=DATA_DEFINE_BOOL;
		pInPar->Result=0;
	}
	gAccount_Run(Num);	
	return pInPar;
}

EXP_UNIT* EXP_FileSysInit(EXP_UNIT* pInPar)
{
	APP_ShowSta("FileSysInit","SPI Flash_init");
	if(SPI_Flash_init())
	{
		TRACE("SPI Flash_init ERR\r\n");
	}
	APP_ShowSta("FileSysInit","SPI_Flash_Exist");
	if(SPI_Flash_Exist())
	{
		TRACE("SPI_Flash_Exist ERR\r\n");
	}
	api_gFileInit();
	if(pInPar != NULL && pInPar->type==DATA_DEFINE_STR)
	{
		if(strcmp(pInPar->pStr,"123456")==0)
		{
			TRACE(">>>>>>>>>Fomat FileSystem<<<<<<<<<<<\r\n");
			gfs_Fomat(pInPar->pStr);
		}
	}
	return pInPar;
}

EXP_UNIT* EXP_FileSysDeinit(EXP_UNIT* pInPar)
{
	APP_ShowSta("FileSysDeinit","SPI Flash_Deinit");
	SPI_Flash_Deinit();
	TRACE("SPI_Flash_Exit Ok\r\n");
	return pInPar;
}

	


gFILE* pFd_gfs=NULL;

EXP_UNIT* EXP_FileSysOpen(EXP_UNIT* pInPar)
{
	TRACE("FileSysOpen[%x],next[%x]\r\n",pInPar,pInPar->pNext);
	pFd_gfs=gfs_open(pInPar->pStr,pInPar->pNext->uData);
	if(pFd_gfs)
	{
		TRACE("FileSysOpen[%s]OK,GetSize[%d]\r\n",pInPar->pStr,gfs_GetSize(pFd_gfs));
	}
	EXP_FreeUNIT(pInPar->pNext,0); pInPar->pNext=NULL;
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_BOOL;
	if(pFd_gfs == NULL)
		pInPar->Result=FALSE;
	else 
		pInPar->Result=TRUE;
	return pInPar;
}

EXP_UNIT* EXP_FileSysWrite(EXP_UNIT* pInPar)
{
	int ret=gfs_write(pFd_gfs,pInPar->pStr,pInPar->Len);
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_INT;
	pInPar->Result=ret;
	return pInPar;
}


EXP_UNIT* EXP_FileSysRead(EXP_UNIT* pInPar)
{
	char *pData;
	int ret;
	pInPar->Len=(u16)pInPar->uData;
	pData=(char*)malloc(pInPar->Len+1);
	ret=gfs_read(pFd_gfs,pData,pInPar->Len);
	TRACE("gfs_read=%d\r\n",ret);
	if(ret>=0)
	{
		pData[ret]='\0';
		//TRACE_HEX("read pData",pData,ret);
		pInPar->type=DATA_DEFINE_STR;
		pInPar->pStr=pData;
	}
	else
	{
		free(pData);
		pInPar->type=DATA_DEFINE_BOOL;
		pInPar->Result=FALSE;
	}
	return pInPar;
}

EXP_UNIT* EXP_FileSysSeek(EXP_UNIT* pInPar)
{
	int ret = gfs_seek(pFd_gfs, pInPar->uData, pInPar->pNext->uData);
	EXP_FreeUNIT(pInPar->pNext,0); pInPar->pNext=NULL;
	pInPar->type=DATA_DEFINE_INT;
	pInPar->Result=ret;
	return pInPar;
}

EXP_UNIT* EXP_FileSysClose(EXP_UNIT* pInPar)
{
	int ret=gfs_close(pFd_gfs);
	TRACE("gfs close ret=%d\r\n",ret);
	return pInPar;
}

EXP_UNIT* EXP_FileSysRemove(EXP_UNIT* pInPar)
{
	int ret=gfs_remove(pInPar->pStr);
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_INT;
	pInPar->Result=ret;
	return pInPar;
}
EXP_UNIT* EXP_FileSysCheck(EXP_UNIT* pInPar)
{
	int ret=gfs_checkALL();
	if(pInPar != NULL)
	{
		if(pInPar->type < DATA_DEFINE_BIN)
			free(pInPar->pStr);
		pInPar->type=DATA_DEFINE_INT;
		pInPar->Result=ret;
		return pInPar;
	}
	return NULL;
}


#if(0)

typedef struct
{
	char sName[32-4];	
	u32 fLen;		
	u8*	pBuff;			//优先级
}DfTestFS;

#define TEST_FINE_NUM_MAX	93		//124
#define MAX_SAVE_TREM_LEN	2048	//0x3FFF

#define MAX_SAVE_FILE_LEN	(8*1024)	//(10*1024)	//65535	//(2*1024*1024)
static DfTestFS tfileArr[TEST_FINE_NUM_MAX]={0};

EXP_UNIT* EXP_FileSysAutoTest(EXP_UNIT* pInPar)
{
	u8	si,sMax;
	u16 i,max,fIndex,oflag;
	int ret,offset,SaveLen;
	u32 it,times,uRand,errTimes;
	u32 totalSave,totalRead,fileLen;
	u8* pCheckbuff,*pSavebuff;
	gFILE* pfd=NULL;
	max=sizeof(tfileArr)/sizeof(tfileArr[0]);
	for(i=0;i<max;i++)
	{
		tfileArr[i].sName[0]='\0';
		tfileArr[i].fLen=0;
		tfileArr[i].pBuff=(u8*)malloc(MAX_SAVE_FILE_LEN);
		if(tfileArr[i].pBuff==NULL)
		{
			TRACE("###gfs (%d)malloc Err",i);
			return NULL;
		}
	}
	pSavebuff = (u8*)malloc(MAX_SAVE_FILE_LEN);
	if(pInPar)
		times=pInPar->uData;
	else times=7;
	for(uRand=0;uRand<times;uRand++)
		rand();
	totalSave=0;
	fileLen =0;
	totalRead=0;
	errTimes=0;
	TRACE("--->(%d)start",times);
	for(it=0;it<times;it++)
	{
		uRand=rand();
		fIndex=uRand%max;
		sMax = (uRand/256)%64;
		if((rand()%27) == 0)
		{
		//	TRACE("gfs_Init...");
			api_gFileInit();
		}
		
		//TRACE("uRand[%x]fIndex[%d]sMax[%d]",uRand,fIndex,sMax);
		if(tfileArr[fIndex].sName[0]=='\0' || tfileArr[fIndex].fLen == 0)
		{
			ret=sprintf_s(tfileArr[fIndex].sName,sizeof(tfileArr[fIndex].sName),"%d-%ld",fIndex,rand());
			pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_CREAT);
			if(pfd==NULL)
			{
				//TRACE("CREAT_open sName Len[%d]", ret);
				//TRACE_HEX("sName",tfileArr[fIndex].sName,ret);
				TRACE("CREAT_open[%d] name[%s]Err-[%d]-t->%d",fIndex,tfileArr[fIndex].sName,ret,it);
				tfileArr[fIndex].sName[0]='\0';
				continue;
			}
			offset = 0;
			for(si=0;si<sMax;si++)
			{
				SaveLen =rand() % MAX_SAVE_TREM_LEN;
				if(SaveLen <= 0) continue;
				if((offset+SaveLen) >= MAX_SAVE_FILE_LEN)
				{
					//TRACE("gfs_open[%s] getData[%d]Err",tfileArr[fIndex].sName,SaveLen);
					break;
				}
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u8)rand();
				}
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff+offset,SaveLen);
				if(ret <= 0)
				{
					TRACE("CREAT0 gfs write[%s]Err,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
					break;
				}
				if(ret < SaveLen)
				{
					TRACE("CREAT1 gfs write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				offset += ret;
				fileLen += ret;
			}
			tfileArr[fIndex].fLen=offset;
			gfs_close(pfd);
		}
		else	//Read----- Data
		{
			oflag=rand();
			
			if((oflag&0x03)==1)		// save 0
			{
				SaveLen = 0;
				while(SaveLen<=0) SaveLen =rand() % MAX_SAVE_FILE_LEN;
				//TRACE("gfs open TRUNC[%s] fLen[%d]SaveLen[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen);
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[i]=(u8)rand();
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_TRUNC);
				if(pfd==NULL)
				{
					TRACE("s##open TRUNC[%s][%d]Err->%d",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,it);
					errTimes++;
					continue;
				}
				fileLen -= tfileArr[fIndex].fLen;
				//tfileArr[fIndex].fLen=0;
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("TRUNC gfs write[%s][%d]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen,ret);
				}
				if(ret > 0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen=ret;
				}
				gfs_close(pfd);			
			}
			else if((oflag&0x03)==2)	//save add
			{	
				SaveLen = 0;
				while(SaveLen<=0) SaveLen =rand() % MAX_SAVE_TREM_LEN;
				offset = tfileArr[fIndex].fLen;
				if((offset+SaveLen) >= MAX_SAVE_FILE_LEN)
				{
					//TRACE("offset[%s] getData[%d]Err",offset,SaveLen);
					continue;
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_APPEND);
				if(pfd==NULL)
				{
					TRACE("s##open APPEND[%s][%d]Err->%d",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,it);
					errTimes++;
					continue;
				}
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u8)rand();
				}
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff+offset,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("APPEND_write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret>0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen += ret;
				}
				gfs_close(pfd);
			}
			if((oflag&0x03)==3)		//save x
			{
				u32 maxLen,offset;
				
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("s##open RDWR[%s][%d]Err->%d",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,it);
					errTimes++;
					continue;
				}	
				maxLen=gfs_GetSize(pfd);
				if((maxLen+2) >= MAX_SAVE_FILE_LEN)
				{
					gfs_close(pfd);
					continue;
				}
				offset = rand() % (maxLen+1);
				SaveLen = 0;
				while(1) 
				{
					SaveLen =rand() % MAX_SAVE_TREM_LEN;
					if(SaveLen <= 0) continue;
					if((offset+SaveLen) < MAX_SAVE_FILE_LEN) break;
				}
				//TRACE("gfs open TRUNC[%s] fLen[%d]SaveLen[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen);
				gfs_seek(pfd,offset,GFS_SEEK_SET);
				totalSave += SaveLen;
				ret=gfs_write(pfd,pSavebuff,SaveLen);
				if(ret <= 0)
				{
					TRACE("RDWR gfs write[%s]Err,SaveLen[%d],maxLen[%d]offset[%d]ret[%d]",tfileArr[fIndex].sName,SaveLen,maxLen,offset,ret);
				}
				else if(ret < SaveLen)
				{
					TRACE("RDWR write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret > 0)
				{		
					for(i=0;i<ret;i++)
					{
						tfileArr[fIndex].pBuff[offset+i]=pSavebuff[i];
					}
					fileLen += ret;
					if(maxLen <(offset+ret))
						maxLen = (offset+ret);
					tfileArr[fIndex].fLen=maxLen;	
				}
				gfs_close(pfd);			
			}
			else  
			{
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("e##gfs_open RDWR[%s][%d]Err->%d",tfileArr[fIndex].sName , tfileArr[fIndex].fLen,it);
					errTimes++;
					continue;
				}
				offset = tfileArr[fIndex].fLen;
				pCheckbuff=(u8*)malloc(offset);
				totalRead += offset;
				ret=gfs_read(pfd,pCheckbuff,offset);
				if(ret < offset)
				{//e##gfs_read[97/10605],offset[3603],ret[2048] Err
					TRACE("e##gfs_read[%s],offset[%d],ret[%d] Err->%d",tfileArr[fIndex].sName,offset,ret,it);
					errTimes++;
				}
				else
				{
					int i;
					u8 *pRead,*pTag;
					pRead=pCheckbuff;
					pTag=tfileArr[fIndex].pBuff;
					for(i=0;i<offset;i++)
					{
						if(pRead[i] != pTag[i]) break;
					}
					if(i < offset)
					{
						TRACE("e##read cmp[%s],offset[%d,%d],ret[%d] Err->%d",tfileArr[fIndex].sName,i,offset,ret,it);
						errTimes++;
					}
				}
				free(pCheckbuff);
				gfs_close(pfd);
				gfs_remove(tfileArr[fIndex].sName);
				tfileArr[fIndex].sName[0]='\0';
				tfileArr[fIndex].fLen=0;
			}
		}
	}
	#ifdef GTREE_FILE_ERR_RECORD
	TRACE("-check--totalSave[%d]fileLen[%d]totalRead[%d]gfs_erase_times[%d]",totalSave,fileLen,totalRead,gfs_erase_times);
	#else
	TRACE("-check--totalSave[%d]fileLen[%d]totalRead[%d]",totalSave,fileLen,totalRead);
	#endif
	//gfs_erase_times
	for(i=0;i<max;i++)
	{
		if(tfileArr[i].fLen > 0)
		{
			offset = tfileArr[i].fLen;
			if(offset <= 0) continue;
			//TRACE("tfileArr[%d].sName[%s]fLen[%d]",i,tfileArr[i].sName,offset);
			pfd=gfs_open(tfileArr[i].sName,GFS_O_RDONLY);
			if(pfd==NULL)
			{
				TRACE("e##gfs open RDONLY[%s][%d]Err",tfileArr[i].sName,tfileArr[i].fLen);
				errTimes++;
				continue;
			}
			if(offset != gfs_GetSize(pfd))
			{
				TRACE("e##gfs[%s] GetSize[%d] != Len[%d] Err",tfileArr[i].sName,gfs_GetSize(pfd),offset);
				gfs_close(pfd);
				errTimes++;
				continue;
			}
			pCheckbuff=(u8*)malloc(offset);
			if(pCheckbuff==NULL)
			{
				TRACE("gfs malloc(%d) Err",offset);
				gfs_close(pfd);
				continue;
			}
			totalRead += offset;
			ret=gfs_read(pfd,pCheckbuff,offset);
			if(ret < offset)
			{
				TRACE("e##gfs read[%s],offset[%d],ret[%d] Err",tfileArr[i].sName,offset,ret);
				errTimes++;
			}
			else
			{
				int j;
				u8 *pRead,*pTag;
				pRead=pCheckbuff;
				pTag=tfileArr[i].pBuff;
				for(j=0;j<offset;j++)
				{
					if(pRead[j] != pTag[j]) break;
				}
				if(j < offset)
				{
					TRACE("e##read cmp2[%s],offset[%d][%d][%d != %d] Err",tfileArr[fIndex].sName,offset,j,pRead[j],pTag[j]);
					errTimes++;
				}
			}
			free(pCheckbuff);
			gfs_close(pfd);
		}
		free(tfileArr[i].pBuff);
	}
	TRACE("totalSave[%d]fileLen[%d]totalRead[%d] errTimes[%d]",totalSave,fileLen,totalRead,errTimes);

	//gfs_checkALL();
	
	return pInPar;
}
#else
typedef struct
{
	char sName[32-4];	
	u32 fLen;		
	u8* pBuff;			//优先级
}DfTestFS;

#define TEST_FINE_NUM_MAX	43		//124
#define MAX_SAVE_TREM_LEN	256	//0x3FFF

#define MAX_SAVE_FILE_LEN	(512*4)	//(10*1024)	//65535	//(2*1024*1024)
static DfTestFS tfileArr[TEST_FINE_NUM_MAX];


EXP_UNIT* EXP_FileSysAutoTest(EXP_UNIT* pInPar)
{
	u8	si,sMax;
	u16 i,max,fIndex,oflag;
	int ret,offset,SaveLen;
	u32 times,uRand,errTimes;
	u32 totalSave,totalRead,fileLen;
	u8* pCheckbuff,*pSavebuff;
	gFILE* pfd=NULL;
	max=sizeof(tfileArr)/sizeof(tfileArr[0]);
	for(i=0;i<max;i++)
	{
		tfileArr[i].sName[0]='\0';
		tfileArr[i].fLen=0;
		tfileArr[i].pBuff=(u8*)malloc(MAX_SAVE_FILE_LEN);
		if(tfileArr[i].pBuff==NULL)
		{
			TRACE("###gfs (%d)malloc Err",i);
			return NULL;
		}
	}
	pSavebuff = (u8*)malloc(MAX_SAVE_FILE_LEN);
	if(pInPar)
		times=pInPar->uData;
	else times=7;
	for(uRand=0;uRand<times;uRand++)
		rand();
	totalSave=0;
	fileLen =0;
	totalRead=0;
	errTimes=0;
	while(times--)
	{
		uRand=rand();
		fIndex=uRand%max;
		sMax = (uRand/256)%64;
		if((rand()%27) == 0)
		{
			api_gFileInit();
		}
		//TRACE("uRand[%x]fIndex[%d]sMax[%d]",uRand,fIndex,sMax);
		if(tfileArr[fIndex].sName[0]=='\0' || tfileArr[fIndex].fLen == 0)
		{
			ret=sprintf(tfileArr[fIndex].sName,"%d-%d",fIndex,rand());
			pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_CREAT);
			if(pfd==NULL)
			{
				//TRACE("CREAT_open sName Len[%d]", ret);
				//TRACE_HEX("sName",tfileArr[fIndex].sName,ret);
				TRACE("CREAT_open[%d] name[%s]Err-[%d]",fIndex,tfileArr[fIndex].sName,ret);
				tfileArr[fIndex].sName[0]='\0';
				continue;
			}
			offset = 0;
			for(si=0;si<sMax;si++)
			{
				SaveLen =rand() % MAX_SAVE_TREM_LEN;
				if((SaveLen <= 0) || (SaveLen&0x03)) continue;
				if((offset+SaveLen) >= MAX_SAVE_FILE_LEN)
				{
					//TRACE("gfs_open[%s] getData[%d]Err",tfileArr[fIndex].sName,SaveLen);
					break;
				}
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u32)rand();
				}
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff +offset,SaveLen);
				if(ret <= 0)
				{
					TRACE("CREAT0 gfs write[%s]Err,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
					break;
				}
				if(ret < SaveLen)
				{
					TRACE("CREAT1 gfs write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				offset += ret;
				fileLen += ret;
			}
			tfileArr[fIndex].fLen=offset;
			gfs_close(pfd);
		}
		else	//Read----- Data
		{
			oflag=rand();
			
			if((oflag&0x03)==1)		// save 0
			{
				SaveLen = 0;
				while(SaveLen<=0 || (SaveLen&0x03)) SaveLen =rand() % MAX_SAVE_FILE_LEN;
				//TRACE("gfs open TRUNC[%s] fLen[%d]SaveLen[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen);
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[i]= rand();
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_TRUNC);
				if(pfd==NULL)
				{
					TRACE("s##open TRUNC[%s][%d]Err",tfileArr[fIndex].sName,tfileArr[fIndex].fLen);
					errTimes++;
					continue;
				}
				fileLen -= tfileArr[fIndex].fLen;
				//tfileArr[fIndex].fLen=0;
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("TRUNC gfs write[%s][%d]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen,ret);
				}
				if(ret > 0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen=ret;
				}
				gfs_close(pfd);			
			}
			else if((oflag&0x03)==2)	//save add
			{	
				SaveLen = 0;
				while(SaveLen<=0 || (SaveLen&0x03)) SaveLen =rand() % MAX_SAVE_TREM_LEN;
				offset = tfileArr[fIndex].fLen;
				if((offset+SaveLen) >= MAX_SAVE_FILE_LEN)
				{
					//TRACE("offset[%s] getData[%d]Err",offset,SaveLen);
					continue;
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_APPEND);
				if(pfd==NULL)
				{
					TRACE("s##open APPEND[%s][%d]Err",tfileArr[fIndex].sName,tfileArr[fIndex].fLen);
					errTimes++;
					continue;
				}
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u32)rand();
				}
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff+offset,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("APPEND_write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret>0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen += ret;
				}
				gfs_close(pfd);
			}
			if((oflag&0x03)==3)		//save x
			{
				u32 maxLen,offset;
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("s##open RDWR[%s][%d]Err",tfileArr[fIndex].sName,tfileArr[fIndex].fLen);
					errTimes++;
					continue;
				}	
				maxLen=gfs_GetSize(pfd);
				if((maxLen+2) >= MAX_SAVE_FILE_LEN)
				{
					gfs_close(pfd);	
					continue;
				}
				while(1)
				{
					offset = rand() % (maxLen+1);
					if(offset&0x03) continue;
					break;
				}
				while(1) 
				{
					SaveLen =rand() % MAX_SAVE_TREM_LEN;
					if(SaveLen <= 0|| (SaveLen&0x03)) continue;
					if((offset+SaveLen) < MAX_SAVE_FILE_LEN) break;
				}
				gfs_seek(pfd,offset,GFS_SEEK_SET);
				totalSave += SaveLen;
				ret=gfs_write(pfd,pSavebuff,SaveLen);
				if(ret <= 0)
				{
					TRACE("RDWR gfs write[%s]Err,SaveLen[%d],maxLen[%d]offset[%d]ret[%d]",tfileArr[fIndex].sName,SaveLen,maxLen,offset,ret);
				}
				else if(ret < SaveLen)
				{
					TRACE("RDWR write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret > 0)
				{		
					for(i=0;i<ret;i++)
					{
						tfileArr[fIndex].pBuff[offset+i]=pSavebuff[i];
					}
					fileLen += ret;
					if(maxLen <(offset+ret))
						maxLen = (offset+ret);
					tfileArr[fIndex].fLen=maxLen;	
				}
				gfs_close(pfd);			
			}
			else  //dellet
			{
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("e##gfs_open RDWR[%s][%d]Err",tfileArr[fIndex].sName , tfileArr[fIndex].fLen);
					errTimes++;
					continue;
				}
				offset = tfileArr[fIndex].fLen;
				pCheckbuff=(u8*)malloc(offset);
				totalRead += offset;
				ret=gfs_read(pfd,pCheckbuff,offset);
				if(ret < offset)
				{//e##gfs_read[97/10605],offset[3603],ret[2048] Err
					TRACE("e##gfs_read[%s],offset[%d],ret[%d] Err",tfileArr[fIndex].sName,offset,ret);
					errTimes++;
				}
				else
				{
					int i;
					u8 *pRead,*pTag;
					pRead=pCheckbuff;
					pTag=tfileArr[fIndex].pBuff;
					for(i=0;i<offset;i++)
					{
						if(pRead[i] != pTag[i]) break;
					}
					if(i < offset)
					{
						TRACE("e##read cmp[%s],offset[%d,%d],ret[%d] Err",tfileArr[fIndex].sName,i,offset,ret);
						errTimes++;
					}
				}
				free(pCheckbuff);
				gfs_close(pfd);
				gfs_remove(tfileArr[fIndex].sName);
				tfileArr[fIndex].sName[0]='\0';
				tfileArr[fIndex].fLen=0;
			}
		}
	}
	TRACE("totalSave[%d]fileLen[%d]totalRead[%d]gfs_erase_times[%d]",totalSave,fileLen,totalRead,gfs_erase_times);
	//gfs_erase_times
	for(i=0;i<max;i++)
	{
		if(tfileArr[i].fLen > 0)
		{
			offset = tfileArr[i].fLen;
			if(offset <= 0) continue;
			//TRACE("tfileArr[%d].sName[%s]fLen[%d]",i,tfileArr[i].sName,offset);
			pfd=gfs_open(tfileArr[i].sName,GFS_O_RDONLY);
			if(pfd==NULL)
			{
				TRACE("e##gfs open RDONLY[%s][%d]Err",tfileArr[i].sName,tfileArr[i].fLen);
				errTimes++;
				continue;
			}
			if(offset != gfs_GetSize(pfd))
			{
				TRACE("e##gfs[%s] GetSize[%d] != Len[%d] Err",tfileArr[i].sName,gfs_GetSize(pfd),offset);
				gfs_close(pfd);
				errTimes++;
				continue;
			}
			pCheckbuff=(u8*)malloc(offset);
			if(pCheckbuff==NULL)
			{
				TRACE("gfs malloc(%d) Err",offset);
				gfs_close(pfd);
				continue;
			}
			totalRead += offset;
			ret=gfs_read(pfd,pCheckbuff,offset);
			if(ret < offset)
			{
				TRACE("e##gfs read[%s],offset[%d],ret[%d] Err",tfileArr[i].sName,offset,ret);
				errTimes++;
			}
			else
			{
				int j;
				u8 *pRead,*pTag;
				pRead=pCheckbuff;
				pTag=tfileArr[i].pBuff;
				for(j=0;j<offset;j++)
				{
					if(pRead[j] != pTag[j]) break;
				}
				if(j < offset)
				{
					TRACE("e##read cmp2[%s],offset[%d][%d][%d != %d] Err",tfileArr[fIndex].sName,offset,j,pRead[j],pTag[j]);
					TRACE("gfs GetSize[%d],fLen[%d]",gfs_GetSize(pfd),tfileArr[fIndex].fLen);
					errTimes++;
				}
			}
			free(pCheckbuff);
			gfs_close(pfd);
		}
		free(tfileArr[i].pBuff);
	}
	TRACE("totalSave[%d]fileLen[%d]totalRead[%d] errTimes[%d]",totalSave,fileLen,totalRead,errTimes);

	//gfs_checkALL();
	
	return pInPar;
}




#endif

#endif
