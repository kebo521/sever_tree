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

#ifdef GTREE_FILE_ERR_RECORD
u32	SaveDataErr=0,SaveHearErr=0,EraseTimes=0;
u32 gfs_erase_times=0;
#endif




//========================================================================================================

typedef struct
{
	u16	head;	//块头:[FFFF] 已经察除，[48||84] 坏块，[EEEE] 引导数据，[DDDD] 使用数据
	u16	pgSize;	//块大小
	int count;
}eFlashCheck;


typedef struct
{
	eFlashCheck f;
	u32 data[0];
}DfBalancedAccountUnit;

int BalancedAccountInit(DfBalancedRecord *pAccount,u16 BlockStart,u16 BlockNum,u16 aSize)
{
	u32 Addr;
	u16 bLen,max;
	eFlashCheck *phead;
	if((aSize+sizeof(eFlashCheck)) > GFS_BLOCK_SIZE) return -1;
	if(aSize&0x03) return -2;
	pAccount->pgSize = ((aSize+sizeof(eFlashCheck))+(GFS_PAGE_SIZE-1))&(~(GFS_PAGE_SIZE-1));
	pAccount->PageNum = GFS_BLOCK_SIZE / pAccount->pgSize;
	pAccount->PageMax = BlockNum * pAccount->PageNum;
	if(pAccount->PageMax == 0) return -3;
	pAccount->parLen = aSize;
	pAccount->blockStart = BlockStart;
	phead =(eFlashCheck *)malloc(pAccount->PageMax * sizeof(eFlashCheck));
	max = 0;
	for(pAccount->PageIndex=0;pAccount->PageIndex < pAccount->PageMax ;pAccount->PageIndex++)
	{
		Addr = GFS_BLOCK_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum))+ \
				pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
		SPI_Flash_Read(Addr,(u32*)&phead[max],sizeof(eFlashCheck));
		if(phead[max].head == GFS_BASIC_DATA && phead[max].pgSize == pAccount->pgSize)
		{
			phead[max].head = pAccount->PageIndex;	//借用 head
			max++;
		}	
	}
	TRACE("BalancedAccountInit->max[%d]\r\n",max);
	pAccount->count = 0;
	for(bLen=0;bLen<max;bLen++)
	{//-----------------检索出最后一条记录-----------------------------
		if(((bLen+1) == max) ||((phead[bLen].count - phead[bLen+1].count)>0))
		{
			pAccount->count = phead[bLen].count;
			pAccount->PageIndex = phead[bLen].head;
			TRACE("Ok->PageIndex[%d]->count[%d]pgSize[%d]bLen[%d]\r\n",pAccount->PageIndex,pAccount->count,pAccount->pgSize,bLen);
			free(phead);
			return pAccount->parLen;
		}
	}
	free(phead);
	return 0;
}

int BalancedAccountRead(DfBalancedRecord *pAccount,int PreviousIndex,void* pBuff)
{
	u32 Addr;
	int index;
	u32 buffdata[GFS_BLOCK_SIZE/4];
	DfBalancedAccountUnit *pUnit;
	if(pAccount->PageMax == 0) return -2;
	if(PreviousIndex > pAccount->PageMax) return -3;
	index = (int)pAccount->PageIndex - PreviousIndex;
	if(index < 0) index += pAccount->PageMax;

	Addr=GFS_BLOCK_SIZE*(pAccount->blockStart+(index / pAccount->PageNum))\
			+ pAccount->pgSize*(index % pAccount->PageNum);
	SPI_Flash_Read(Addr,buffdata,sizeof(eFlashCheck)+DfGetBeiSu(pAccount->parLen,4));
	pUnit = (DfBalancedAccountUnit *)buffdata;
	if(pUnit->f.head == GFS_BASIC_DATA && pUnit->f.pgSize == pAccount->pgSize)
	{
		memcpy(pBuff,pUnit->data,pAccount->parLen);
		return pAccount->parLen;
	}
	return 0;
}

