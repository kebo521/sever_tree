
#include "communal.h"


#ifdef GTREE_FILE_COURCE

#define GFS_COUNTER_ADDR			(8*256-16)
#define GFS_COUNTER_SIZE			(16)
DfBalancedHead gtreebHead	=	{{0},NULL};
u32		gCountdata[32];

void APP_ShowSta(char* pTitle,char* msg)
{
	TRACE("ShowSta->[%s]--->%s\r\n",pTitle,msg);
}


int gTreeFile_Init(char* pTitle)
{
	int ret;
	APP_ShowSta(pTitle,"SPI_Flash_init");
	if(SPI_Flash_init())
	{
		TRACE("SPI_Flash_init ERR\r\n");
	}
	APP_ShowSta(pTitle,"SPI_Flash_Exist");
	if(SPI_Flash_Exist())
	{
		TRACE("SPI_Flash_Exist ERR\r\n");
	}

	APP_ShowSta(pTitle,"BalancedRecordInit");
	ret=BalancedRecordInit(&gtreebHead,GFS_COUNTER_ADDR,GFS_COUNTER_SIZE,sizeof(gCountdata));
	if(ret > 0)
	{
		ret=BalancedRecordRead(&gtreebHead,gCountdata);
		TRACE("gCountdata[%ld][%ld] Init\r\n",gCountdata[0],gCountdata[15]);
	}
	else
	{
		gCountdata[0]=0;
		gCountdata[1]=10;
		gCountdata[2]=100;
		gCountdata[3]=1000;
		gCountdata[4]=10000;
		gCountdata[15]=100000;
	}
	return 0;
}


int gTreeFile_Run(int Max)
{
	int ret,times,errs=0;
	TRACE("gTreeFile_Run->[%d]->",Max);
	for(times=0;times<Max;times++)
	{
		{
			gCountdata[0]++;
			gCountdata[1]++;
			gCountdata[2]++;
			gCountdata[3]++;
			gCountdata[4]++;
			gCountdata[15]+=7;
			ret=BalancedRecordSave(&gtreebHead,gCountdata);
		}

		{
			u32 data[32];
			ret=BalancedRecordRead(&gtreebHead,data);
			if(data[0]==gCountdata[0] && data[15]==gCountdata[15])
			{
				TRACE("gCountdata[%d][%d] OK.[%d]...",data[0],data[15],ret);
			}
			else
			{
				gCountdata[0]=data[0];
				gCountdata[15]=data[15];
				TRACE("gCountdata[%d][%d] Err[%d]****",data[0],data[15],ret);
				errs++;
			}
		}
	}
	#ifdef GTREE_FILE_ERR_RECORD
	TRACE("gTreeFile_Run Err[%d] SaveData[%d]SaveHear[%d],Erase[%d] End\r\n",errs,SaveDataErr,SaveHearErr,EraseTimes);
	#else
	TRACE("gTreeFile_Run Err[%d] End\r\n",errs);
	#endif
	
	return 0;
}



EXP_UNIT* EXP_TreeFileInit(EXP_UNIT* pInPar)
{
	if(pInPar)
	{
		int ret;
		ret=gTreeFile_Init((char*)pInPar->pBuff);
		free(pInPar->pStr);
		//--------填充结果------------------------
		pInPar->type=DATA_DEFINE_BOOL;
		pInPar->Result=ret;
	}
	else
		gTreeFile_Init("File_Init");
	
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
	gTreeFile_Run(Num);
	
	return pInPar;
}

//#define GFS_FILE_COUNTSTART			(4*256)
//#define GFS_FILE_COUNTSIZE			(16)
//#define GFS_FILE_START				(GFS_FILE_COUNTSTART+GFS_FILE_COUNTSIZE)
//#define GFS_FILE_END				(8*256)

#define GFS_FILE_COUNTSTART			(0)
#define GFS_FILE_COUNTSIZE			(8)
#define GFS_FILE_END				(4*256)


