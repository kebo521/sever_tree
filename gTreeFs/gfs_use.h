#ifndef _GFS_USE_
#define _GFS_USE_

//#include "gfs.h"
#include "gfs_api.h"


extern int api_TremParInit(void *pData,int size);
extern int api_TremParRead(void *pData);
extern int api_TremParWrite(void *pData);


extern int api_RecordInit(void *pData,int size);
extern int api_RecordRead(void *pData);
extern int api_RecordWrite(void *pData);


extern int api_fileInit(void);
extern int api_fileFomat(const char* pKey);

/*
gFILE* pFd_gfs=NULL;
pFd_gfs=gfs_open("pathname",GFS_O_RDWR);
ret=gfs_write(pFd_gfs,(u8*)pInPar->pStr,pInPar->Len);
ret=gfs_read(pFd_gfs,(u8*)pData,pInPar->Len);
ret = gfs_seek(pFd_gfs, pInPar->uData, pInPar->pNext->uData);
ret=gfs_close(pFd_gfs);
ret=gfs_remove(pInPar->pStr);
*/
typedef struct	
{
	char	Mask[4];			// "gfs"
	LPgFILE (*open)(const char*, int,...);	//(const char *path, int flags ,...)
	int (*close)(LPgFILE);				//(void* pfd)
	int (*seek)(LPgFILE,int, int);	//(void* pfd,int offset, int whence)
	int (*read)(LPgFILE,void *,int);	//(void* pfd,void *buff,int size)
	int (*write)(LPgFILE,void *,int); //(void* pfd,void *buff,int writeLen)
	int (*getSize)(LPgFILE);			//(void* pfd)
	int (*readSize)(const char*);	//(const char *path)
	int (*remove)(const char*);		//(const char *path)
		
	int (*fileFomat)(const char*);	//(const char* pKey)
	//--后进先出 队列记录器，独立FLASH操作文件系统之外-------------------
	int (*LifoRecordInit)(void *,int);	//(void *pData,int size)
	int (*LifoRecordRead)(void *,int);		//(void *pData,int Index)
	int (*LifoRecordWrite)(void *);		//(void *pData)
}API_FILE_Def;

extern const API_FILE_Def ApiGfs;


#endif
