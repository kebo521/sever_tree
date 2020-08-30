
#include "gfs_api.h"
//#include "comm.h"


//#include "gfs.h"
//#include "gfs_util.h"
//#include "MercuryThread.h"
//#include "MercuryAudio.h"
//#include "MercuryCSTD.h"

#ifdef GTREE_FILE_COURCE



gFILE* gfs_open(const char *path, int flags ,...)
{
	u16 index,slen;
	DfGTreeFsFileName readName;

	index=gfs_ScanFile_BlockIndex(&readName,path,GFS_TYPE_BIN);
	if(index != GFS_INDEX_NULL)
	{
		gFILE *pfd =(gFILE *)malloc(sizeof(gFILE));
		if(pfd == NULL)
		{
			TRACE("gfs Retrievefiles1 malloc is not\r\n");
			//GFS_ERR_NOMEM
			return NULL;
		}
		memcpy_u32((u32*)&pfd->u.h.name,(u32*)&readName,sizeof(readName)/sizeof(u32));
		pfd->IndexGuide = index;
		pfd->IndexEntry = gfs_GetBlockIndex(index);
		gfs_FsReadblock(pfd->IndexEntry,sizeof(readName),((u8*)&pfd->u)+sizeof(readName),sizeof(pfd->u)-sizeof(readName));
		pfd->fOpenFlag = flags;
		pfd->fLen= ~pfd->u.h.InvLen;
		pfd->flagUpT = 0;
		pfd->flagUpH = 0;
		pfd->flagUpE = 0;
		pfd->flagNew = 0;
		if(flags&GFS_O_APPEND)
		{
			gfs_SetOffset(pfd,0,GFS_SEEK_END);
			pfd->flagNew = 4;
		}
		else 
		{
			if((flags&GFS_O_TRUNC) && pfd->fLen>0)
			{
				pfd->flagNew = 2;
				pfd->fLen = 0;
				//-----移除原有文件------
				/*
				pfd->u.h.InvLen = ~pfd->fLen;
				gfs_RecordDelete(&pfd->IndexEntry,1);
				gfs_RecordDelete(pfd->u.h.fdIndex,sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0]));
				//-----空间不够执行------------
				gfs_RecordBlockIndex(pfd->IndexGuide,GFS_INDEX_NULL);	//清除入口地址
				gfs_BalancedGlobalSave(1);	//存标签
				gfs_CheckEraseBlockBit();
				pfd->IndexGuide=gfs_GetNewBlockNum();
				*/
			}
			pfd->fOffset = 0;
			pfd->IndexB = 0;
			pfd->bOffet = sizeof(pfd->u.h);
		}
		return pfd;
	}
	if(flags&GFS_O_CREAT)
	{
		slen = strlen(path);
		if(slen < 1 && slen >= offsize(DfGTreeFsFileName,filepath))
		{
			TRACE_HEX("##filepath is Err",(void*)path,32);
			return NULL;
		}
		index=gfs_GetNewBlockNum();
		if(index != GFS_INDEX_NULL)
		{
			u16 offBlock = gfs_GetNextNullBlock();
			if(offBlock != GFS_INDEX_NULL)
			{
				gFILE *pfd =(gFILE *)malloc(sizeof(gFILE));
				if(pfd == NULL)
				{
					TRACE("gfs Retrievefiles2 malloc is not\r\n");
					//GFS_ERR_NOMEM
					return NULL;
				}
				gfs_RecordBlockIndex(index,offBlock);	
				memset_u32((u32*)&pfd->u.h,0xFFFFFFFF,sizeof(pfd->u.h)/sizeof(u32));
				strcpy(pfd->u.h.name.filepath,(char*)path);
				pfd->u.h.name.fType = GFS_TYPE_BIN;
				pfd->IndexGuide = index;
				pfd->IndexEntry = offBlock;
				pfd->fOpenFlag = flags;
				pfd->fLen = 0;
				pfd->fOffset = 0;
				pfd->IndexB = 0;
				pfd->bOffet = sizeof(pfd->u.h);
				pfd->flagUpT = 1;
				pfd->flagUpH = 0x40;	//申请空间标记
				pfd->flagUpE = 0;
				pfd->flagNew = 1;
				return pfd;
			}
			gfs_RecordBlockIndex(index,GFS_INDEX_NULL); //清除入口地址
			TRACE("gfs Retrievefiles GetNextNullBlock is not\r\n");
			//GFS_ERR_NOSPC
			return NULL;
		}
		TRACE("gfs Retrievefiles BlockIndex is full\r\n");
	}
	return NULL;
}

