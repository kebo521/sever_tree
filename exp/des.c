//===================================================================
// 单    位：深圳市证通金信科技有限公司
// 作    者：付庆
// 版    本：0.00.01	[主版本号.次版本号.修订版本号]
// 创建日期：2006-04-10
// 修改日期：2006-04-10
// 说    明：DES模块
// 修改记录：
//===================================================================

#include "comm.h"


#define EN0   0      /* MODE == encrypt */
#define DE1   1      /* MODE == decrypt */


static unsigned long KnL[32] = { 0L };



const  unsigned char Df_Key[24] = 
{
       0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
       0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
       0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67 
};

const unsigned short bytebit[8]={0200, 0100, 040, 020, 010, 04, 02, 01 };

const unsigned long bigbyte[24] = 
{
       0x800000L,    0x400000L,     0x200000L,    0x100000L,
       0x80000L,     0x40000L,      0x20000L,     0x10000L,
       0x8000L,      0x4000L,       0x2000L,      0x1000L,
       0x800L,       0x400L,        0x200L,       0x100L,
       0x80L,        0x40L,         0x20L,        0x10L,
       0x8L,         0x4L,          0x2L,         0x1L   
};

// Use the key schedule specified in the Standard (ANSI X3.92-1981). 

const unsigned char pc1[56] =
{
       56, 48, 40, 32, 24, 16,  8,   0, 57, 49, 41, 33, 25, 17,
        9,  1, 58, 50, 42, 34, 26,  18, 10,  2, 59, 51, 43, 35,
       62, 54, 46, 38, 30, 22, 14,   6, 61, 53, 45, 37, 29, 21,
       13,  5, 60, 52, 44, 36, 28,  20, 12,  4, 27, 19, 11,  3 
};

const unsigned char totrot[16]={1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28 };

const unsigned char pc2[48] = 
{
       13, 16, 10, 23,  0,  4,       2, 27, 14,  5, 20,  9,
       22, 18, 11,  3, 25,  7,      15,  6, 26, 19, 12,  1,
       40, 51, 30, 36, 46, 54,      29, 39, 50, 44, 32, 47,
       43, 48, 38, 55, 33, 52,      45, 41, 49, 35, 28, 31 
};



const unsigned long SP1[64] = 
{
  0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
  0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
  0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
  0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
  0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
  0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
  0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
  0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
  0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
  0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
  0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
  0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
  0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
  0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
  0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
  0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L 
};

const unsigned long SP2[64] = 
{
  0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
  0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
  0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
  0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
  0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
  0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
  0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
  0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
  0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
  0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
  0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
  0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
  0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
  0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
  0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
  0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L 
};

const unsigned long SP3[64] = 
{
  0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
  0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
  0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
  0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
  0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
  0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
  0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
  0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
  0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
  0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
  0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
  0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
  0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
  0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
  0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
  0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L 
};

const unsigned long SP4[64] = 
{
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
  0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
  0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
  0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
  0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
  0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
  0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
  0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
  0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L 
};

const unsigned long SP5[64] = 
{
  0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
  0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
  0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
  0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
  0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
  0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
  0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
  0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
  0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
  0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
  0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
  0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
  0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
  0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
  0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
  0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L 
 };

const unsigned long SP6[64] = 
{
  0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
  0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
  0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
  0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
  0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
  0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
  0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
  0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
  0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
  0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
  0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
  0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
  0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
  0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
  0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
  0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L 
};

const unsigned long SP7[64] = 
{
  0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
  0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
  0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
  0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
  0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
  0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
  0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
  0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
  0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
  0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
  0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
  0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
  0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
  0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
  0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
  0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L 
};

const unsigned long SP8[64] = 
{
  0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
  0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
  0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
  0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
  0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
  0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
  0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
  0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
  0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
  0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
  0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
  0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
  0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
  0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
  0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
  0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L 
};








