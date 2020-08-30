#ifndef _COMM_STU_
#define _COMM_STU_

#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

typedef enum{
	LOG_DEBUG, 
	LOG_INFO, 
	LOG_WARN, 
	LOG_ERROR,
	LOG_NULL,	//�޵�����Ϣ
} LOG_T;


#define CURR_LOG_T						LOG_WARN			
extern void OsLogSetTag(const char *Tag);
extern void OsLogSet(LOG_T CurrPrio,int clog);
extern int OsLog(LOG_T Prio,const char *fmt,...);
#define LogSetTag(file)					OsLogSetTag(file)	
#define LOG(t,...)						OsLog(t, ## __VA_ARGS__)
#define TRACE(...)						OsLog(LOG_INFO, ## __VA_ARGS__)
extern void OsLogHex(LOG_T Prio,char* msg,void* pBuff,int Len);
#define LOG_HEX(logT,msg,pBuff,Len)		OsLogHex(logT,msg,pBuff,Len)
#define TRACE_HEX(msg,pBuff,Len)		OsLogHex(LOG_INFO,msg,pBuff,Len)
*/

//#define DISPLAY_HORIZONTAL_SCREEN		//��������

//#define FILE_SHARD_MEMORY

typedef enum _ret_t { RET_OK = 0, RET_OOM, RET_ERR,RET_FAIL, RET_BAD_PARAMS } ret_t;

#define		BYTE2_TO_INT(buf)	(buf[0]*0x100+buf[1])
#define		BYTE4_TO_INT(buf)	(buf[0]*0x1000000+buf[1]*0x10000+buf[2]*0x100+buf[3])

#define		DfGetStr2Len(nL,sL)	{nL=sL[0]*0x100+sL[1];}
#define		DfSetStr2Len(sL,nL)	{sL[0]=(u8)(nL/0x100); sL[1]=(u8)nL;}
#define 	DfGetStr4Len(nL,sL) {nL=sL[0]*0x1000000+sL[1]*0x10000+sL[2]*0x100+sL[3];}
#define 	DfSetStr4Len(sL,nL) {sL[0]=nL/0x1000000; sL[1]=(u8)(nL/0x10000);sL[2]=(u8)(nL/0x100);sL[3]=(u8)nL;}
#define		DfGetBeiSu(nL,n)	(((nL)+(n-1))&(~(n-1)))	//�������������2/4/8/16/32...
#ifndef offsetof
#define 	offsetof(bak,item)	(int)(&((bak *)0)->item)
#endif
#ifndef offsize
#define 	offsize(bak,item)	(sizeof(((bak *)0)->item))
#endif
#define 	_BIT(__n)       	(1<<(__n))
#define		MIN(a,b)			(((a) < (b)) ? (a) : (b))
#define		MAX(a,b)			(((a) > (b)) ? (a) : (b))


/*

typedef int8_t			s8;     //!< Define s8 ����������charΪ��unsigned��������ʹ��signed����
typedef uint8_t   		u8;		//!< Define u8
typedef int16_t		s16;	//!< Define s16
typedef uint16_t  		u16;	//!< Define u16
typedef int32_t		s32;	//!< Define s32
typedef uint32_t		u32;	//!< Define u32
*/

typedef signed char				s8;     //!< Define s8 ����������charΪ��unsigned��������ʹ��signed����
typedef unsigned char   		u8;		//!< Define u8

typedef signed short			s16;	//!< Define s16
typedef unsigned short  		u16;	//!< Define u16

typedef signed int				s32;	//!< Define s32
typedef unsigned int			u32;	//!< Define u32

typedef unsigned long long		u64;	//!< Define u32
//======================================================================
#ifndef TRUE
#define TRUE 1
#endif /*TRUE*/

#ifndef FALSE
#define FALSE 0
#endif /*FALSE*/

#ifndef bool_t
#define bool_t uint8_t
#endif /*bool_t*/

#ifndef bool
#define bool uint8_t
#endif /*bool*/

#ifndef FILE
#define FILE void*
#endif /*bool*/

typedef 	void (*PFUNC)(void);			//�պ���ָ������(Guozu add)
typedef 	int (*R_PFUNC)(void); 
typedef int (*CHECK_DATA_FULL)(u8 *,int); 


//======================================================================
#define API_atoi		atoi
#define API_strstr		strstr
#define API_strlen		strlen
#define API_strcpy		strcpy
#define API_strcmp		strcmp
#define API_strcat		strcat
#define API_memcpy		memcpy
#define API_memcmp		memcmp
#define API_memset		memset
#define API_memmove	memmove
#define API_sprintf		sprintf
#define	CLEAR(st)		memset(&(st),0,sizeof(st))
//===================================================================================
typedef struct {
	char Name[64];
	char Version[32];
} ST_OPT_INFO;


typedef struct{
	char Id[64];
	char Name[64];
	char Bin[64];
	char Artwork[64];
	char Desc[64];
	char Vender[32];
	char Version[32] ;
} ST_APP_INFO;


typedef struct
{
	u32 uFileSize;
	u32 uOffset;
	u8	upFlag;
	char sAppVer[15];
	u8	sFileMd5[20];
	char sFileNo[64];
}tData_UpIni;

typedef struct
{
	u16	LightSleepTimeS;		//ǳ����ʱ��
	u16	DeepSleepTimeS;		//�������ʱ��
	//-------------------------------
	u16	TmsPackMax;			//Tms���ؽ������������
	u16	TmsPost;				//Tms���ʶ˿�
	char TmsAddre[35];			//Tms���ʵ�ַ
	u8	TmsSSL;				//Tms���ʷ�ʽ

	u8	WifiOpen; 		//Wifi����
	u8	volume;		//������С
	u8	language;		//����
	u8	DebugMode;		//���Կ���
}tData_Set;

typedef struct
{
	char	HardSN[34];
	u8		SnLen;
	char	Life[4];	//HARD_SN_STATUS_WR -> HARD_SN_STATUS_UP -> HARD_SN_STATUS_DOWN -> HARD_SN_STATUS_DOWN
	char	LifeEnd;	//'\0'
	u8		KeyIndex;
	u8		TMK[16];
	u8		Addcheck;
}tHardSN_Key;


typedef struct{
	int				pid,sig;
	tData_UpIni		tUpIni;
	tData_Set		tTermSet;
	tHardSN_Key		tHardSnKey;
	unsigned short 	mAppMax,nCurrAppId;
	unsigned short 	nDefAppId,nNouser;
	ST_APP_INFO 	AppInfo[6];
	unsigned short 	mOptMax,nOptId;
	ST_OPT_INFO 	OptInfo[10];
} ST_SYS_MSG;

typedef struct{
	int 			AppExitCode;
	unsigned short 	nCurrAppId,pNextAppId;
	unsigned char  aAppIDStack[20];
	unsigned short	sWriteLen,sReadLen;
	unsigned char  sendBuff[2048];
	unsigned short	rWriteLen,rReadLen;
	unsigned char  readBuff[2048];
} ST_SYS_DATA;

#endif