int gfs_seek(gFILE *pfd,int offset, int whence)
{
	if(pfd==NULL) return -1;
	if((u32)offset > pfd->fLen) return -2;
	return gfs_SetOffset(pfd,offset,whence);
}

int gfs_read(gFILE* pfd,void *buff,int size)
{
	int offset;
	u16 bOffset,bMax;
	if(pfd==NULL) return GFS_ERR_DIR;
	if(!(pfd->fOpenFlag&GFS_O_RDONLY))
		return GFS_ERR_NORD_PER;
	
	if(size > (int)(pfd->fLen - pfd->fOffset))
		size = (int)(pfd->fLen - pfd->fOffset);
	if(size <= 0) return 0;

	bMax = GFS_BLOCK_SIZE - pfd->bOffet;
	bOffset = pfd->bOffet;
	offset = 0;
	while(offset < size)
	{
		//----------------------check IndexB and fdIndex[pfd->IndexB]-----------------------
		if(pfd->IndexB > (sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])))
		{
			TRACE("##gfs read IndexB[%d] ERR\r\n",pfd->IndexB);
			return -2;
		}
		//-------------------------------------------------------------------------------
		if(bMax > (size-offset)) 
			bMax = (size-offset);

		if(buff)
		{
			if(pfd->IndexB==0)	//第一块内容从RAM中读取
			{
				if(bOffset < sizeof(pfd->u.h) || bOffset >= sizeof(pfd->u))
				{
					TRACE("gfs read IndexB=0,bOffset[%d] ERR\r\n",bOffset);
					return -5;
				}
				memcpy((u8*)buff+offset,((u8*)&pfd->u)+bOffset,bMax);
			}
			else if(pfd->u.h.fdIndex[pfd->IndexB - 1] != GFS_INDEX_NULL)
			{
				gfs_FsReadblock(pfd->u.h.fdIndex[pfd->IndexB - 1],bOffset,(u8*)buff+offset,bMax);
			}
			else
			{
				TRACE("###gfs read fdIndex[%d] offset[%d][%d]ERR\r\n",pfd->IndexB,offset,bMax);
				return GFS_ERR_NOSPC;
			}
		}
		offset += bMax;
		bOffset += bMax;

		if(bOffset >= GFS_BLOCK_SIZE)
		{
			bOffset -= GFS_BLOCK_SIZE;
			pfd->IndexB ++;
		}
		bMax = GFS_BLOCK_SIZE;
	}
	pfd->bOffet = bOffset;
	pfd->fOffset += offset;
	return offset;
}

