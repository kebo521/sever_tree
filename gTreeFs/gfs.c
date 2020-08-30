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

#include "gfs_api.h"


#ifdef GTREE_FILE_COURCE


void memcpy_u32(u32* pU1,u32* pU2,int len)
{
	while(len--)
		*pU1++ = *pU2++;
}

void memset_u32(u32* pU1,u32 tag,int len)
{
	while(len--)
		*pU1++ = tag;
}

int memcmp_u32(u32* pU1,u32* pU2,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		if (*pU1 != *pU2)
			return (i+1);
		pU1++; pU2++;
	}
	return 0;
}

//================================================================================
static DfGTreeFsBasi gBasiGlobal={0};
int gfs_BalancedGlobalInit(void)
{
	u32 Addr;
	DfGTreeFsGlobal gGlobalOldeData;
	u16 buffCoutIndex=gBasiGlobal.gfsCoutIndex;
	if(gBasiGlobal.gfsCoutSize == 0) return -1;
	gBasiGlobal.gfsCoutIndex = gBasiGlobal.gfsCoutSize*(GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal));
	while(gBasiGlobal.gfsCoutIndex--)
	{
		Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
		if(sizeof(gGlobalOldeData.t) == SPI_Flash_Read(Addr,&gGlobalOldeData.t,sizeof(gGlobalOldeData.t)))
		{
			if(gGlobalOldeData.t.head == GFS_BASIC_GUID && gGlobalOldeData.t.flagG == 'G')
			{
				if(sizeof(gGlobalOldeData) == SPI_Flash_Read(Addr,&gGlobalOldeData,sizeof(gGlobalOldeData)))
				{
					if(buffCoutIndex != gBasiGlobal.gfsCoutIndex)
					{
						TRACE("##buffCoutIndex[%d->%d]gfsCoutSize=%d", buffCoutIndex, gBasiGlobal.gfsCoutIndex, gBasiGlobal.gfsCoutSize);
					}
					
					{
						u32		NewData[32];	//空块 block
						u32		OldData[32];	//需要察除的块
						int		i;
						for(i=0;i<32;i++)
							NewData[i] =gBasiGlobal.g.NullBit[i]^gBasiGlobal.g.EraseBit[i];
						for(i=0;i<32;i++)
							OldData[i] =gGlobalOldeData.NullBit[i]^gGlobalOldeData.EraseBit[i];
						
						if(memcmp_u32(OldData,NewData,sizeof(NewData)/sizeof(u32)))
						{
							TRACE_HEX("  gBasiGlobal.g",(u8*)&gBasiGlobal.g, sizeof(gBasiGlobal.g));
							TRACE_HEX("gGlobalOldeData",(u8*)&gGlobalOldeData, sizeof(gGlobalOldeData));
						}
					}
					//TRACE("Global 3Read[%x]->[%d] [%X][%d] OK",Addr,gBasiGlobal.gfsCoutIndex,gGlobalOldeData.t.head,gGlobalOldeData.t.flagG);
					memcpy_u32((u32*)&gBasiGlobal.g,(u32*)&gGlobalOldeData,sizeof(DfGTreeFsGlobal)/sizeof(u32));
					return sizeof(gBasiGlobal.g);
				}
			}
		}
	}
	//----------------引导块需要初始化-----------------------
	memset_u32((u32*)&gBasiGlobal.g,0xffffffff,sizeof(gBasiGlobal.g)/sizeof(u32));
	gBasiGlobal.g.t.head = GFS_BASIC_GUID;
	gBasiGlobal.g.t.index = 0;
	gBasiGlobal.g.t.flagG	='G';
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
	DfGTreeFsGlobal gGlobalOldeData;
	blockNum = GFS_BLOCK_SIZE/sizeof(DfGTreeFsGlobal);
	maxIndex = gBasiGlobal.gfsCoutSize*blockNum;
	i=maxIndex;
	max=0;
	if(gBasiGlobal.g.t.flagClear != 0xFF)
	{
		pNew=1;
		gBasiGlobal.g.t.flagClear = 0xFF;
	}
	while(i--)
	{
		if(pNew) 
		{
			gBasiGlobal.gfsCoutIndex++;
			if(gBasiGlobal.gfsCoutIndex >= maxIndex)
			{
				gBasiGlobal.gfsCoutIndex=0;
				if(max < (sizeof(Esblock)/sizeof(Esblock[0])))
					Esblock[max++] = gBasiGlobal.gfsCoutSize-1;	//End
			}
			else if((gBasiGlobal.gfsCoutIndex % blockNum) == 0)
			{
				if(max < (sizeof(Esblock)/sizeof(Esblock[0])))
				{
					if(gBasiGlobal.gfsCoutIndex == 0)
						Esblock[max++] = gBasiGlobal.gfsCoutSize-1;	//End
					else
						Esblock[max++] = (gBasiGlobal.gfsCoutIndex/blockNum) -1;	//0~n

				}
					
			}
		}
		//TRACE("---BalancedGlobalSave gfsCoutIndex[%d]--\r\n",gBasiGlobal.gfsCoutIndex);
		Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
		ret = SPI_Flash_Write(Addr,&gBasiGlobal.g,sizeof(gBasiGlobal.g));
		if(sizeof(gBasiGlobal.g) != ret)
		{
			TRACE("DATA Flash Write[%d]->[%d]\r\n",sizeof(gBasiGlobal.g),ret);
			continue;
		}
		//memset_u32((u32*)&gGlobalOldeData,0x00,sizeof(gGlobalOldeData)/sizeof(u32));
		ret = SPI_Flash_Read(Addr,&gGlobalOldeData,sizeof(gGlobalOldeData));
		if(sizeof(gGlobalOldeData) != ret)
		{
			TRACE("DATA Flash Read[%d]->[%d]\r\n",sizeof(gGlobalOldeData),ret);
			continue;
		}
		if(memcmp_u32((u32*)&gBasiGlobal.g ,(u32*)&gGlobalOldeData,sizeof(DfGTreeFsGlobal)/sizeof(u32)))
		{
			if(pNew == 0)
			{
				TRACE("BalancedGlobalSave->pNew[%d]\r\n",pNew);
			}
			pNew=1;	
			TRACE_HEX("SPI Flash_Write",&gBasiGlobal.g, sizeof(DfGTreeFsGlobal));
			TRACE_HEX("SPI Flash_Read",&gGlobalOldeData,sizeof(gGlobalOldeData));
			continue;
		}
		//TRACE("Global Write[%x]->[%d] [%X][%d] OK\r\n",Addr,gBasiGlobal.gfsCoutIndex,gBasiGlobal.g.t.head,gBasiGlobal.g.t.flagG);
		ret = sizeof(DfGTreeFsGlobal);
		break;
	}
	//----------------检查是否有察除块------------------------
	for(i=0;i<max;i++)
	{
		SPI_Flash_EraseOne(SPI_SECTOR_SIZE*(gBasiGlobal.gfsCoutStart+ Esblock[i]));
		//SPI_Flash_OneSector(gBasiGlobal.gfsCoutStart+ Esblock[i]);
		#ifdef GTREE_FILE_ERR_RECORD
		gfs_erase_times++;
		#endif
	}
	//--------------------------------------------------------
	if(ret < 0)
	{
		TRACE("******BalancedGlobalSave*Err*[max=%d]****\r\n",max);
	}
	return ret;
}