EXP_UNIT* EXP_FileSysInit(EXP_UNIT* pInPar)
{
	APP_ShowSta("FileSysInit","SPI_Flash_init");
	if(SPI_Flash_init())
	{
		TRACE("SPI_Flash_init ERR\r\n");
	}
	APP_ShowSta("FileSysInit","SPI_Flash_Exist");
	if(SPI_Flash_Exist())
	{
		TRACE("SPI_Flash_Exist ERR\r\n");
	}
	gfs_Init(GFS_FILE_COUNTSTART,GFS_FILE_END-GFS_FILE_COUNTSTART,GFS_FILE_COUNTSIZE);
	if(pInPar != NULL && pInPar->type==DATA_DEFINE_STR)
	{
		if(gfs_strcmp_path((u8*)pInPar->pStr,(u8*)"123456",'\0')>=0)
		{
			TRACE(">>>>>>>>>Fomat FileSystem<<<<<<<<<<<\r\n");
			gfs_Fomat(pInPar->pStr);
		}
	}
	return pInPar;
}

EXP_UNIT* EXP_FileSysDeinit(EXP_UNIT* pInPar)
{
	APP_ShowSta("FileSysDeinit","SPI_Flash_Deinit");
	SPI_Flash_Deinit();
	TRACE("SPI_Flash_Exit Ok\r\n");
	return pInPar;
}

	


DfGFsUnitfd* pFd_gfs=NULL;

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
	int ret=gfs_write(pFd_gfs,(u8*)pInPar->pStr,pInPar->Len);
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
	ret=gfs_read(pFd_gfs,(u8*)pData,pInPar->Len);
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

typedef struct
{
	char sName[32];	
	u32 fLen;		
	u8*	pBuff;			//优先级
}DfTestFS;


