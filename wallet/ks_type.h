#ifndef _KS_TYPE
#define _KS_TYPE

#include "comm_type.h"
#include "comm.h"

typedef unsigned char			UINT8;	
typedef unsigned int			UINT32;	

typedef enum{
	ALG_ENCRYPT = 0,
	ALG_DECRYPT = 1
} ALG_EN_DE_MODE;


typedef enum{
	MODE_SM3 = 0x00,
	MODE_SHA0 = 0x10,
	MODE_SHA1 = 0x20,
	MODE_SHA224 = 0x30,
	MODE_SHA256 = 0x40,
} HASH_MODE;

typedef enum
{
	SUCCESS_ALG          =  1,
	FAIL_ALG             =  0,
	ERROR_DATA_LENGTH    = -1,
	ERROR_KEY_LENGTH     = -2,
	ERROR_KEY_MODE       = -3,
	ERROR_ENDE_MODE      = -4,
	ERROR_ECBCBC_MODE    = -5,
	ERROR_DATA_BUF       = -6,
	ERROR_OTHER_PARA     = -7,
}ALG_STATUS;


/**
* SM2公钥
*/
typedef struct 
{
  u8 xQ[32]; //!< 椭圆曲线上一点x
  u8 yQ[32]; //!< 椭圆曲线上一点y
} SM2_PUBKEY;

/**
* SM2私钥
*/
typedef struct 
{
  u8 xQ[32]; //!< 椭圆曲线上一点x
  u8 yQ[32]; //!< 椭圆曲线上一点x
  u8  d[32]; //!< 随机数k
} SM2_PRIVKEY;



#endif