void gfs_BalancedGlobalErase(void)
{
	u32 Addr;
	Addr=gBasiGlobal.gfsCoutStart*GFS_BLOCK_SIZE + (sizeof(DfGTreeFsGlobal)*gBasiGlobal.gfsCoutIndex);
	gBasiGlobal.g.t.flagClear=0x55;	//标记 已经察除
	SPI_Flash_Write(Addr,&gBasiGlobal.g.t,sizeof(gBasiGlobal.g.t));
}
//==================================================================================================


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
				return i;
		}
		return size;
	}
	return -1;
}

int gfs_FsEraseblock(u16 block)
{
//	TRACE("----gfs Eraseblock block[%d]----",block);
	return SPI_Flash_EraseOne((gBasiGlobal.gfsBlockStart+block)*SPI_SECTOR_SIZE);
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
	newIndex = ~(++par);	//加一个固定值避免 ~0 == 0xFFFF
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
			gBasiGlobal.g.BlockIndex[i] = GFS_BASIC_DATA;	//避免其它线程抢占入口
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
			if(strcmp(path,pName->filepath)==0)
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
		gBasiGlobal.g.EraseBit[ei] |= (0x01 << (b));
	}
}


int gfs_CheckEraseBlockBit(void)
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
					if(gBasiGlobal.g.t.flagClear == 0xFF)
					{//--非 FF 已经察除不需要再重复动作---
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
	{//---当前位置标记已察除-----
		gfs_BalancedGlobalErase();
	}
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
			//gBasiGlobal.g.EraseBit[Ix] &= ~bitflag;	//用于异常处理，解决文件没有close而引发碎片
			return gBasiGlobal.gfsNewIndex++;
		}
		gBasiGlobal.gfsNewIndex++;
	}
	TRACE("*****GetNextNullBlock ERR*[%d]**Is No BitSpace*****\r\n",gBasiGlobal.gfsNewIndex);
	//TRACE_HEX("NullBit",(u8*)gBasiGlobal.g.NullBit,IxMax*sizeof(gBasiGlobal.g.NullBit[0]));
	return GFS_INDEX_NULL;
}





