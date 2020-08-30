
#ifndef MBED_LFSFILESYSTEM_H
#define MBED_LFSFILESYSTEM_H
#include "comm_type.h"
#include "comm.h"

#define GTREE_FILE_COURCE

#define GTREE_FILE_ERR_RECORD

#define WINDOW_FLASH_SOFT

#include "gfs_flash.h"
#include "gfs.h"
#include "gfs_BR.h"
//#include "gfs_api.h"

// Possible error codes, these are negative to allow
// valid positive return values
enum lfs_error {
    GFS_OK      	= 0,    // No error
    GFS_ERR_DIR      = -2,   // Error during device operation
	GFS_ERR_NORD_PER= -4,	// No write permission
	GFS_ERR_NOWR_PER= -5,	// No write permission
	GFS_ERR_LEN		= -7,	// No write permission
    GFS_ERR_CORRUPT = -52,  // Corrupted
    GFS_ERR_NOENT   = -2,   // No directory entry
    GFS_ERR_EXIST   = -17,  // Entry already exists
    GFS_ERR_NOTDIR  = -20,  // Entry is not a dir
    GFS_ERR_ISDIR   = -21,  // Entry is a dir
    GFS_ERR_INVAL   = -22,  // Invalid parameter
    GFS_ERR_NOSPC   = -28,  // No space left on device
    GFS_ERR_NOMEM   = -12,  // No more memory available
};

// File open flags
enum lfs_open_flags {
    // open flags
    GFS_O_RDONLY = 1,        // Open a file as read only
    GFS_O_WRONLY = 2,        // Open a file as write only
    GFS_O_RDWR   = 3,        // Open a file as read and write
    GFS_O_CREAT  = 0x0100,   // Create a file if it does not exist
    GFS_O_TRUNC  = 0x0200,   // Truncate the existing file to zero size
    GFS_O_APPEND = 0x0400,   // Move to end of file on every write
    GFS_O_ENC 	 = 0x0800,   // Encrypt file
    GFS_O_COUNT  = 0x1000,   // Counter file
};

// File seek flags
enum lfs_whence_flags {
    GFS_SEEK_SET = 0,   // Seek relative to an absolute position
    GFS_SEEK_CUR = 1,   // Seek relative to the current file position
    GFS_SEEK_END = 2,   // Seek relative to the end of the file
};


extern int gfs_remove(const char *path);
extern gFILE* gfs_open(const char *path, int flags ,...);
extern int gfs_seek(gFILE *pfd,int offset, int whence);
extern int gfs_read(gFILE* pfd,void *buff,int size);
extern int gfs_write(gFILE* pfd,void *buff,int writeLen);
extern int gfs_close(gFILE* pfd);
extern int gfs_GetSize(gFILE* pfd);
extern void* gfs_GetData2K(gFILE* pfd);
extern int gfs_ReadSize(const char *path);

//=============证通文件系统接口========================

#define	F_RDONLY	GFS_O_RDONLY		//!< 只读
#define	F_WRONLY	GFS_O_WRONLY		//!< 只写
#define	F_RDWR		GFS_O_RDWR		//!< 读写
#define	F_CREAT		GFS_O_CREAT		//!< 自动创建
#define	F_APPEND	GFS_O_APPEND	//!< 追加到尾部
#define	F_CRNEW		GFS_O_TRUNC		//!< 删除当前，创建新的
#define	F_ENC		GFS_O_ENC		//!< 加密文件 ，后面变参为密码
#define	F_CONT		GFS_O_COUNT		//!< 加密文件 ，后面变参为密码

/**
 * 文件seek模式
 */
#define	F_SEEK_SET	GFS_SEEK_SET		//!< 文件开头
#define	F_SEEK_CUR	GFS_SEEK_CUR		//!< 当前位置
#define	F_SEEK_END	GFS_SEEK_END		//!< 文件结尾

#define API_fopen(name,flag,...) 	gfs_open(name,flag,## __VA_ARGS__)
#define API_fclose(fd)				gfs_close(fd)
#define API_fseek(fd,offset,whence)	gfs_seek(fd,offset,whence)
#define API_fread(fd,buf,count)		gfs_read(fd,buf,count)
#define API_fwrite(fd,buf,count)	gfs_write(fd,buf,count)
#define API_fremove(name)			gfs_remove(name)
#define API_fsize(name)				gfs_ReadSize(name)

//---------------------------------------------------------


//--------文件系统测使用空间---------------------------------
#define GFS_FILE_START				((TEST_FLASH_START/SPI_SECTOR_SIZE)+248)		// >124K
#define GFS_FILE_COUNTSIZE			4	//(16)		//SPI_BLOCK_SIZE/SPI_SECTOR_SIZE
#define GFS_FILE_SIZE				256	//(256*512)

extern void api_gFileInit(void);


#include "sdebug.h"

//#include "comm.h"

#endif
