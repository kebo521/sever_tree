
#ifndef MBED_LFSFILESYSTEM_H
#define MBED_LFSFILESYSTEM_H

// default parameters
#define GFS_READ_SIZE 4096
#define GFS_PROG_SIZE 4096
#define GFS_BLOCK_COUNT 1024  //filesystem size = LFS_BLOCK_COUNT*LFS_BLOCK_SIZE
#define GFS_LOOKAHEAD 4096




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
};

// File seek flags
enum lfs_whence_flags {
    GFS_SEEK_SET = 0,   // Seek relative to an absolute position
    GFS_SEEK_CUR = 1,   // Seek relative to the current file position
    GFS_SEEK_END = 2,   // Seek relative to the end of the file
};


extern int gfs_remove(const char *path);
extern DfGFsUnitfd* gfs_open(const char *path, int flags);
extern int gfs_seek(DfGFsUnitfd *pfd,int offset, int whence);
extern int gfs_read(DfGFsUnitfd* pfd,unsigned char *buff,int size);
extern int gfs_write(DfGFsUnitfd* pfd,unsigned char *buff,int writeLen);
extern int gfs_close(DfGFsUnitfd* pfd);
extern int gfs_GetSize(DfGFsUnitfd* pfd);
//=========gTreeFileSystem 对外标准封装接口===============
/*

extern int gfs_CreateDir(char *path);
extern int gfs_remove(const char *path);
extern void* gfs_open(const char *path, int flags);
extern int gfs_seek(lfs_file_t *pfd,int offset, int whence);
extern int gfs_read(lfs_file_t* pfd,char *buff,int size);
extern int gfs_write(lfs_file_t* pfd,char *buff,int writeLen);
extern int gfs_close(lfs_file_t* pfd);
extern int gfs_GetSize(lfs_file_t* pfd);
*/

#endif