EXP_UNIT* EXP_FileSysAutoTest(EXP_UNIT* pInPar)
{
	DfTestFS tfileArr[124];
	u8	si,sMax;
	u16 i,max,fIndex,oflag;
	int ret,offset,SaveLen;
	u32 times,uRand,errTimes;
	u32 totalSave,totalRead,fileLen;
	u8* pCheckbuff;
	DfGFsUnitfd* pfd=NULL;
	max=sizeof(tfileArr)/sizeof(tfileArr[0]);
	for(i=0;i<max;i++)
	{
		tfileArr[i].sName[0]='\0';
		tfileArr[i].fLen=0;
		tfileArr[i].pBuff=(u8*)malloc(4*1024*1024);
		if(tfileArr[i].pBuff==NULL)
		{
			TRACE("gfs (%d)malloc Err",i);
		}
	}
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
		
		//TRACE("uRand[%x]fIndex[%d]sMax[%d]",uRand,fIndex,sMax);
		if(tfileArr[fIndex].sName[0]=='\0')
		{
			sprintf(tfileArr[fIndex].sName,"%d/%d",fIndex,rand());
			pfd=gfs_open((const char *)tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_CREAT);
			if(pfd==NULL)
			{
				TRACE("gfs_open CREAT[%s]Err",tfileArr[fIndex].sName);
				continue;
			}
			offset = 0;
			for(si=0;si<sMax;si++)
			{
				SaveLen =rand() % 0x00ff;
				if(SaveLen <= 0) continue;
				if((offset+SaveLen) >= 4*1024*1024)
				{
					TRACE("gfs_open[%s] getData[%d]Err",tfileArr[fIndex].sName,SaveLen);
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
					TRACE("gfs_write[%s]Err,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
					break;
				}
				if(ret < SaveLen)
				{
					TRACE("gfs_write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				offset += SaveLen;
				fileLen += ret;
			}
			tfileArr[fIndex].fLen=offset;
			gfs_close(pfd);
		}
		else
		{
			oflag=rand();
			
			if((oflag&0x03)==1)
			{
				SaveLen = 0;
				while(SaveLen<=0) SaveLen =rand() % 0x7fff;
				//TRACE("gfs open TRUNC[%s] fLen[%d]SaveLen[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen);
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[i]=(u8)rand();
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_TRUNC);
				if(pfd==NULL)
				{
					TRACE("gfs open TRUNC[%s]Err",tfileArr[fIndex].sName);
					continue;
				}
				fileLen -= tfileArr[fIndex].fLen;
				tfileArr[fIndex].fLen=0;
				
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("gfs_write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret > 0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen=SaveLen;
				}
				gfs_close(pfd);			
			}
			else if((oflag&0x03)==2)
			{	
				SaveLen = 0;
				while(SaveLen<=0) SaveLen =rand() % 0x7fff;
				offset = tfileArr[fIndex].fLen;
				if((offset+SaveLen) >= 4*1024*1024)
				{
					TRACE("offset[%s] getData[%d]Err",offset,SaveLen);
					continue;
				}
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u8)rand();
				}
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR|GFS_O_APPEND);
				if(pfd==NULL)
				{
					TRACE("gfs open TRUNC[%s]Err",tfileArr[fIndex].sName);
					continue;
				}
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff+offset,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("gfs_write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret>0)
				{
					fileLen += ret;
					tfileArr[fIndex].fLen += SaveLen;
				}
				gfs_close(pfd);
			}
			if((oflag&0x03)==3)
			{
				u32 maxLen,offset;
				
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("gfs open RDWR[%s]Err",tfileArr[fIndex].sName);
					continue;
				}		
				maxLen=gfs_GetSize(pfd);
				offset = 0;
				if (maxLen > 0)
				{					
					while(offset==0) offset = rand() % maxLen;
				}
				SaveLen = 0;
				while(1) 
				{
					SaveLen =rand() % 0x7fff;
					if(offset+SaveLen < 4*1024*1024) break;
				}
				//TRACE("gfs open TRUNC[%s] fLen[%d]SaveLen[%d]",tfileArr[fIndex].sName,tfileArr[fIndex].fLen,SaveLen);
				for(i=0;i<SaveLen;i++)
				{
					tfileArr[fIndex].pBuff[offset+i]=(u8)rand();
				}
				gfs_seek(pfd,offset,GFS_SEEK_SET);
				totalSave += SaveLen;
				ret=gfs_write(pfd,tfileArr[fIndex].pBuff+ offset,SaveLen);
				if(ret < SaveLen)
				{
					TRACE("gfs write[%s]warning,SaveLen[%d],ret[%d]",tfileArr[fIndex].sName,SaveLen,ret);
				}
				if(ret > 0)
				{				
					fileLen -= maxLen;
					if(maxLen <(offset+ret))
						maxLen = (offset+ret);
					fileLen += maxLen;
					tfileArr[fIndex].fLen=maxLen;	
				}
				gfs_close(pfd);			
			}
			else  
			{
				pfd=gfs_open(tfileArr[fIndex].sName,GFS_O_RDWR);
				if(pfd==NULL)
				{
					TRACE("gfs_open RDWR[%s]Err",tfileArr[fIndex].sName);
					errTimes++;
					continue;
				}
				offset = tfileArr[fIndex].fLen;
				pCheckbuff=(u8*)malloc(offset);
				totalRead += offset;
				ret=gfs_read(pfd,pCheckbuff,offset);
				if(ret < offset)
				{
					TRACE("gfs_read[%s],offset[%d],ret[%d] Err",tfileArr[fIndex].sName,offset,ret);
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
						TRACE("read cmp[%s],offset[%d,%d],ret[%d] Err",tfileArr[fIndex].sName,i,offset,ret);
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
			//TRACE("tfileArr[%d].sName[%s]fLen[%d]",i,tfileArr[i].sName,offset);
			pfd=gfs_open(tfileArr[i].sName,GFS_O_RDONLY);
			if(pfd==NULL)
			{
				TRACE("gfs open RDONLY[%s]Err",tfileArr[i].sName);
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
				TRACE("gfs read[%s],offset[%d],ret[%d] Err",tfileArr[i].sName,offset,ret);
				errTimes++;
			}
			else
			{
				ret=memcmp(pCheckbuff,tfileArr[i].pBuff,offset);
				if(ret)
				{
					TRACE("read cmp[%s],offset[%d],ret[%d] Err",tfileArr[i].sName,offset,ret);
					errTimes++;
				}
			}
			free(pCheckbuff);
			gfs_close(pfd);
		}
		free(tfileArr[i].pBuff);
	}
	TRACE("totalSave[%d]fileLen[%d]totalRead[%d] errTimes[%d]",totalSave,fileLen,totalRead,errTimes);
	return pInPar;
}


#endif