int BalancedAccountSave(DfBalancedRecord *pAccount,void* pBuff)
{
	u32 Addr;
	u16 lenPar,Maxlen;
	u32 buffdata[GFS_BLOCK_SIZE/4];
	u32 pCheck[GFS_BLOCK_SIZE/4];
	DfBalancedAccountUnit *pUnit;
	//u8	*pCheck;
	if(pAccount->PageMax == 0) return -2;
	lenPar = sizeof(pUnit->f) + pAccount->parLen;
	pUnit =(DfBalancedAccountUnit *)buffdata;
	pUnit->f.head = GFS_BASIC_DATA;
	pUnit->f.pgSize = pAccount->pgSize;
	memcpy(pUnit->data,pBuff,pAccount->parLen);
	Maxlen = DfGetBeiSu(lenPar,4);
	//pCheck = ((u8*)pUnit)+Maxlen;	//复用地址
	while(1)
	{
		pAccount->count++;
		pUnit->f.count = pAccount->count;
		pAccount->PageIndex++;
		if(pAccount->PageIndex >= pAccount->PageMax)
			pAccount->PageIndex=0;
	
		Addr=GFS_BLOCK_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum)) + \
			pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
		if(!(pAccount->PageIndex % pAccount->PageNum))
		{
			SPI_Flash_EraseOne(Addr);
		}
		//TRACE("PageIndex[%d] Addr[%x]\r\n",pAccount->PageIndex,Addr);
		SPI_Flash_Write(Addr,pUnit,Maxlen);
		SPI_Flash_Read(Addr,pCheck,Maxlen);
		if(memcmp(pUnit,pCheck,lenPar) == 0 ) break;
		//LOG(LOG_WARN,"**AccountSave PageIndex[%d] len[%d]\r\n",pAccount->PageIndex,Maxlen);
		pUnit->f.head = GFS_BASIC_BAD;
		SPI_Flash_Write(Addr,(u32*)pUnit,sizeof(DfBalancedAccountUnit));
	}
	//free(pUnit);
	return pAccount->parLen;
}

int BalancedAccountClear(DfBalancedRecord *pAccount)
{
	u32 Addr;
	if(pAccount->PageMax == 0) return -2;
	for(pAccount->PageIndex=0; pAccount->PageIndex<pAccount->PageMax; pAccount->PageIndex++)
	{		
		if(!(pAccount->PageIndex % pAccount->PageNum))
		{
			Addr=GFS_BLOCK_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum)) + \
				pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
			SPI_Flash_EraseOne(Addr);
		}
	}
	pAccount->PageIndex=0;
	pAccount->count=0;
	//free(pUnit);
	return 0;
}


void* fOpen_Record(u32 uAddress,u32 uLen,void *pData,int size)
{
	int ret;
	DfBalancedRecord* ptreebHead;
	ptreebHead=malloc(sizeof(DfBalancedRecord));
	if(ptreebHead==NULL) return NULL;
	if(uAddress&(GFS_BLOCK_SIZE-1)) return NULL;
	uLen /= GFS_BLOCK_SIZE;
	if(uLen<2) return NULL;
	uAddress /= GFS_BLOCK_SIZE;
	ret=BalancedAccountInit(ptreebHead,uAddress,uLen,size);
	if(size == ret && pData != NULL)
	{
		if(pData)
			return (void*)BalancedAccountRead(ptreebHead,0,pData);	
	}
	return (void*)ptreebHead;
}
int fRead_Record(void *fd,     void *pData)
{
	if(fd==NULL) return -5;
	return BalancedAccountRead((DfBalancedRecord*)fd,0,pData);	
}

int fWrite_Record(void *fd,      void *pData)
{
	if(fd==NULL) return -5;
	return BalancedAccountSave((DfBalancedRecord*)fd,pData);	
}

int fClear_Record(void *fd)
{
	if(fd==NULL) return -5;
	BalancedAccountClear(fd);
	return 0;
}

int fClose_Record(void *fd)
{
	if(fd==NULL) return -5;
	free(fd);
	return 0;
}



#endif


