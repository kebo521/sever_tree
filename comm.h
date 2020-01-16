#ifndef _COMM_
#define _COMM_


#define GTREE_FILE_COURCE
#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef signed char				s8;     //!< Define s8 编译器决定char为必unsigned，所以须使用signed定义
typedef unsigned char   		u8;		//!< Define u8

typedef unsigned char   		BYTE;		//!< Define u8
//typedef unsigned char   		uint8_t;

typedef signed short			s16;	//!< Define s16
typedef unsigned short  		u16;	//!< Define u16

typedef signed int				s32;	//!< Define s32
typedef unsigned int			u32;	//!< Define u32

//typedef signed long				uint32;	//!< Define s32
//typedef unsigned long			uint32_t;	//!< Define u32


typedef unsigned long long      u64;	//!< Define u64

//typedef signed short				size_s;	
//typedef unsigned short				size_t;	

typedef int				      BOOL;
#define TRUE				1	
#define FALSE				0	

#define API_memcpy			memcpy
#define API_memset			memset
#define API_memcmp			memcmp
#define API_strcpy			strcpy
#define API_strcmp			strcmp
#define API_strlen			strlen

#define		DfGetStr2Len(nL,sL)	{nL=sL[0]*0x100+sL[1];}
#define		DfSetStr2Len(sL,nL)	{sL[0]=(u8)(nL/0x100); sL[1]=(u8)nL;}


#include "sdebug.h"

#include "tree.h"
#include "TagBuff.h"

#include "ExpresBasics.h"
#include "ExpresLogic.h"
#include "ExpresFunction.h"

#include "base64.h"

#include "des.h"
#include "sm4.h"

//#include "gfs_util.h"
//#include "gfs.h"
//#include "gfs_BR.h"
//#include "gTreeFs_API.h"
//#include "gfs_spiflash_bsd.h"



#endif
