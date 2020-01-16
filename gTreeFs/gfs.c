/*
 * The gTree filesystem
 *
 * Copyright (c) 2019 DengGuozu
 *
 * Licensed under the Apache License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "communal.h"


#ifdef GTREE_FILE_COURCE


#define GFS_PATH_TAG				('/')
//========文件路径检查===返回 1,文件路径，-1非文件夹，字符不匹配，0 匹配文件------
int gfs_strcmp_path(u8* pPath,u8* strTag,u8 pathTag)
{
	if(strTag==NULL)
	{
		while(*pPath != '\0' && *pPath != pathTag) pPath++;
		if(*pPath == pathTag)
			return 1;
		return 0;
	}
	
	while(*strTag)
	{
		if(*pPath != *strTag)
			return -1;
		pPath++;
		strTag++;
	}
	if(*pPath == pathTag)
		return 1;
	if(*pPath)	//后面还有文件名
		return -1;
	return 0;
}

void memcpy_u32(u32* pU1,u32* pU2,int len)
{
	while(len--)
		*pU1++ = *pU2++;
}

void memcpy_u8(u8* pU1,u8* pU2,int len)
{
	while(len--)
		*pU1++ = *pU2++;
}

void memset_u32(u32* pU1,u32 tag,int len)
{
	while(len--)
		*pU1++ = tag;
}

void memset_u8(u8* pU1,u8 tag,int len)
{
	while(len--)
		*pU1++ = tag;
}

void strcpy_char(char* pS1,char* pS2)
{
	while(*pS2)
	{
		*pS1++ = *pS2++;
	}
	*pS1 = *pS2;
}

int memcmp_u32(u32* pU1,u32* pU2,int len)
{
	while(len--)
	{
		if (*pU1 != *pU2)
			return (*pU1 - *pU2);
		pU1++; pU2++;
	}
	return 0;
}

//================================================================================
static DfGTreeFsGlobal gGlobalOldeData;
static DfGTreeFsBasi gBasiGlobal={0};
u32 gfs_erase_times=0;

int gfs_BalancedGlobalInit(DfGTreeFsGlobal *pGlobal)
{
	u32 Addr;
	DfGTreeFsBlockHead t;
	u16 i,max,eMax,flag;
	if(gBasiGlobal.gfsCoutSize == 0) return -1;
	max = gBasiGlobal.gfsCoutSize*(GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal));
	eMax = 0; flag=0;
	for(i=0;i<max;i++)
	{
		Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*i);
		if(sizeof(t) == SPI_Flash_Read(Addr,&t,sizeof(t)))
		{
			if(t.head == GFS_BASIC_GUID)
			{
				flag=1;
			}
			else 
			{
				if(t.head != GFS_BASIC_NULL)
					eMax++;
				if(flag) break;
			}
		}
	}
	if(flag)
	{
		gBasiGlobal.gfsCoutIndex=i-1;
		Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
		if(sizeof(gGlobalOldeData) == SPI_Flash_Read(Addr,&gGlobalOldeData,sizeof(gGlobalOldeData)))
		{
			memcpy_u32((u32*)pGlobal,(u32*)&gGlobalOldeData,sizeof(DfGTreeFsGlobal)/sizeof(u32));
			return sizeof(gGlobalOldeData);
		}
	}
	//----------------引导块需要初始化-----------------------
	if(eMax >= (GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal)))
	{
		max=eMax/(GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal));
		for(i=0;i<max;i++) 
			SPI_Flash_OneSector(gBasiGlobal.gfsCoutStart+i);
		gfs_erase_times++;
	}
	memset_u32((u32*)&gGlobalOldeData,0xffffffff,sizeof(gGlobalOldeData)/sizeof(u32));
	gGlobalOldeData.t.head = GFS_BASIC_GUID;
	gGlobalOldeData.t.index = 0;
	memcpy_u32((u32*)pGlobal,(u32*)&gGlobalOldeData,sizeof(DfGTreeFsGlobal)/sizeof(u32));
	gBasiGlobal.gfsCoutIndex = 0;
	TRACE("-------BalancedGlobalInit*NULL------------\r\n");
	return 0;
}


int gfs_BalancedGlobalSave(int pNew)
{
	u32 Addr;
	int ret=-1;
	u16 i,max,Esblock[16];
	u16 maxIndex,blockNum;
	blockNum = GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal);
	maxIndex = gBasiGlobal.gfsCoutSize*blockNum;
	
	i=maxIndex;
	max=0;
	if(gBasiGlobal.g.flagClear != 0xFF)
	{
		pNew=1;
		gBasiGlobal.g.flagClear = 0xFF;
	}
	while(i--)
	{
		if(pNew) 
		{
			gBasiGlobal.gfsCoutIndex++;
			if(gBasiGlobal.gfsCoutIndex >= maxIndex)
			{
				gBasiGlobal.gfsCoutIndex=0;
				if(max<(sizeof(Esblock)/sizeof(Esblock[0])))
					Esblock[max++] = gBasiGlobal.gfsCoutSize;
			}
			else if((gBasiGlobal.gfsCoutIndex%blockNum)==0)
			{
				if(max<(sizeof(Esblock)/sizeof(Esblock[0])))
					Esblock[max++] = gBasiGlobal.gfsCoutIndex/blockNum;
			}
		}
		
		//TRACE("---BalancedGlobalSave gfsCoutIndex[%d]--\r\n",gBasiGlobal.gfsCoutIndex);
		Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
		if(sizeof(DfGTreeFsGlobal) != SPI_Flash_Write(Addr,&gBasiGlobal.g,sizeof(DfGTreeFsGlobal)))
			continue;
		if(sizeof(gGlobalOldeData) != SPI_Flash_Read(Addr,&gGlobalOldeData,sizeof(gGlobalOldeData)))
			continue;
		if(memcmp_u32((u32*)&gBasiGlobal.g ,(u32*)&gGlobalOldeData,sizeof(DfGTreeFsGlobal)/sizeof(u32)) != 0)
		{
			if(pNew == 0)
			{
				TRACE("BalancedGlobalSave->pNew[%d]",pNew);
			}
			pNew=1;	
			continue;
		}
		ret = sizeof(DfGTreeFsGlobal);
		break;
	}
	//----------------检查是否有察除块------------------------
	for(i=0;i<max;i++)
	{
		SPI_Flash_OneSector(Esblock[i]-1);
		gfs_erase_times++;
	}
	//--------------------------------------------------------
	if(ret < 0)
	{
		TRACE("******BalancedGlobalSave*Err*[max=%d]****\r\n",max);
	}
	return ret;
}



void gfs_BalancedEraseOk(void)
{
	u32 Addr;
	gBasiGlobal.g.flagClear=0x55;
	//TRACE("---BalancedGlobalSave gfsCoutIndex[%d]--\r\n",gBasiGlobal.gfsCoutIndex);
	Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
	SPI_Flash_Write(Addr,&gBasiGlobal.g,sizeof(DfGTreeFsGlobal)-sizeof(gBasiGlobal.g.NullBit)-sizeof(gBasiGlobal.g.EraseBit));
}


int gfs_FsReadblock(u16 block,u16 offset,void* buff,u16 size)
{
	u32 Addr=(gBasiGlobal.gfsBlockStart+block)*GFS_BLOCK_SIZE + offset;
	return SPI_Flash_Read(Addr,buff,size);
}

int gfs_FsWriteblock(u16 block,u16 offset,void* buff,u16 size)
{
	u32 Addr=(gBasiGlobal.gfsBlockStart+block)*GFS_BLOCK_SIZE + offset;
	if(size == SPI_Flash_Write(Addr,buff,size))
	{//---------校验写入是否正确-----------------
		u8 CheckBuff[GFS_BLOCK_SIZE],*pTag=buff;
		u16 i;
		SPI_Flash_Read(Addr,CheckBuff,size);
		for(i=0;i<size;i++)
		{
			if(pTag[i] != CheckBuff[i])
				return 0;
		}
		return size;
	}
	return -1;
}

int gfs_FsEraseblock(u16 block)
{
//	TRACE("----gfs Eraseblock block[%d]----",block);
	return SPI_Flash_OneSector(gBasiGlobal.gfsBlockStart+block);
}
//================================================================================



int gfs_RecordBlockIndex(u16 i,u16 par)
{
	u16 oldIndex,newIndex;
	if(i >= (sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0])))
	{
		TRACE("gfs RecordBlockIndex i[%d] tobig Err\r\n",i);
		return -1;
	}
	oldIndex = gBasiGlobal.g.BlockIndex[i];
	newIndex = ~(par+1);	//加一个固定值避免 ~0 == 0xFFFF
	gBasiGlobal.g.BlockIndex[i] = newIndex;
	if(newIndex == (newIndex&oldIndex))
	{
//		TRACE("gfs RecordBlockIndex i[%d]par[%d] newIndex OK\r\n",i,par);
		return 0;
	}
	return 1;
}

u16 gfs_GetBlockIndex(u16 i)
{//加一个固定值避免 ~0 == 0xFFFF
	return ((~gBasiGlobal.g.BlockIndex[i])-1);
}

u16 gfs_GetNewBlockNum(void)
{
	u16 blockMax,i;
	if(gBasiGlobal.gfsBlockSize == 0)	
		return GFS_INDEX_NULL;
	blockMax = sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gBasiGlobal.g.BlockIndex[i] == GFS_INDEX_NULL)
		{
			return i;
		}
	}
	TRACE("gfs GetNewBlockNum i[%d] Err OK\r\n",i);
	return GFS_INDEX_NULL;
}

u16 gfs_ScanFile_BlockIndex(DfGTreeFsFileName *pName,const char *path,char pathType)
{
	u16 i,blockMax;
	if(gBasiGlobal.gfsBlockSize == 0)	
		return GFS_INDEX_NULL;
	blockMax = sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gBasiGlobal.g.BlockIndex[i] != GFS_INDEX_NULL)
		{
			gfs_FsReadblock(gfs_GetBlockIndex(i),0,pName,sizeof(DfGTreeFsFileName));
			if(pName->fType != pathType) 
				continue;	//文件夹内容不能匹配

			if(gfs_strcmp_path((u8*)path,(u8*)pName->filepath,'\0') >= 0)
			{
				return i;
			}
		}
	}
	return GFS_INDEX_NULL;
}

void gfs_RecordDelete(u16* pIndexArray,int max)
{
	int i;
	u16 ei,b;
	for(i=0;i<max;i++)
	{//-----一查到底部，避免重新写入长度的文件后面空间没有被清空------
		if(pIndexArray[i] == GFS_INDEX_NULL)	//遇到一个 GFS_INDEX_NULL 就跳出来
			break;

		ei = pIndexArray[i]/(sizeof(gBasiGlobal.g.EraseBit[0])*8);
		if(ei >= (sizeof(gBasiGlobal.g.EraseBit)/sizeof(gBasiGlobal.g.EraseBit[0])))
		{
			TRACE("gfs RecordDelete EraseBlock[%d] tobig Err\r\n",pIndexArray[i]);
			return;
		}
		b = pIndexArray[i]%(sizeof(gBasiGlobal.g.EraseBit[0])*8);
		gBasiGlobal.g.EraseBit[ei] &= ~(0x01<<b);
		pIndexArray[i] = GFS_INDEX_NULL;
	}
}

void gfs_UndoRecordDelete(u16* pIndexArray,int max)
{
	u32 bitflag;
	int i;
	u16 ei,b;
	for(i=0;i<max;i++)
	{
		if(pIndexArray[i] == GFS_INDEX_NULL)	//遇到一个 GFS_INDEX_NULL 就跳出来
			break;
		ei = pIndexArray[i]/(sizeof(gBasiGlobal.g.EraseBit[0])*8);
		if(ei >= (sizeof(gBasiGlobal.g.EraseBit)/sizeof(gBasiGlobal.g.EraseBit[0])))
		{
			TRACE("gfs UndoRecordDelete EraseBlock[%d] tobig Err\r\n",pIndexArray[i]);
			return;
		}
		
		b = pIndexArray[i]%(sizeof(gBasiGlobal.g.EraseBit[0])*8);
		bitflag = (0x01 << (b));
		if(!(bitflag&gBasiGlobal.g.EraseBit[ei]))
		{
			gBasiGlobal.g.EraseBit[ei] |= bitflag;
		}
	}
}


int gfs_RecordBlockEraseBit(u16 EraseBlock)
{
	u16 i,b;
	if(EraseBlock == GFS_INDEX_NULL) return 1;
	i = EraseBlock/(sizeof(gBasiGlobal.g.EraseBit[0])*8);
	if(i >= (sizeof(gBasiGlobal.g.EraseBit)/sizeof(gBasiGlobal.g.EraseBit[0])))
	{
		TRACE("gfs RecordEraseBlockBit EraseBlock[%d] tobig Err\r\n",EraseBlock);
		return -1;
	}
	b = EraseBlock%(sizeof(gBasiGlobal.g.EraseBit[0])*8);
	gBasiGlobal.g.EraseBit[i] &= ~(0x01<<b);
	return 0;
}

int gfs_CheckEraseBlockBit(int initFalg)
{
	u16 i,iMax,b,bMax;
	u32 bitflag, UpFlag = 0;
	
	iMax=sizeof(gBasiGlobal.g.EraseBit)/sizeof(gBasiGlobal.g.EraseBit[0]);
	bMax=sizeof(gBasiGlobal.g.EraseBit[0])*8;
	for(i=0;i<iMax;i++)
	{
		if(~gBasiGlobal.g.EraseBit[i])	//No 0xffffffff
		{
			bitflag=0x01;
			for(b=0;b<bMax;b++)
			{
				if(!(gBasiGlobal.g.EraseBit[i] & bitflag))
				{//-----察除前面被标记的块-------
					if(initFalg==0 || gBasiGlobal.g.flagClear == 0xFF)
					{//---------非 FF 已经察除不需要再重复动作---
						gfs_FsEraseblock(i*bMax + b);
						UpFlag++;
					}
					gBasiGlobal.g.EraseBit[i] |= bitflag;
					gBasiGlobal.g.NullBit[i] |= bitflag;
				}
				bitflag <<= 1;			
			}
		}
	}
	if(UpFlag)
	{
		gfs_BalancedEraseOk();
	}
//	TRACE("gfs CheckEraseBit ERASE_Times[%d]\r\n",UpFlag);
	return UpFlag;
}



u16 gfs_GetNextNullBlock(void)
{
	u32 bitflag;
	u16 Ix,IxMax,bMax,times;
	
	IxMax=sizeof(gBasiGlobal.g.NullBit)/sizeof(gBasiGlobal.g.NullBit[0]);
	bMax=sizeof(gBasiGlobal.g.NullBit[0])*8;
	times = gBasiGlobal.gfsBlockSize;
	while(times--)
	{
		if(gBasiGlobal.gfsNewIndex >= gBasiGlobal.gfsBlockSize)
		{//----避免超出使用空间------
			gBasiGlobal.gfsNewIndex=0;
		}
		Ix = gBasiGlobal.gfsNewIndex / bMax;
		bitflag = (0x01<<(gBasiGlobal.gfsNewIndex % bMax));
		if(Ix < IxMax && (gBasiGlobal.g.NullBit[Ix] & bitflag))
		{
			gBasiGlobal.g.NullBit[Ix] &= ~bitflag;
			gBasiGlobal.g.EraseBit[Ix] &= ~bitflag;	//用于异常处理，解决文件没有close而引发碎片
			return gBasiGlobal.gfsNewIndex++;
		}
		gBasiGlobal.gfsNewIndex++;
	}
	TRACE("*****GetNextNullBlock ERR*[%d]**Is No BitSpace*****\r\n",gBasiGlobal.gfsNewIndex);
	TRACE_HEX("NullBit",(u8*)gBasiGlobal.g.NullBit,IxMax*sizeof(gBasiGlobal.g.NullBit[0]));
	return GFS_INDEX_NULL;
}





int gfs_Init(u16 startCap,u16 fdSize,u16 countSize)
{
	if(fdSize <= countSize) return -1;
	gBasiGlobal.gfsCoutStart = startCap;
	gBasiGlobal.gfsCoutSize = countSize;
	gBasiGlobal.gfsBlockStart = startCap+countSize;
	gBasiGlobal.gfsBlockSize = fdSize-countSize;
	if(gfs_BalancedGlobalInit(&gBasiGlobal.g))
	{
		gfs_CheckEraseBlockBit(1);
	}
	gBasiGlobal.gfsNewIndex=gBasiGlobal.g.t.index;
	return 0;
}

int gfs_Fomat(char *pKey)
{
	u16 i,max;
	max = gBasiGlobal.gfsCoutSize+gBasiGlobal.gfsBlockSize;
	for(i=0;i<max;i++)
	{
		TRACE("ERASE[%x]...",gBasiGlobal.gfsCoutStart+i);
		SPI_Flash_OneSector(gBasiGlobal.gfsCoutStart+i);
	}
	gfs_BalancedGlobalInit(&gBasiGlobal.g);
	gBasiGlobal.gfsNewIndex=gBasiGlobal.g.t.index;
	return 0;
}


int gfs_SetOffset(DfGFsUnitfd *pfd,int offset, int whence)
{
	if(whence == GFS_SEEK_CUR)
	{
		if((offset + pfd->fOffset) > (~ pfd->u.h.InvLen)) return -2;
		pfd->fOffset += offset;
	}
	else 
	{
		if(whence == GFS_SEEK_END)
		{
			offset = (~ pfd->u.h.InvLen) - offset;
		}
		pfd->IndexB = 0;
		pfd->bOffet = sizeof(pfd->u.h);
		pfd->fOffset = offset;
	}
	
	//-----------------------------------------
	{
		int IndexMax =GFS_BLOCK_SIZE - pfd->bOffet;
		while(offset >= IndexMax)
		{
			offset -= IndexMax;
			pfd->IndexB ++;
			pfd->bOffet =0;
			IndexMax = GFS_BLOCK_SIZE;
		}
	}
	pfd->bOffet += offset;
	return 0;
}




/*
DfGFsUnitfd* gfs_RetrieveFile(const char *path, int flags)
{
	u16 index;
	DfGTreeFsFileName readName;
	if(gBasiGlobal.gfsBlockSize == 0)
		return NULL;

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
		memcpy(&pfd->u.h.name,&readName,sizeof(readName));
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
		}
		else if(flags&GFS_O_TRUNC)
		{
			if(~ pfd->u.h.InvLen)
			{
				pfd->u.h.InvLen = (u32)(~0);
				pfd->flagNew = 1;
			}
			pfd->fOffset = 0;
			pfd->IndexB = 0;
			pfd->bOffet = sizeof(pfd->u.h);
		}
		else
		{
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
				memset(&pfd->u.h,0xFF,sizeof(pfd->u.h));
				strcpy(pfd->u.h.name.filepath,path);
				pfd->u.h.name.fType = GFS_TYPE_BIN;
				pfd->u.h.fdIndex[0] = offBlock;
				pfd->fInvLen= (~0);
				pfd->fOffset = 0;
				pfd->fOpenFlag = flags;
				pfd->IndexT = index;
				pfd->IndexB = 0;
				pfd->bOffet = sizeof(pfd->u.h);
				pfd->flagUpT = 1;
				pfd->flagUpH = 1;
				pfd->flagUpE = 0;
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

DfGFsUnitfd* gfs_RetrievePath(const char *path, int flags)
{
	u16 i,blockMax;
	int ret=0;
	DfGTreeFsFileName readName;
	if(gBasiGlobal.gfsBlockSize == 0)
		return NULL;
	
	blockMax = sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gfs_GetBlockIndex(i) != GFS_INDEX_NULL)
		{
			gfs_FsReadblock(gBasiGlobal.g.BlockIndex[i],0,&readName,sizeof(readName));
			if(readName.fType != GFS_TYPE_FLD) 
				continue;	//文件夹内容不能匹配
			ret = gfs_strcmp_path((u8*)path, (u8*)readName.filepath,'/');
			if(0 <= ret)
			{
				DfGFsUnitfd *pfd;
				pfd=(DfGFsUnitfd *)malloc(sizeof(DfGFsUnitfd));
				if(pfd == NULL)
				{
					TRACE("gfs Retrievefiles1 malloc is not\r\n");
					return NULL;
				}
				memcpy(&pfd->u.h.name,&readName,sizeof(readName));
				gfs_FsReadblock(gfs_GetBlockIndex(i),sizeof(readName),((u8*)&pfd->u.h)+sizeof(readName),sizeof(pfd->u.h)-sizeof(readName));
				pfd->IndexT = i;
				pfd->flagUpT = 0;
				pfd->flagUpH = 0;
				pfd->flagUpE = 0;
				pfd->flagNew = 0;
				pfd->fOffset = 0;
				pfd->IndexB = 0;
				pfd->bOffet = sizeof(pfd->u.h);
				return pfd;
			}
		}
	}
	if(flags&GFS_O_CREAT)
	{
		u16 offBlock=0;
		i=gfs_GetNewBlockNum();
		if(i != NULL)
		{
			offBlock = gfs_GetNextNullBlock();
			if(offBlock != GFS_INDEX_NULL)
			{
				DfGFsUnitfd *pfd;
				pfd=(DfGFsUnitfd *)malloc(sizeof(DfGFsUnitfd));
				if(pfd == NULL)
				{
					TRACE("gfs Retrievefiles2 malloc is not\r\n");
					return NULL;
				}
				gfs_RecordBlockIndex(i,offBlock);	
				memset(&pfd->u.h,0xFF,sizeof(pfd->u.h));
				strcpy(pfd->u.h.name.filepath,path);
				pfd->u.h.name.fType = GFS_TYPE_FLD;
				pfd->u.h.fdIndex[0] = offBlock;
				pfd->flagUpT = 1;
				pfd->flagUpH = 1;
				pfd->flagUpE = 0;
				pfd->flagNew = 0;
				pfd->fOffset = 0;
				pfd->IndexB = 0;
				pfd->bOffet = sizeof(pfd->u.h);
				return pfd;
			}
			TRACE("gfs RetrievePath GetNextNullBlock is not\r\n");
		}
		TRACE("gfs RetrievePath BlockIndex is full\r\n");
	}
	return NULL;
}

*/
#endif