int gfs_write(gFILE* pfd,void *buff,int writeLen)
{
	int offset,ret;
	int bMax;
	u16 bOffset,sOffset;	//save_Index;
	u16 new_Index,old_Index;
	//-----处理中间插入数据-----
	int saveLen=0;
	u8* pBlockBuff;
	u8 *pSave;

	if(pfd==NULL) return GFS_ERR_DIR;
	if(writeLen <= 0) return writeLen;
	if(buff==NULL) return GFS_ERR_LEN;
	if(!(pfd->fOpenFlag&GFS_O_WRONLY))
		return GFS_ERR_NOWR_PER;
	
	bMax = GFS_BLOCK_SIZE - pfd->bOffet;
	bOffset = pfd->bOffet;
	offset = 0;
	pBlockBuff=NULL;
	old_Index=GFS_INDEX_NULL;
	while(offset < writeLen)
	{
		//----------------------check IndexB and fdIndex[pfd->IndexB]-----------------------
		if(pfd->IndexB > (sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])))
		{
			TRACE("gfs write IndexB[%d>%d] ERR\r\n",pfd->IndexB,(sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])));
			return -2;
		}
		//-------------------------------------------------------------------------------
		if(bMax > (writeLen-offset))
			bMax = (writeLen-offset);
		if(pfd->IndexB == 0)
		{
			if(bOffset < sizeof(pfd->u.h) || bOffset >= sizeof(pfd->u))
			{
				TRACE("gfs write IndexB=0,bOffset[%d] ERR\r\n",bOffset);
				return -5;
			}
			memcpy((u8*)(&pfd->u) + bOffset,(u8*)buff,bMax);
			if((bOffset-sizeof(pfd->u.h)) < (~pfd->u.h.InvLen))
				pfd->flagUpH |= (0x10|0x80);
			else 
				pfd->flagUpH |= 0x10;
		}
		else //pfd->IndexB > 0
		{
			new_Index = pfd->u.h.fdIndex[pfd->IndexB-1];
			if(new_Index != GFS_INDEX_NULL)
			{
			//	save_Index = pfd->u.h.fdIndex[IndexB];	//避免早请不到空间，无法写入新空间?				
				if(bMax < GFS_BLOCK_SIZE)
				{
					ret = (~pfd->u.h.InvLen);
					if(ret < pfd->fLen)
						ret = pfd->fLen;
					if((pfd->fOffset + offset) < ret)
					{//--不允许在中间原来的块上写入内容---
						pBlockBuff=(u8*)malloc(GFS_BLOCK_SIZE);
						gfs_FsReadblock(new_Index,0,pBlockBuff,GFS_BLOCK_SIZE);
						for(saveLen=0;saveLen<bMax;saveLen++)
						{//-------对原来空间进行检查--如果可以写入直接写入---------
							if(((u8*)buff)[offset+saveLen]!=(pBlockBuff[saveLen+bOffset] & ((u8*)buff)[offset+saveLen]))
								break;
						}
						if(saveLen < bMax)
						{//-------原来空间不满足写入条件--重新换块--------
							memcpy(pBlockBuff+bOffset,(u8*)buff+offset,bMax);
							if(pfd->fLen >  (pfd->fOffset + offset)+bMax)
							{
								saveLen =bOffset+(pfd->fLen-(pfd->fOffset + offset));
								if((u32)saveLen > GFS_BLOCK_SIZE) saveLen=GFS_BLOCK_SIZE;
							}
							else
							{
								saveLen = bOffset+bMax;
							}
							old_Index = new_Index;
							new_Index = GFS_INDEX_NULL;
							pfd->flagUpH |= 0x80;
						}
						else
						{//----原来空间满足写入条件---直接写原来空间----
							free(pBlockBuff); pBlockBuff=NULL;
						}
					}
				}
				else
				{//---满足全块写入--直接切换块，不需要保存-----
					old_Index = new_Index;
					new_Index = GFS_INDEX_NULL;
					pfd->flagUpH |= 0x80;
				}
			}
		ReWriteData_Addr:
			if(new_Index == GFS_INDEX_NULL)
			{
				new_Index = gfs_GetNextNullBlock();
				if(new_Index == GFS_INDEX_NULL)
				{
					if(old_Index != GFS_INDEX_NULL)
					{//-------申请不到空间写原来位置-----------
						gfs_FsEraseblock(old_Index);	//写入失败察除再写入
						new_Index = old_Index;
						gfs_UndoRecordDelete(&old_Index,1);
						old_Index = GFS_INDEX_NULL;
						TRACE("**write GetNextNullBlock1[%d] save_Index[%d]\r\n",pfd->IndexB, new_Index);
					}
					else
					{
						TRACE("##gfs write GetNextNullBlock1[%d] [%d]ERR\r\n",pfd->IndexB,pfd->flagNew);
						if(pBlockBuff) free(pBlockBuff);
						break;
					}
				}
			}
			
			if(pBlockBuff)
			{
				pSave = pBlockBuff;
				sOffset =0;
			}
			else
			{
				pSave = (u8*)buff+offset;
				saveLen = bMax;
				sOffset =bOffset;
			}
			
			ret = gfs_FsWriteblock(new_Index,sOffset,pSave,saveLen);
			if(ret != saveLen)
			{//-----------写失败后处理------------------
			//	TRACE("gfs write bMax[%d] ->fdIndex[%d]=[%X] ERR\r\n",bMax,pfd->IndexB,pfd->u.h.fdIndex[pfd->IndexB]);
				if(bMax < GFS_BLOCK_SIZE && pBlockBuff==NULL)
				{//---------------------后补写失败------------------------------
					if((int)(pfd->fOffset + offset) < pfd->fLen || bOffset >0)
					{//--不允许在中间原来的块上写入内容---
						pBlockBuff=(u8*)malloc(GFS_BLOCK_SIZE);
						gfs_FsReadblock(new_Index,0,pBlockBuff,GFS_BLOCK_SIZE);
						memcpy(pBlockBuff+bOffset,(u8*)buff+offset,bMax);
						saveLen = GFS_BLOCK_SIZE;//bOffset+bMax;
					}
				}
				gfs_RecordDelete(&new_Index,1);
				//new_Index=NULL;
				goto ReWriteData_Addr;
				/*
				gfs_FsEraseblock(new_Index); //写入失败察除再写入
				ret = gfs_FsWriteblock(new_Index,sOffset,pSave,saveLen);
				if(ret != saveLen)
				{
					gfs_RecordDelete(&new_Index,1);
					pfd->flagUpT |= 0x80;
					pfd->flagUpE = 1;
					pfd->flagUpH |= (0x80|0x40);
					TRACE("*****Write_FsWriteblock[%d,%d]",offset,writeLen);
					continue;
				}
				*/
			}
			pfd->u.h.fdIndex[pfd->IndexB-1]=new_Index;
			pfd->flagUpH |= 0x20;
			if(old_Index != new_Index && old_Index != GFS_INDEX_NULL)
			{//---------有更新块，察除原来块--------------
				gfs_RecordDelete(&old_Index,1);
				old_Index = GFS_INDEX_NULL;
				pfd->flagUpT |= 0x80;
				pfd->flagUpE = 1;
			}
			
			if(pBlockBuff)
			{
				free(pBlockBuff); pBlockBuff=NULL;
			}
		}
		//--------------------------------------------------------------------
		offset += bMax;
		bOffset += bMax;
		if(bOffset >= GFS_BLOCK_SIZE)
		{
			bOffset -= GFS_BLOCK_SIZE;
			pfd->IndexB++;
		}
		bMax = GFS_BLOCK_SIZE;
	}
	pfd->bOffet = bOffset;
	pfd->fOffset += offset;
	if(pfd->fOffset > pfd->fLen)
	{
		pfd->fLen =  pfd->fOffset;
		pfd->flagUpH |= 0x40;
	}
    return offset;
}

