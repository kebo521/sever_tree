
#include "communal.h"


//#include "gfs.h"
//#include "gfs_util.h"
//#include "MercuryThread.h"
//#include "MercuryAudio.h"
//#include "MercuryCSTD.h"

#ifdef GTREE_FILE_COURCE

int gfs_CreateDir(char *path)
{


	return -1;
}


DfGFsUnitfd* gfs_open(const char *path, int flags)
{
	u16 index;
	DfGTreeFsFileName readName;

	index=gfs_ScanFile_BlockIndex(&readName,path,GFS_TYPE_BIN);
	if(index != GFS_INDEX_NULL)
	{
		DfGFsUnitfd *pfd;
		pfd=(DfGFsUnitfd *)malloc(sizeof(DfGFsUnitfd));
		if(pfd == NULL)
		{
			TRACE("gfs Retrievefiles1 malloc is not\r\n");
			//GFS_ERR_NOMEM
			return NULL;
		}
		memcpy_u32((u32*)&pfd->u.h.name,(u32*)&readName,sizeof(readName)/sizeof(u32));
		gfs_FsReadblock(gfs_GetBlockIndex(index),sizeof(readName),((u8*)&pfd->u)+sizeof(readName),sizeof(pfd->u)-sizeof(readName));
		pfd->fOpenFlag = flags;
		pfd->fInvLen= pfd->u.h.InvLen;
		pfd->IndexT = index;
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
			if((flags&GFS_O_TRUNC) && (~ pfd->u.h.InvLen))
			{
				pfd->u.h.InvLen = (u32)(~0);
				pfd->flagNew = 2;
			}
			pfd->fOffset = 0;
			pfd->IndexB = 0;
			pfd->bOffet = sizeof(pfd->u.h);
		}
		return pfd;
	}
	if(flags&GFS_O_CREAT)
	{
		index=gfs_GetNewBlockNum();
		if(index != GFS_INDEX_NULL)
		{
			u16 offBlock = gfs_GetNextNullBlock();
			if(offBlock != GFS_INDEX_NULL)
			{
				DfGFsUnitfd *pfd;
				pfd=(DfGFsUnitfd *)malloc(sizeof(DfGFsUnitfd));
				if(pfd == NULL)
				{
					TRACE("gfs Retrievefiles2 malloc is not\r\n");
					//GFS_ERR_NOMEM
					return NULL;
				}
				gfs_RecordBlockIndex(index,offBlock);	
				memset_u32((u32*)&pfd->u.h,0xFFFFFFFF,sizeof(pfd->u.h)/sizeof(u32));
				strcpy_char(pfd->u.h.name.filepath,(char*)path);
				pfd->u.h.name.fType = GFS_TYPE_BIN;
				pfd->u.h.fdIndex[0] = offBlock;
				pfd->fInvLen= (~0);
				pfd->fOffset = 0;
				pfd->fOpenFlag = flags;
				pfd->IndexT = index;
				pfd->IndexB = 0;
				pfd->bOffet = sizeof(pfd->u.h);
				pfd->flagUpT = 1;
				pfd->flagUpH = 0x40;
				pfd->flagUpE = 0;
				pfd->flagNew = 1;
				return pfd;
			}
			TRACE("gfs Retrievefiles GetNextNullBlock is not\r\n");
			//GFS_ERR_NOSPC
			return NULL;
		}
		TRACE("gfs Retrievefiles BlockIndex is full\r\n");
	}
	return NULL;
}

int gfs_seek(DfGFsUnitfd *pfd,int offset, int whence)
{
	if(pfd==NULL) return -1;
	if((u32)offset > (~ pfd->u.h.InvLen)) return -2;
	return gfs_SetOffset(pfd,offset,whence);
}