void Des_F_DesKey(unsigned char *key, short edf)  
{
  // Thanks to James Gillogly & Phil Karn! 
  s16 i, j, l, m, n;
  u8 pc1m[56], pcr[56];
  unsigned long kn[32];

  for ( j = 0; j < 56; j++ ) 
  {
    l = pc1[j];
    m = l & 07;
    pc1m[j] = (key[l >> 3] & bytebit[m]) ? 1 : 0;
  }
  for( i = 0; i < 16; i++ ) 
  {
    if( edf == DE1 ) m = (15 - i) << 1;
    else             m = i << 1;
    n = m + 1;
    kn[m] = kn[n] = 0L;
    for( j = 0; j < 28; j++ ) 
    {
      l = j + totrot[i];
      if( l < 28 ) pcr[j] = pc1m[l];
      else pcr[j] = pc1m[l - 28];
    }
    for( j = 28; j < 56; j++ ) 
    {
      l = j + totrot[i];
      if( l < 56 ) pcr[j] = pc1m[l];
      else         pcr[j] = pc1m[l - 28];
    }
    for( j = 0; j < 24; j++ ) 
    {
      if( pcr[pc2[j]] )    kn[m] |= bigbyte[j];
      if( pcr[pc2[j+24]] ) kn[n] |= bigbyte[j];
    }
  }
  Des_F_CooKey(kn);
}

void Des_F_CooKey(unsigned long *raw1)  
{
  unsigned long *cook, *raw0;
  unsigned long dough[32];
  s16 i;

  cook = dough;
  for( i = 0; i < 16; i++, raw1++ ) 
  {
    raw0 = raw1++;
    *cook   = (*raw0 & 0x00fc0000L) << 6;
    *cook  |= (*raw0 & 0x00000fc0L) << 10;
    *cook  |= (*raw1 & 0x00fc0000L) >> 10;
    *cook++|= (*raw1 & 0x00000fc0L) >> 6;
    *cook   = (*raw0 & 0x0003f000L) << 12;
    *cook  |= (*raw0 & 0x0000003fL) << 16;
    *cook  |= (*raw1 & 0x0003f000L) >> 4;
    *cook++       |= (*raw1 & 0x0000003fL);
  }
  Des_F_UseKey(dough);
}

void Des_F_CpKey(unsigned long *into)  
{
  unsigned long *from, *endp;

  from = KnL, endp = &KnL[32];
  while( from < endp ) *into++ = *from++;
}

void Des_F_UseKey(unsigned long *from)  
{
  unsigned long *to, *endp;

  to = KnL, endp = &KnL[32];
  while( to < endp ) *to++ = *from++;
}

#if 0
void Des_F_Des(unsigned char *inblock, unsigned char *outblock)  
{
  unsigned long work[2];

  Des_F_Scrunch(inblock, work);
  Des_F_Func(work, KnL);
  Des_F_Unscrun(work, outblock);
}
#endif

void Des_F_Scrunch(unsigned char *outof, unsigned long *into)  
{
  *into   = (*outof++ & 0xffL) << 24;
  *into  |= (*outof++ & 0xffL) << 16;
  *into  |= (*outof++ & 0xffL) << 8;
  *into++ |= (*outof++ & 0xffL);
  *into   = (*outof++ & 0xffL) << 24;
  *into  |= (*outof++ & 0xffL) << 16;
  *into  |= (*outof++ & 0xffL) << 8;
  *into  |= (*outof   & 0xffL);
}