int gfs_Init(u16 startCap,u16 fdSize,u16 countSize)	//startCap,fdSize,countSize->SPI_BLOCK_SIZE
{
	if(GFS_BLOCK_SIZE % sizeof(DfGTreeFsGlobal)) return -1;
	if(fdSize <= countSize) return -2;
	if(fdSize <= 64) return -3;
	gBasiGlobal.gfsCoutStart = startCap;
	gBasiGlobal.gfsCoutSize = countSize;
	gBasiGlobal.gfsBlockStart = startCap+countSize;
	gBasiGlobal.gfsBlockSize = fdSize-countSize;
	if(sizeof(gBasiGlobal.g) == gfs_BalancedGlobalInit())
	{
		gfs_CheckEraseBlockBit();
		gBasiGlobal.gfsNewIndex=gBasiGlobal.g.t.index;
		return 0;
	}
	else
	{
		int i,max;
		max = fdSize;
		for(i=0;i<max;i++) 
			SPI_Flash_EraseOne((startCap+i)*GFS_BLOCK_SIZE);
		gfs_BalancedGlobalSave(0);
		gBasiGlobal.gfsNewIndex=gBasiGlobal.g.t.index;
		return 1;
	}
}


int gfs_Fomat(char *pKey)
{
	u16 i,max;
	if(gBasiGlobal.gfsBlockSize < 16)
		return -1;
	max = gBasiGlobal.gfsCoutSize+gBasiGlobal.gfsBlockSize;
	for(i=0;i<max;i++)
	{
		TRACE("ERASE[%d]...",gBasiGlobal.gfsCoutStart+i);
		SPI_Flash_EraseOne((gBasiGlobal.gfsCoutStart+i)*GFS_BLOCK_SIZE);
	}
	gfs_BalancedGlobalInit();
	gfs_BalancedGlobalSave(0);
	gBasiGlobal.gfsNewIndex=gBasiGlobal.g.t.index;
	return 0;
}


