#if(0)
#include "ks_type.h"

#include "gmem.h"

#include "eflash_api.h"

#include "eflash_BR.h"
//========================================================================================================



typedef struct
{
	eFlashCheck f;
	u8 data[0];
}DfBalancedAccountUnit;

int BalancedAccountInit(DfBalancedRecord *pAccount,u16 BlockStart,u16 BlockNum,u16 aSize)
{
	u32 Addr;
	u16 bLen,max;
	eFlashCheck *phead;
	if((aSize+sizeof(eFlashCheck)) > DCBC_PAGE_SIZE) return -1;
	pAccount->pgSize = ((aSize+sizeof(eFlashCheck))+(GFS_PAGE_SIZE-1))&(~(GFS_PAGE_SIZE-1));
	pAccount->PageNum = DCBC_PAGE_SIZE / pAccount->pgSize;
	pAccount->PageMax = BlockNum * pAccount->PageNum;
	if(pAccount->PageMax == 0) return -2;
	pAccount->parLen = aSize;
	pAccount->blockStart = BlockStart;
	phead =(eFlashCheck *)gMalloc(pAccount->PageMax * sizeof(eFlashCheck));
	max = 0;
	for(pAccount->PageIndex=0;pAccount->PageIndex < pAccount->PageMax ;pAccount->PageIndex++)
	{
		Addr = DCBC_PAGE_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum))+ \
				pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
		eflash_read_B4buff(Addr,(UINT32*)&phead[max],sizeof(eFlashCheck) >> 2);
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
			gFree(phead);
			return pAccount->parLen;
		}
	}
	gFree(phead);
	return 0;
}

int BalancedAccountRead(DfBalancedRecord *pAccount,int PreviousIndex,void* pBuff)
{
	u32 Addr;
	int index;
	u32 buffdata[DCBC_PAGE_SIZE/4];
	DfBalancedAccountUnit *pUnit;
	if(pAccount->PageMax == 0) return -2;
	if(PreviousIndex > pAccount->PageMax) return -3;
	index = (int)pAccount->PageIndex - PreviousIndex;
	if(index < 0) index += pAccount->PageMax;

	Addr=DCBC_PAGE_SIZE*(pAccount->blockStart+(index / pAccount->PageNum))\
			+ pAccount->pgSize*(index % pAccount->PageNum);
	eflash_read_B4buff(Addr,buffdata,(sizeof(eFlashCheck)+pAccount->parLen)>>2);
	pUnit = (eFlashCheck *)buffdata;
	if(pUnit->f.head == GFS_BASIC_DATA && pUnit->f.pgSize == pAccount->pgSize)
	{
		memcpy(pBuff,pUnit->data,pAccount->parLen);
		return pAccount->parLen;
	}
	/*
	SPI_Flash_Read(Addr, &tUnit, sizeof(tUnit));
	if(tUnit.f.head == GFS_BASIC_DATA && tUnit.f.pgSize == pAccount->pgSize)
	{
		return SPI_Flash_Read(Addr+sizeof(tUnit),pBuff,pAccount->parLen);
	}
	*/
	return 0;
}

int BalancedAccountSave(DfBalancedRecord *pAccount,void* pBuff)
{
	u32 Addr,Maxlen;
	u32 buffdata[DCBC_PAGE_SIZE/4];
	u32 pCheck[DCBC_PAGE_SIZE/4];
	DfBalancedAccountUnit *pUnit;
	//u8	*pCheck;
	if(pAccount->PageMax == 0) return -2;
	Maxlen = sizeof(pUnit->f) + pAccount->parLen;
	//pUnit =(DfBalancedAccountUnit *)gMalloc(Maxlen * 2);
	pUnit =(DfBalancedAccountUnit *)buffdata;
	pUnit->f.head = GFS_BASIC_DATA;
	pUnit->f.pgSize = pAccount->pgSize;
	memcpy(pUnit->data,pBuff,pAccount->parLen);
	//pCheck = ((u8*)pUnit)+Maxlen;	//复用地址
	while(1)
	{
		pAccount->count++;
		pUnit->f.count = pAccount->count;
		pAccount->PageIndex++;
		if(pAccount->PageIndex >= pAccount->PageMax)
			pAccount->PageIndex=0;
	
		Addr=DCBC_PAGE_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum)) + \
			pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
		if(!(pAccount->PageIndex % pAccount->PageNum))
		{
			//SPI_Flash_ERASE_SECTOR(Addr);
			eflash_page_erase(Addr);
		}
		//TRACE("PageIndex[%d] Addr[%x]\r\n",pAccount->PageIndex,Addr);
		eflash_bulk_program(Addr,(Maxlen+3)>>2,(u32*)pUnit);
		eflash_read_B4buff(Addr,pCheck,(Maxlen+3)>>2);
		if(memcmp(pUnit,pCheck,Maxlen) == 0 ) break;
		//LOG(LOG_WARN,"**AccountSave PageIndex[%d] len[%d]\r\n",pAccount->PageIndex,Maxlen);
		pUnit->f.head = GFS_BASIC_BAD;
		eflash_bulk_program(Addr,sizeof(DfBalancedAccountUnit)>>2,(u32*)pUnit);
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
			//SPI_Flash_ERASE_SECTOR(Addr);
			Addr=DCBC_PAGE_SIZE*(pAccount->blockStart+(pAccount->PageIndex / pAccount->PageNum)) + \
				pAccount->pgSize * (pAccount->PageIndex % pAccount->PageNum);
			eflash_page_erase(Addr);
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
	ptreebHead=gMalloc(sizeof(DfBalancedRecord));
	if(ptreebHead==NULL) return NULL;
	if(uAddress&(DCBC_PAGE_SIZE-1)) return NULL;
	uLen /= DCBC_PAGE_SIZE;
	if(uLen<2) return NULL;
	uAddress /= DCBC_PAGE_SIZE;
	ret=BalancedAccountInit(ptreebHead,uAddress,uLen,size);
	if(size == ret && pData != NULL)
	{
		if(pData)
			return BalancedAccountRead(ptreebHead,0,pData);	
	}
	return ptreebHead;
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
	gFree(fd);
	return 0;
}


#endif

/*
static DfBalancedRecord gtreebHead	=	{0};
int LIFO_RecordInit(void *pData,int size)
{
	int ret;
	ret=BalancedAccountInit(&gtreebHead,GFS_COUNTER_ADDR/DCBC_PAGE_SIZE,GFS_COUNTER_SIZE/DCBC_PAGE_SIZE,size);
	if(size == ret && pData != NULL)
	{
		if(pData)
			return BalancedAccountRead(&gtreebHead,0,pData);	
	}
	return ret;
}
int LIFO_RecordRead(void *pData,int index)
{
	return BalancedAccountRead(&gtreebHead,index,pData);	
}

int LIFO_RecordWrite(void *pData)
{
	return BalancedAccountSave(&gtreebHead,pData);	
}
*/