int gfs_close(gFILE* pfd)
{
	//u32 nLen,oLen;
	int ret;
	u16 bMax,save_IndexEntry;
	if(pfd==NULL) 
		return -1;
	//TRACE("============gfs_close==============");
	if(pfd->fLen == 0)
	{
		if(pfd->flagNew == 1)
		{//--------创建而没有使用--------
			gfs_RecordBlockIndex(pfd->IndexGuide, GFS_INDEX_NULL);
			if(pfd->flagUpH & 0x20)
			{
				gfs_FsEraseblock(pfd->IndexEntry);	
			}
			gfs_RecordDelete(&pfd->IndexEntry,1);
			free(pfd);
			return 0;
		}
		else if(pfd->flagUpH & 0x20) //有写的动作
		{//--------清除写入没有成功------------
			gfs_RecordDelete(pfd->u.h.fdIndex,(sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])));
			gfs_RecordDelete(&pfd->IndexEntry,1);
			gfs_RecordBlockIndex(pfd->IndexGuide,GFS_INDEX_NULL);	//清除入口地址
			pfd->flagUpT = 0x80;
			pfd->flagUpE = 1;
			pfd->flagUpH = 0;
		}
		else
		{
			free(pfd);
			return 0;
		}
	}
	//nLen = pfd->fLen;
	//oLen = ~ pfd->u.h.InvLen;
	ret = 0;
	if(pfd->flagUpH)
	{
		u32 oldLen = ~ pfd->u.h.InvLen;
		if((pfd->fLen > oldLen)||((pfd->flagNew == 2) && (pfd->fLen < oldLen)))
		{
			if(pfd->fLen < oldLen)
			{//-------GFS_O_TRUNC---------------更新内容没有原来那么长------------
				int Offset;
				if(pfd->fLen <= sizeof(pfd->u.fData)) Offset=sizeof(pfd->u.fData);
				else 
					Offset = sizeof(pfd->u.fData)+DfGetBeiSu(pfd->fLen-sizeof(pfd->u.fData),GFS_BLOCK_SIZE);
				if(Offset < (int)oldLen)
				{
					int i=0;
					Offset -= sizeof(pfd->u.fData);
					while(Offset > 0)
					{
						i++;
						Offset -= GFS_BLOCK_SIZE;
					}
					gfs_RecordDelete(&pfd->u.h.fdIndex[i],(sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0]))-i);
					pfd->flagUpE = 1;
					pfd->flagUpH |= 0x80;
				}
			}
			oldLen = ~pfd->fLen;
			if(oldLen ^ (oldLen & pfd->u.h.InvLen))
				pfd->flagUpH |= 0x80;
			pfd->u.h.InvLen = oldLen;
			pfd->flagUpH |= 1;
		}
	
		if(pfd->fLen >= sizeof(pfd->u.fData))
			bMax = sizeof(pfd->u);
		else 
			bMax = sizeof(pfd->u.h) + pfd->fLen;
		
		save_IndexEntry=pfd->IndexEntry;
		if(pfd->flagUpH&0x80)	//pfd->flagNew > 1 || 
		{
			pfd->IndexEntry=GFS_INDEX_NULL;
		}
		while(1)
		{
			if(pfd->IndexEntry == GFS_INDEX_NULL)
			{
				pfd->IndexEntry = gfs_GetNextNullBlock();
				if(pfd->IndexEntry != GFS_INDEX_NULL)
				{
					pfd->flagUpH |= (0x80|0x40);
				}
				else if(save_IndexEntry != GFS_INDEX_NULL)
				{
					gfs_FsEraseblock(save_IndexEntry);	//写入失败察除再写入
					pfd->IndexEntry=save_IndexEntry;
					gfs_UndoRecordDelete(&save_IndexEntry,1);
					save_IndexEntry = GFS_INDEX_NULL;
				}
				if(pfd->IndexEntry == GFS_INDEX_NULL)
					break;
			}
			//------------Write Head------------------------------------
			ret = gfs_FsWriteblock(pfd->IndexEntry,0,&pfd->u,bMax);
			if(ret == bMax) break;
			//-----------写失败后处理------------------
			//TRACE("*****close_FsWriteblock[%x][%d,%X][%d,%d][%d][%X,%X]",
			//pfd,pfd->flagNew,pfd->flagUpH,pfd->IndexEntry,save_IndexEntry,ret,oLen,nLen);
			gfs_RecordDelete(&pfd->IndexEntry,1);
			pfd->flagUpT |= 0x80;
			pfd->flagUpE = 1;
			if(save_IndexEntry == GFS_INDEX_NULL) break;
		}
		
		if(pfd->IndexEntry == GFS_INDEX_NULL)
		{//---文件头存无法存储--文件所有内容必需清空----
			TRACE("#####NOSPC close ERR file[%s]delete[%d]\r\n",pfd->u.h.name.filepath,pfd->IndexEntry);
			gfs_RecordDelete(pfd->u.h.fdIndex,(sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])));
			gfs_RecordDelete(&pfd->IndexEntry,1);
			pfd->flagUpE = 1;
			gfs_RecordBlockIndex(pfd->IndexGuide,GFS_INDEX_NULL);	//清除入口地址
			pfd->flagUpT |= 0x80;
			ret = GFS_ERR_NOSPC;
		}
		else if(save_IndexEntry != pfd->IndexEntry && save_IndexEntry != GFS_INDEX_NULL)	//(save_IndexEntry != pfd->IndexEntry)
		{
			gfs_RecordDelete(&save_IndexEntry,1);
			pfd->flagUpE = 1;
			gfs_RecordBlockIndex(pfd->IndexGuide,pfd->IndexEntry);	//更新入口地址
			pfd->flagUpT |= 0x80;
		}
		/*
		if(pfd->flagUpH & 0x40)
		{//-----执行过 gfs_GetNextNullBlock 需要重新检查空间----
			gfs_UndoRecordDelete(pfd->u.h.fdIndex,sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0]));
			gfs_UndoRecordDelete(&pfd->IndexEntry,1);
		}
		*/
	}
	if(pfd->flagUpT)
	{
		if(pfd->flagUpT&0x80)
			gfs_BalancedGlobalSave(1);
		else
			gfs_BalancedGlobalSave(0);
	}
	if(pfd->flagUpE)
	{
		gfs_CheckEraseBlockBit();
	}
	free(pfd);
    return ret;
}