int gfs_SetOffset(gFILE *pfd,int offset, int whence)
{
	if(whence == GFS_SEEK_CUR)
	{
		if((offset + pfd->fOffset) > pfd->fLen) return -2;
		pfd->fOffset += offset;
	}
	else 
	{
		if(whence == GFS_SEEK_END)
		{
			offset = pfd->fLen - offset;
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

void Gfs_SetNullBit(u16 index,u32 *pBit32)
{
	u32 bitflag,i;
	bitflag = (0x01<<(index % 32));
	i = index / 32;
	pBit32[i] &= ~bitflag;
	pBit32[i] &= ~bitflag;
}



int gfs_checkALL(void)
{
	u32 	NullBit[32];	//空块 block
	DfgFsUnitBlock tfilePar;
	u32 offset,fileLen,AllfileLen=0;
	u32 fileNum=0,blockNUN=0,Lenzero=0;
	u16 i,blockMax,j,max;
	if(gBasiGlobal.gfsBlockSize == 0)	
		return GFS_INDEX_NULL;
	memset_u32(NullBit,0xFFFFFFFF,sizeof(NullBit)/sizeof(u32));
	blockMax = sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0]);
	max = sizeof(tfilePar.h.fdIndex)/sizeof(tfilePar.h.fdIndex[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gBasiGlobal.g.BlockIndex[i] != GFS_INDEX_NULL)
		{
			gfs_FsReadblock(gfs_GetBlockIndex(i),0,&tfilePar,sizeof(DfgFsUnitBlock));
			//tfileHead.name.fType
			fileLen = ~(tfilePar.h.InvLen);
			fileNum ++;
			blockNUN ++;
			offset = sizeof(tfilePar.fData);
			Gfs_SetNullBit(gfs_GetBlockIndex(i),NullBit);
			for(j=0;j<max;j++)	//while(j++)
			{
				if(tfilePar.h.fdIndex[j] == GFS_INDEX_NULL) break;
				Gfs_SetNullBit(tfilePar.h.fdIndex[j],NullBit);
				//gfs_FsReadblock(Index,0,&tfileHead,sizeof(DfgFsUnitHead));
				offset += sizeof(DfgFsUnitBlock);
				blockNUN ++;
			}
			tfilePar.h.name.filepath[sizeof(tfilePar.h.name.filepath)-1]='\0';
			if(fileLen == 0) 
			{
				TRACE("*1 Index[%d] Block[%d] is Zero name0[%x]\r\n",i,gBasiGlobal.g.BlockIndex[i],tfilePar.h.name.filepath[0]);
				Lenzero++;
			}
			else if(fileLen <=	sizeof(tfilePar.fData) && offset > sizeof(tfilePar.fData))
			{
				TRACE("#1[%d] name[%s] len[%d,%d]-[%d]\r\n",fileNum, tfilePar.h.name.filepath, fileLen, offset,j);
			}
			else if(fileLen > sizeof(tfilePar.fData) && (offset-sizeof(tfilePar.fData)) > DfGetBeiSu(fileLen-sizeof(tfilePar.fData),GFS_BLOCK_SIZE))
			{
				TRACE("#2[%d] name[%s] len[%d,%d]-[%d]\r\n",fileNum, tfilePar.h.name.filepath, fileLen, offset,j);
			}
			else
			{
				TRACE("Ok[%d] name[%s] len[%d,%d]-[%d]\r\n",fileNum,tfilePar.h.name.filepath,fileLen,offset,j);
			}
			AllfileLen += fileLen;
		}
	}
	TRACE("allfile num[%d] blockNUN[%d<%d] len[%d]\r\n",fileNum,blockNUN,gBasiGlobal.gfsBlockSize,AllfileLen);
	blockMax = sizeof(gBasiGlobal.g.NullBit)/sizeof(gBasiGlobal.g.NullBit[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gBasiGlobal.g.EraseBit[i] != 0xFFFFFFFF)
		{
			TRACE("***EraseBit[%d]=[%X]\r\n",i,gBasiGlobal.g.EraseBit[i]);
		}
		if(NullBit[i] !=  gBasiGlobal.g.NullBit[i])
		{
			TRACE("###NullBit[%d]=[%X != %X]\r\n",i,NullBit[i],gBasiGlobal.g.NullBit[i]);
		}
	}
	return Lenzero;
}


void gfs_clearNullfile(void)
{
	DfgFsUnitHead tfileh;  //	GTREE_FS_UNIT_SIZE
	u16 i,blockMax,block;
	if(gBasiGlobal.gfsBlockSize == 0)	
		return;
	blockMax = sizeof(gBasiGlobal.g.BlockIndex)/sizeof(gBasiGlobal.g.BlockIndex[0]);
	for(i=0;i<blockMax;i++)
	{
		if(gBasiGlobal.g.BlockIndex[i] != GFS_INDEX_NULL)
		{
			gfs_FsReadblock(gfs_GetBlockIndex(i),0,&tfileh,sizeof(tfileh));
			if((~(tfileh.InvLen)) == 0) 
			{
				gfs_RecordDelete(tfileh.fdIndex,sizeof(tfileh.fdIndex)/sizeof(tfileh.fdIndex[0]));
				block = gfs_GetBlockIndex(i);
				gfs_RecordBlockIndex(i,GFS_INDEX_NULL); //清除入口地址
				gfs_RecordDelete(&block,1);
				gfs_BalancedGlobalSave(1);
				gfs_CheckEraseBlockBit();
			}
		}
	}
}

#endif

