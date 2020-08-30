#ifndef _APP_SDK_
#define _APP_SDK_

#include "gfs_api.h"


extern ALG_STATUS sm2_verify(u8 *pubkey,u8 *e,u8 *signature);
extern ALG_STATUS sm4_ecb_encrypt_decrypt(ALG_EN_DE_MODE en_de_mode,u8 *key,u8 *src,u8 *dest,u32 datalen);
extern void api_md_hash(HASH_MODE hashType,UINT8 *input, int ilen,UINT8 *output);
extern ALG_STATUS sm2_generate_keypair(UINT8  * pubkey,UINT8  * prvkey);
extern ALG_STATUS sm2_encrypt(UINT8 *pubkey,UINT8 *Message,UINT32 MessageLen,UINT8 *Crypto,UINT32 *CryptoLen);
extern ALG_STATUS sm2_decrypt(UINT8 *prvkey,UINT8 *Crypto,UINT32 CryptoLen,UINT8 *Message,UINT32 *MessageLen);
//===================================================================================


extern void api_rand(u32 *pRng,int num);
extern int api_sm_sign2(u8 *prvkey,u8 *pIn,int len,u8* pOutSign);
extern int GetSessionKey_ATC(u8 *key,u8 *pATC,u8* pOutKey);
extern int GetMAC(u8 *Mackey,u8 *pIndata,int inlen,u8* pOutMac);
extern int GetMAC_ATC(u8 *key,u8 *pATC,u8 *pIndata,int inlen,u8* pOutMac);

extern u8 *eMemseek(u8* pbuf1,int len1,u8* pbuf2,int len2);


extern u32 Conv_Hex4ToNum(u8* pbuf,u8 Inlen);
extern u64 Conv_Hex8ToNum(u8* pbuf,u8 Inlen);
extern void Conv_NumToHex(u32 num,u8* pbuf,int size);
extern void Conv_NumToDecimal(u32 num,u8* pbuf,int size);
extern u32 Conv_DecimalToNum(u8* pbuf,int size);
extern void Conv_BcdToStr(u8* bcd,s16 blen,char* str);
extern void Conv_buffXor(u8* destination,u8* source1,u8* source2,int len);

extern int	API_Uart_PackSend(u8* pSendBuf,u32 sendLen);
extern u8*	API_Uart_PackRecv(u8* pRecvBuf,u32 *recvLen);

extern int APP_FileSaveBuff(const char * pfilePath,u32* pBuff,int len);
extern int APP_FileReadBuff(const char * pfilePath,u32* pBuff,int size);

extern int APP_FileReadCont(const char * pfilePath,u32* pBuff,u32 num);
extern int APP_FileSaveCont(const char * pfilePath,u32* pBuff,u32 num);


#endif