int gfs_read(DfGFsUnitfd* pfd,unsigned char *buff,int size)
{
	u32 fLen;
	int offset;
	u16 bOffset,bMax;
	if(pfd==NULL) return GFS_ERR_DIR;
	if(!(pfd->fOpenFlag&GFS_O_RDONLY))
		return GFS_ERR_NORD_PER;
	
	fLen = ~ pfd->u.h.InvLen;
	
	if(size > (int)(fLen-pfd->fOffset))
		size = (int)(fLen-pfd->fOffset);
	if(size <= 0) return 0;

	bMax = GFS_BLOCK_SIZE - pfd->bOffet;
	bOffset = pfd->bOffet;
	offset = 0;
	while(offset < size)
	{
		//----------------------check IndexB and fdIndex[pfd->IndexB]-----------------------
		if(pfd->IndexB >= (sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])))
		{
			TRACE("gfs read IndexB[%d] ERR\r\n",pfd->IndexB);
			return -2;
		}
		if(pfd->u.h.fdIndex[pfd->IndexB] == GFS_INDEX_NULL)
		{
			TRACE("gfs read fdIndex ERR\r\n",pfd->IndexB);
			return -4;
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
				memcpy_u8(buff+offset,((u8*)&pfd->u)+bOffset,bMax);
			}
			else
			{
				gfs_FsReadblock(pfd->u.h.fdIndex[pfd->IndexB],bOffset,buff+offset,bMax);
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

int gfs_write(DfGFsUnitfd* pfd,unsigned char *buff,int writeLen)
{
	u32 fLen;
	int offset;
	u16 bOffset,bMax;
	//-----处理中间插入数据-----
	int saveLen,sOffset;
	u8* pBlockBuff;
	u8 *pSave;

	if(pfd==NULL) return GFS_ERR_DIR;
	if(writeLen <= 0) return GFS_ERR_LEN;
	if(buff==NULL) return GFS_ERR_LEN;
	if(!(pfd->fOpenFlag&GFS_O_WRONLY))
		return GFS_ERR_NOWR_PER;
	
	fLen = ~ pfd->u.h.InvLen;

	bMax = GFS_BLOCK_SIZE - pfd->bOffet;
	bOffset = pfd->bOffet;
	offset = 0;
	pBlockBuff=NULL;
	while(offset < writeLen)
	{
		//----------------------check IndexB and fdIndex[pfd->IndexB]-----------------------
		if(pfd->IndexB >= (sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0])))
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
			memcpy_u8((u8*)(&pfd->u) + bOffset,buff,bMax);
			pfd->flagUpH |= 1;
		}
		else 
		{
			if(pfd->u.h.fdIndex[pfd->IndexB] != GFS_INDEX_NULL)
			{
				if(bMax < GFS_BLOCK_SIZE)
				{
					if((pfd->fOffset + offset) < fLen)
					{//--不允许在中间原来的块上写入内容---
						pBlockBuff=(u8*)malloc(GFS_BLOCK_SIZE);
						gfs_FsReadblock(pfd->u.h.fdIndex[pfd->IndexB],0,pBlockBuff,GFS_BLOCK_SIZE);
						for(saveLen=0;saveLen<bMax;saveLen++)
						{//-------对原来空间进行检查--如果可以写入直接写入---------
							if(buff[offset+saveLen] != (pBlockBuff[bOffset+saveLen]&buff[offset+saveLen]))
								break;
						}
						if(saveLen < bMax)
						{//-------原来空间不满足写入条件--重新换块--------
							memcpy_u8(pBlockBuff+bOffset,buff+offset,bMax);
							if((fLen-(pfd->fOffset + offset)) >  bMax)
							{
								saveLen =bOffset+(fLen-(pfd->fOffset + offset));
								if(saveLen > GFS_BLOCK_SIZE) saveLen=GFS_BLOCK_SIZE;
							}
							else
							{
								saveLen = bOffset+bMax;
							}
							gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[pfd->IndexB]);
							pfd->flagUpT = 1;
							pfd->flagUpE = 1;
							pfd->u.h.fdIndex[pfd->IndexB] = GFS_INDEX_NULL;
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
					gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[pfd->IndexB]);
					pfd->flagUpT = 1;
					pfd->flagUpE = 1;
					pfd->u.h.fdIndex[pfd->IndexB] = GFS_INDEX_NULL;
					pfd->flagUpH |= 0x80;
				}
			}
			
			if(pfd->u.h.fdIndex[pfd->IndexB] == GFS_INDEX_NULL)
			{
				pfd->u.h.fdIndex[pfd->IndexB] = gfs_GetNextNullBlock();
				if(pfd->u.h.fdIndex[pfd->IndexB] == GFS_INDEX_NULL)
				{
					TRACE("gfs write GetNextNullBlock[%d] ERR\r\n",pfd->IndexB);
					if(pBlockBuff) free(pBlockBuff);
					break;
				}
				pfd->flagUpH |= 0x40;
				pfd->flagUpT = 1;
			}
			
			if(pBlockBuff)
			{
				pSave = pBlockBuff;
				sOffset =0;
			}
			else
			{
				pSave = buff+offset;
				saveLen = bMax;
				sOffset =bOffset;
			}
			if(saveLen != gfs_FsWriteblock(pfd->u.h.fdIndex[pfd->IndexB],sOffset,pSave,saveLen))
			{//-----------写失败后处理------------------
			//	TRACE("gfs write bMax[%d] ->fdIndex[%d]=[%X] ERR\r\n",bMax,pfd->IndexB,pfd->u.h.fdIndex[pfd->IndexB]);
				gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[pfd->IndexB]);
				pfd->flagUpT = 1;
				pfd->flagUpE = 1;
				pfd->flagUpH |= 0x80;
				if(bMax < GFS_BLOCK_SIZE && pBlockBuff==NULL)
				{//---------------------后补写失败------------------------------
					if(bOffset>0)
					{
						pBlockBuff=(u8*)malloc(GFS_BLOCK_SIZE);
						if(bOffset)
							gfs_FsReadblock(pfd->u.h.fdIndex[pfd->IndexB],0,pBlockBuff,bOffset);
						memcpy_u8(pBlockBuff+bOffset,buff+offset,bMax);
						saveLen = bOffset+bMax;
					}
				}
				pfd->u.h.fdIndex[pfd->IndexB] = GFS_INDEX_NULL;
				continue;
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
	if(pfd->fOffset > fLen)
	{
		pfd->u.h.InvLen = ~ pfd->fOffset;
		pfd->flagUpH |= 0x80;
	}
    return offset;
}

