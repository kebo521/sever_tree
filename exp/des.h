//===================================================================
// 单    位：深圳市证通金信科技有限公司
// 作    者：付庆
// 版    本：0.00.01	[主版本号.次版本号.修订版本号]
// 创建日期：2006-04-10
// 修改日期：2006-04-10
// 说    明：DES模块
// 修改记录：
//===================================================================

#ifndef _DES_H_DENNIS
#define _DES_H_DENNIS



typedef struct 
{
	unsigned long ek[32];
	unsigned long dk[32];
} DES_CTX;


extern void Des_F_Func(unsigned long *block, unsigned long *keys)  ;
extern void Des_F_DesKey(unsigned char *key, short edf); 
		//                  hexkey[8]     MODE
		// Sets the internal key register according to the hexadecimal
		// key contained in the 8 bytes of hexkey, according to the DES,
		// for encryption or decryption according to MODE.


extern void Des_F_UseKey(unsigned long *from);  
		//                cookedkey[32]
		// Loads the internal key register with the data in cookedkey.


extern void Des_F_CpKey(unsigned long *into);  
		//               cookedkey[32]
		// Copies the contents of the internal key register into the storage
		// located at &cookedkey[0].

extern void Des_F_Des(unsigned char *inblock, unsigned char *outblock);  
		//                from[8]         to[8]
		// Encrypts/Decrypts (according to the key currently loaded in the
		// internal key register) one block of eight bytes at address 'from'
		// into the block at address 'to'.  They can be the same.

extern void Des_F_Scrunch(unsigned char *outof, unsigned long *into);  
extern void Des_F_Unscrun(unsigned long *outof, unsigned char *into);  
extern void Des_F_Desfunc(unsigned long *block, unsigned long *keys);  
extern void Des_F_CooKey(unsigned long *raw1);  

//==========================================================================================

extern void Des_Key(DES_CTX *dc, unsigned char *key);
extern void Des_Enc(DES_CTX *dc, unsigned char *Data, u16 blocks);
extern void Des_Dec(DES_CTX *dc, unsigned char *Data, u16 blocks);
extern void Des_Enc3(DES_CTX *dc1, DES_CTX *dc2, unsigned char *Data, unsigned char blocks);
extern void Des_Dec3(DES_CTX *dc1, DES_CTX *dc2, unsigned char *Data, unsigned char blocks);
//==========================================================================================


extern void Des_Mac (u8 * Data, s16 DataLen, u8 * Mac,u8 * MacCodeKey );

//==========================================================================================
extern void Des_B1Dec(u8 *s,u8 *passkey,u8 Dm);	// 1 des解密
extern void Des_B1Enc(u8 *s,u8 *passkey,u8 Dm);	 // 1des加密

extern void Des_B2Dec(u8 *s,u8 *passkey,u8 Dm);
extern void Des_B2Enc(u8 *s,u8 *passkey,u8 Dm);

extern void Des_B3Dec(u8*,u8*,u8);
extern void Des_B3Enc(u8*,u8*,u8);

#endif