int gfs_GetSize(gFILE* pfd)
{
	if(pfd==NULL) return GFS_ERR_DIR;
	return pfd->fLen;
}

void* gfs_GetData2K(gFILE* pfd)
{
//	if(pfd==NULL) return NULL;
	return pfd->u.fData;
}

int gfs_remove(const char *path)
{
	gFILE* fd;
	fd=gfs_open(path,GFS_O_RDWR);
	if(fd == NULL) return 1;
	gfs_RecordDelete(fd->u.h.fdIndex,sizeof(fd->u.h.fdIndex)/sizeof(fd->u.h.fdIndex[0]));
	gfs_RecordDelete(&fd->IndexEntry,1);
	gfs_RecordBlockIndex(fd->IndexGuide,GFS_INDEX_NULL);	//清除入口地址
	gfs_BalancedGlobalSave(1);
	gfs_CheckEraseBlockBit();
	free(fd);
	return 0;	
}

int gfs_ReadSize(const char *path)
{	
	gFILE* fd;int rFsize;	
	fd=gfs_open(path,GFS_O_RDONLY); 
	if(fd==NULL) return 0; 
	rFsize=gfs_GetSize(fd); 
	gfs_close(fd);	
	return rFsize;	
}


void api_gFileInit(void)
{
	int ret;
	ret=gfs_Init(GFS_FILE_START,GFS_FILE_SIZE,GFS_FILE_COUNTSIZE);
	if(ret < 0)
	{
		TRACE("##gfs_AddreInit=%d\r\n",ret);
	}
}


#endif