int gfs_close(DfGFsUnitfd* pfd)
{
	u32 fLen;
	int bMax,ret=0;
	if(pfd==NULL) return -1;
	
	fLen = ~ pfd->u.h.InvLen;
	//-------GFS_O_TRUNC---------------------------
	if((pfd->fOffset > fLen)||((pfd->flagNew == 2) && (fLen != pfd->fOffset)))
	{
		fLen = pfd->fOffset;
		pfd->u.h.InvLen = ~ fLen;
		pfd->flagUpH |= 1;
	}
	
	if(pfd->flagUpH)
	{
		if(fLen >= sizeof(pfd->u.fData))
			bMax = sizeof(pfd->u);
		else 
			bMax = sizeof(pfd->u.h) + fLen;
		
		while(1)
		{
		/*
			if(pfd->u.h.InvLen != (pfd->fInvLen & pfd->u.h.InvLen))
			{//----------------主动更新入口--------------------------
				if(pfd->u.h.fdIndex[0] != GFS_INDEX_NULL)
					gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[0]);
				pfd->u.h.fdIndex[0] = gfs_GetNextNullBlock();
				pfd->flagUpH |= 0x40;
				pfd->flagUpT |= 1;
				pfd->flagUpE = 1;
				if(gfs_RecordBlockIndex(pfd->IndexT,pfd->u.h.fdIndex[0]))	//更新入口地址
				{
					pfd->flagUpT |= 0x80;
				}
			}
			*/
			if(pfd->flagNew != 1)
			{
				if(pfd->u.h.fdIndex[0] != GFS_INDEX_NULL)
				{
					gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[0]);
					pfd->u.h.fdIndex[0]=GFS_INDEX_NULL;
					pfd->flagUpE = 1;
				}
			}
			if(pfd->u.h.fdIndex[0] == GFS_INDEX_NULL)
			{
				pfd->u.h.fdIndex[0] = gfs_GetNextNullBlock();
				pfd->flagUpH |= 0x40;
				if(gfs_RecordBlockIndex(pfd->IndexT,pfd->u.h.fdIndex[0]))	//更新入口地址
					pfd->flagUpT |= 0x80;
//				TRACE("gfs close GetNextNullBlock[%d] Warning\r\n",pfd->u.h.fdIndex[0]);
				if(pfd->u.h.fdIndex[0] == GFS_INDEX_NULL)
				{//---文件头存无法存储--文件所有内容必需清空----
					gfs_RecordDelete(&pfd->u.h.fdIndex[1],(sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0]))-1);
					ret = GFS_ERR_NOSPC;
					break;
				}
			}
			if(gfs_FsWriteblock(pfd->u.h.fdIndex[0],0,&pfd->u,bMax) == bMax) break;
			//-----------写失败后处理------------------
//			TRACE("gfs close Writeblock->fdIndex[%d] ERR\r\n",pfd->u.h.fdIndex[0]);
			gfs_RecordBlockEraseBit(pfd->u.h.fdIndex[0]);
			pfd->u.h.fdIndex[0]=GFS_INDEX_NULL;
			pfd->flagUpE = 1;
		}
		if(pfd->flagUpH & 0x40)
		{//-----执行过 gfs_GetNextNullBlock 需要重新检查空间----
			gfs_UndoRecordDelete(pfd->u.h.fdIndex,sizeof(pfd->u.h.fdIndex)/sizeof(pfd->u.h.fdIndex[0]));		
		}
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
		gfs_CheckEraseBlockBit(0);
	}
	free(pfd);
    return ret;
}

int gfs_GetSize(DfGFsUnitfd* pfd)
{
	if(pfd==NULL) return -1;
	return ~ pfd->u.h.InvLen;
}



int gfs_remove(const char *path)
{
	DfGFsUnitfd* fd;
	fd=gfs_open(path,GFS_O_RDWR);
	if(fd == NULL) return 1;
	gfs_RecordDelete(fd->u.h.fdIndex,sizeof(fd->u.h.fdIndex)/sizeof(fd->u.h.fdIndex[0]));
	gfs_RecordBlockIndex(fd->IndexT,GFS_INDEX_NULL);	//清除入口地址
	gfs_BalancedGlobalSave(1);
	gfs_CheckEraseBlockBit(0);
	free(fd);
	return 0;	
}


#endif