void Des_F_Unscrun(unsigned long *outof, unsigned char *into)  
{
  *into++ = (u8)((*outof >> 24) & 0xffL);
  *into++ = (u8)((*outof >> 16) & 0xffL);
  *into++ = (u8)((*outof >>  8) & 0xffL);
  *into++ = (u8)( *outof++      & 0xffL);
  *into++ = (u8)((*outof >> 24) & 0xffL);
  *into++ = (u8)((*outof >> 16) & 0xffL);
  *into++ = (u8)((*outof >>  8) & 0xffL);
  *into   = (u8)(*outof     & 0xffL);
}
void Des_F_Func(unsigned long *block, unsigned long *keys)  
{
  unsigned long fval, work, right, leftt;
  u8 round;

  leftt = block[0];
  right = block[1];
  work = ((leftt >> 4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  leftt ^= (work << 4);
  work = ((leftt >> 16) ^ right) & 0x0000ffffL;
  right ^= work;
  leftt ^= (work << 16);
  work = ((right >> 2) ^ leftt) & 0x33333333L;
  leftt ^= work;
  right ^= (work << 2);
  work = ((right >> 8) ^ leftt) & 0x00ff00ffL;
  leftt ^= work;
  right ^= (work << 8);
  right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = ((leftt << 1) | ((leftt >> 31) & 1L)) & 0xffffffffL;

  for( round = 0; round < 8; round++ ) {
    work  = (right << 28) | (right >> 4);
    work ^= *keys++;
    fval  = SP7[ work             & 0x3fL];
    fval |= SP5[(work >>  8) & 0x3fL];
    fval |= SP3[(work >> 16) & 0x3fL];
    fval |= SP1[(work >> 24) & 0x3fL];
    work  = right ^ *keys++;
    fval |= SP8[ work             & 0x3fL];
    fval |= SP6[(work >>  8) & 0x3fL];
    fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    leftt ^= fval;
    work  = (leftt << 28) | (leftt >> 4);
    work ^= *keys++;
    fval  = SP7[ work             & 0x3fL];
    fval |= SP5[(work >>  8) & 0x3fL];
    fval |= SP3[(work >> 16) & 0x3fL];
    fval |= SP1[(work >> 24) & 0x3fL];
    work  = leftt ^ *keys++;
    fval |= SP8[ work             & 0x3fL];
    fval |= SP6[(work >>  8) & 0x3fL];
    fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    right ^= fval;
  }

  right = (right << 31) | (right >> 1);
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = (leftt << 31) | (leftt >> 1);
  work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
  right ^= work;
  leftt ^= (work << 8);
  work = ((leftt >> 2) ^ right) & 0x33333333L;
  right ^= work;
  leftt ^= (work << 2);
  work = ((right >> 16) ^ leftt) & 0x0000ffffL;
  leftt ^= work;
  right ^= (work << 16);
  work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
  leftt ^= work;
  right ^= (work << 4);
  *block++ = right;
  *block = leftt;
}

/********************  Validation sets: ******************************
 *
 * Single-length key, single-length plaintext -
 * Key    : 0123 4567 89ab cdef
 * Plain  : 0123 4567 89ab cde7
 * Cipher : c957 4425 6a5e d31d
 *
**********************************************************************/

void Des_Key(DES_CTX *dc, unsigned char *key)
{
  Des_F_DesKey(key,EN0);
  Des_F_CpKey(dc->ek);
  Des_F_DesKey(key,DE1);
  Des_F_CpKey(dc->dk);
}

// Encrypt several blocks in ECB mode.  Caller is responsible for  short blocks. 
void Des_Enc(DES_CTX *dc, unsigned char *Data, u16 blocks)
{
	unsigned long work[2];
	unsigned char *cp;
	
	cp = Data;
	while(blocks--)
	{
		Des_F_Scrunch(cp,work);
		Des_F_Func(work,dc->ek);
		Des_F_Unscrun(work,cp);
		cp+=8;
	}
}

void Des_Dec(DES_CTX *dc, unsigned char *Data, u16 blocks)
{
  unsigned long work[2];
  unsigned char *cp;

  cp = Data;
  while(blocks--)
  {
    Des_F_Scrunch(cp,work);
    Des_F_Func(work,dc->dk);
    Des_F_Unscrun(work,cp);
    cp+=8;
  }
}

void Des_Enc3(DES_CTX *dc1, DES_CTX *dc2, unsigned char *Data, unsigned char blocks)
{
	unsigned long work[2];
	unsigned char *cp;

	cp = Data;
	while(blocks--)
	{
		Des_F_Scrunch(cp, work);
		Des_F_Func(work, dc1->ek);
		Des_F_Func(work, dc2->dk);
		Des_F_Func(work, dc1->ek);
		Des_F_Unscrun(work, cp);
		cp += 8;
	}
}

void Des_Dec3(DES_CTX *dc1, DES_CTX *dc2, unsigned char *Data, unsigned char blocks)
{
	unsigned long work[2];
	unsigned char *cp;

	cp = Data;
	while(blocks--)
	{
		Des_F_Scrunch(cp, work);
		Des_F_Func(work, dc1->dk);
		Des_F_Func(work, dc2->ek);
		Des_F_Func(work, dc1->dk);
		Des_F_Unscrun(work, cp);
		cp += 8;
	}
}


//==========================================================================================


void Des_Mac (u8 * Data, s16 DataLen, u8 * Mac,u8 * MacCodeKey )
{
	#define MAX_MACCODE_LEN	  8			// MAC 码的最大个数

	s16 		i, n, di,Index;
	DES_CTX 	dc;

	memset(Mac,0,MAX_MACCODE_LEN);
	
	if ( DataLen <= 0 )		return;

	di = (DataLen % 8 == 0)? DataLen/8 : (DataLen / 8 + 1);
	Des_Key(&dc,(u8*)MacCodeKey);

	for ( n = 0; n < di ;n++ )
	{
		for ( i = 0; i < MAX_MACCODE_LEN ;i++ )
		{
			Index=n*MAX_MACCODE_LEN + i;
			if(Index<DataLen)
				Mac[i] ^= Data[Index];
			else
				Mac[i] ^= 0;		//数据不为8的倍数补0
		}
		Des_Enc(&dc,(unsigned char*)Mac,1);
	}

}
//============2006/08/28  zhou added====================
void Des_B2Dec(u8 *s,u8 *passkey,u8 Dm)
{
	DES_CTX ec;
	DES_CTX dc;

	Des_Key(&ec,(unsigned char*)passkey);
	Des_Key(&dc,(unsigned char*)passkey+8);			
	Des_Dec3(&ec,&dc,s,Dm);	
}
void Des_B2Enc(u8 *s,u8 *passkey,u8 Dm)
{
	DES_CTX ec;
	DES_CTX dc;

	Des_Key(&ec,(unsigned char*)passkey);
	Des_Key(&dc,(unsigned char*)passkey+8);			
	Des_Enc3(&ec,&dc,s,Dm);	
}
//============end zhou===============================

void Des_B1Dec(u8 *s,u8 *passkey,u8 Dm)	// 1 des解密
{
	DES_CTX dc;
	Des_Key(&dc, passkey);
	Des_Dec(&dc, s, Dm);
}
void Des_B1Enc(u8 *s,u8 *passkey,u8 Dm)	 // 1des加密
{
	DES_CTX dc;

	Des_Key(&dc, passkey);
	Des_Enc(&dc, s, Dm);
}

//============2006/08/28  zhou added====================
void Des_B3Dec(u8 *s,u8 *passkey,u8 Dm)
{
	// 3 des解密(分部计算，节约内存空间)
	Des_B1Dec(s,passkey+16,Dm);	
	Des_B1Enc(s,passkey+8,Dm);
	Des_B1Dec(s,passkey,Dm);	
/*
	unsigned long work[2];
	unsigned char i;
	unsigned char *cp;
	DES_CTX dc1,dc2,dc3;
	Des_Key(&dc1, passkey+16);
	Des_Key(&dc2, passkey+8);
	Des_Key(&dc3, passkey);
	cp = s;
	for(i=0;i<Dm;i++)
	{
		Des_F_Scrunch(cp, work);
		Des_F_Func(work, dc1.dk);
		Des_F_Func(work, dc2.ek);
		Des_F_Func(work, dc3.dk);
		Des_F_Unscrun(work, cp);
		cp += 8;
	}
	*/
}

void Des_B3Enc(u8 *s,u8 *passkey,u8 Dm)
{
	// 3 des解密(分部计算，节约内存空间)
		Des_B1Enc(s,passkey,Dm);
		Des_B1Dec(s,passkey+8,Dm);	
		Des_B1Enc(s,passkey+16,Dm);
/*
	unsigned long work[2];
	unsigned char i;
	unsigned char *cp;
	DES_CTX dc1,dc2,dc3;
	Des_Key(&dc1, passkey);
	Des_Key(&dc2, passkey+8);
	Des_Key(&dc3, passkey+16);
	cp = s;
	for(i=0;i<Dm;i++)
	{
		Des_F_Scrunch(cp, work);
		Des_F_Func(work, dc1.ek);
		Des_F_Func(work, dc2.dk);
		Des_F_Func(work, dc3.ek);
		Des_F_Unscrun(work, cp);
		cp += 8;
	}
	*/
}



