
#include "gfs_api.h"
#include "gfs_use.h"
#include "gfs.h"


#if(0)	//def GTREE_FILE_COURCE
//--------------4M-32*4096---28----------------------------
#define GFS_COUNTER_ADDR			(4*256-32)
#define GFS_COUNTER_SIZE			(16+12)
//-----------------ÖÕ¶Ë²ÎÊýÇø--4---------------------------------
#define GFS_PAR_ADDR				(4*256-4)
#define GFS_PAR_SIZE				(2)
//--------------4M-16*4096--16,4M-----------------------------
#define GFS_FILE_COUNTSTART			(4*256)		//SPI_BLOCK_SIZE->
#define GFS_FILE_COUNTSIZE			(16)		//SPI_BLOCK_SIZE/SPI_SECTOR_SIZE
#define GFS_FILE_SIZE				(4*256)


/*
DfBalancedHead gtreebHead={0};

static DfBalancedHead gtreebHead	=	{{0},NULL};

int api_RecordInit(void *pData,int size)
{
	int ret;
	ret=BalancedRecordInit(&gtreebHead,GFS_COUNTER_ADDR,GFS_COUNTER_SIZE,size);
	if(size == ret && pData != NULL)
	{
		return BalancedRecordRead(&gtreebHead,pData);	
	}
	return ret;
}

int api_RecordRead(void *pData)
{
	return BalancedRecordRead(&gtreebHead,pData);	
}

int api_RecordWrite(void *pData)
{
	return BalancedRecordSave(&gtreebHead,pData);	
}
*/

static DfBalancedAccount gtreebHead	=	{0};
int LIFO_RecordInit(void *pData,int size)
{
	int ret;
	ret=BalancedAccountInit(&gtreebHead,GFS_COUNTER_ADDR,GFS_COUNTER_SIZE,size);
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


static DfBalancedAccount gtreeParHead	=	{0};
int api_TremParInit(void *pData,int size)
{
	int ret;
	ret=BalancedAccountInit(&gtreeParHead,GFS_PAR_ADDR,GFS_PAR_SIZE,size);
	if(size == ret && pData != NULL)
	{
		return BalancedAccountRead(&gtreeParHead,0,pData);	
	}
	return ret;
}

int api_TremParRead(void *pData)
{
	return BalancedAccountRead(&gtreeParHead,0,pData);	
}

int api_TremParWrite(void *pData)
{
	return BalancedAccountSave(&gtreeParHead,pData);	
}


int api_fileInit(void)
{
	//SPI_Flash_init();
	return gfs_Init(GFS_FILE_COUNTSTART,GFS_FILE_SIZE,GFS_FILE_COUNTSIZE);
}	

int api_fileFomat(const char* pKey)
{
	if(strcmp(pKey,"123456")==0)
		return gfs_Fomat(NULL);
	return -1;
}	


const API_FILE_Def ApiGfs={
	{'g','f','s',12},
	gfs_open,
	gfs_close,
	gfs_seek,
	gfs_read,
	gfs_write,
	gfs_GetSize,
	gfs_ReadSize,
	gfs_remove,

	api_fileFomat,

	LIFO_RecordInit,
	LIFO_RecordRead,
	LIFO_RecordWrite,
};	
/*
gFILE* pFd_gfs=NULL;
pFd_gfs=gfs_open(pInPar->pStr,pInPar->pNext->uData);
ret=gfs_write(pFd_gfs,(u8*)pInPar->pStr,pInPar->Len);
ret=gfs_read(pFd_gfs,(u8*)pData,pInPar->Len);
ret = gfs_seek(pFd_gfs, pInPar->uData, pInPar->pNext->uData);
ret=gfs_close(pFd_gfs);
ret=gfs_remove(pInPar->pStr);


*/



#endif
