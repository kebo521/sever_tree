#include "ks_type.h"

#include "gMem.h"
#include "app_sdk.h"

ALG_STATUS sm2_verify(u8 *pubkey,u8 *e,u8 *signature)
{
	return SUCCESS_ALG;
}

ALG_STATUS sm4_ecb_encrypt_decrypt(ALG_EN_DE_MODE en_de_mode,u8 *key,u8 *src,u8 *dest,u32 datalen)
{
	return SUCCESS_ALG;
}

void api_md_hash(HASH_MODE hashType,UINT8 *input, int ilen,UINT8 *output)
{
	
}

ALG_STATUS sm2_generate_keypair(UINT8  * pubkey,UINT8  * prvkey)
{
	static int indexKey=0;
	SM2_PRIVKEY pKeybuff[3]={
		{
			{0xE0,0x04,0x82,0xAA,0x76,0xB1,0xCF,0x35,0x59,0x69,0x1B,0xFA,0x04,0x75,0x9A,0x63,0xF7,0x35,0x87,0x80,0xA8,0x6F,0x8D,0x3D,0x73,0xF6,0xF0,0x52,0x54,0xBD,0xCA,0x97},
			{0xA9,0x27,0x13,0x19,0x6C,0xEA,0x0B,0xE8,0x61,0x06,0xCD,0xF0,0x96,0x4B,0x1D,0x40,0xC2,0xF7,0xB9,0x7F,0x4A,0x1F,0xE7,0x44,0x10,0x62,0x88,0x49,0xF5,0x2C,0x86,0x24},
			{0x7C,0xFE,0x09,0xCB,0x9E,0x8A,0xE9,0xFD,0xE8,0x0C,0xBA,0x95,0xAB,0x8F,0x57,0xE1,0x8C,0xD5,0x87,0x67,0x16,0x95,0xB1,0x24,0x3A,0xA0,0x67,0xF0,0x9B,0xC3,0x42,0x7D}
		},
		{
			{0xDF,0xE3,0x5A,0x89,0xE8,0x7B,0x61,0xCB,0x60,0x28,0x9E,0xBB,0x4D,0xE3,0x66,0xA3,0x4E,0x3F,0x46,0xAD,0xDF,0x87,0xD2,0xD2,0xBC,0xCE,0x29,0x14,0xCA,0x46,0xD3,0x80},
			{0x65,0x78,0x61,0x9E,0x40,0x9F,0xA5,0x6D,0x9E,0x3B,0x6D,0x6A,0x19,0x2B,0x8B,0x50,0x53,0x7A,0xD5,0xF6,0x13,0xE2,0xDA,0xBD,0x83,0x91,0x81,0x66,0xC9,0x9B,0xFC,0x90},
			{0x98,0xC1,0x4B,0xB5,0x90,0x67,0xC3,0xC8,0xAE,0x39,0xC4,0xE2,0x58,0x8F,0xBA,0x38,0x44,0x4A,0xF0,0xE3,0x02,0xC4,0xB2,0x9B,0x9C,0x5C,0x8A,0x81,0x1F,0xE4,0x52,0xD0}
		},
		{
			{0x35,0x1C,0x7D,0x86,0xBA,0xE6,0xFB,0x06,0x00,0xA8,0x9E,0x95,0x16,0xEA,0xB1,0x6A,0xF2,0x65,0xB1,0x42,0xF9,0xDD,0xDF,0xCD,0x26,0xD2,0x54,0x35,0xF7,0xBB,0x9E,0x5D},
			{0xF1,0xAC,0x10,0x55,0x6B,0x9C,0x5A,0x0F,0xD6,0xA7,0x5B,0xA4,0x5E,0x10,0x6E,0x25,0x65,0xAC,0xF3,0x61,0xA2,0x17,0xCA,0x8D,0x27,0x0C,0xD0,0x87,0x3A,0xAC,0xB2,0xF1},
			{0x06,0xE9,0x63,0xD1,0x27,0x1D,0x3A,0xB0,0x50,0xF7,0x3B,0x44,0x0E,0x6F,0x94,0x94,0x03,0x95,0x63,0xC9,0x23,0x49,0x17,0x06,0xBF,0xE3,0x0E,0xB5,0xA2,0xF7,0x97,0x9D}
		}
	};
	if((pubkey == NULL)||(prvkey == NULL))
	{
		return ERROR_DATA_BUF;
	}
	memcpy(pubkey,pKeybuff[indexKey].xQ,64);
	memcpy(prvkey,pKeybuff[indexKey].d,32);	
	indexKey++;
	if(indexKey > 2) indexKey=0;
	return SUCCESS_ALG;
}

ALG_STATUS sm2_encrypt(UINT8 *pubkey,UINT8 *Message,UINT32 MessageLen,UINT8 *Crypto,UINT32 *CryptoLen)
{
	if((MessageLen == 0)||(MessageLen > 0x400))
	{
		return ERROR_DATA_LENGTH;
	}
	if((pubkey == NULL)||(Message == NULL)||(Crypto == NULL))
	{
		return ERROR_DATA_BUF;
	}

	
	
	*CryptoLen = MessageLen+96;
	return SUCCESS_ALG;
}

ALG_STATUS sm2_decrypt(UINT8 *prvkey,UINT8 *Crypto,UINT32 CryptoLen,UINT8 *Message,UINT32 *MessageLen)
{
	if((CryptoLen > 0x460)||(CryptoLen < 0x60))
	{
		return ERROR_DATA_LENGTH;
	}
	if((prvkey == NULL)||(Crypto == NULL)||(Message == NULL))
	{
		return ERROR_DATA_BUF;
	}


	*MessageLen = CryptoLen-0X60;
	return SUCCESS_ALG;
}


//================================================================================
void api_rand(u32 *pRng,int num)
{
	while(num--)
		*pRng++ = rand();
}

int api_sm_sign2(u8 *prvkey,u8 *pIn,int len,u8* pOutSign)
{
	/*
	api_md_hash(MODE_SM3,pIn,len,pOutSign);
	if(SUCCESS_ALG == sm2_sign(prvkey,pOutSign,pOutSign))
		return 0;
		*/
	return 1;
}

int GetSessionKey_ATC(u8 *key,u8 *pATC,u8* pOutKey)
{
	u8 buffATC[16],nBuff[16];
	u16 i,max;
	memset(buffATC,0x00,sizeof(buffATC));
	memcpy(buffATC,pATC,4);
	buffATC[4] = 0x80;
	max = sizeof(buffATC);
	for(i=0;i<max;i++)
	{
		nBuff[i]=~buffATC[i];
	}
	//---------文档两部分连接为32字节与运算不符--------------
	memcpy(buffATC+8,nBuff,8);	//修正处理
	/*
	if(SUCCESS_ALG == sm4_ecb_encrypt_decrypt(ALG_ENCRYPT,key,buffATC,pOutKey,16))
		return 0;
		*/
	return -1;
}

int GetMAC(u8 *Mackey,u8 *pIndata,int inlen,u8* pOutMac)
{
	u8 outbuff[16]={0};//,iv[16]={0};
	//DfGetBeiSu(inlen,16)
	/*
	sm4_cbc_encrypt_decrypt(ALG_ENCRYPT,Mackey,iv,pIndata,outbuff,inlen);
	*/
	memcpy(pOutMac,outbuff,4);
	return 0;
}

int GetMAC_ATC(u8 *key,u8 *pATC,u8 *pIndata,int inlen,u8* pOutMac)
{
	u8 outbuff[16];//,iv[16]={0};
	GetSessionKey_ATC(key,pATC,outbuff);
	//add 80 00 00 00 00 00 00 00 00 00 00 ....
	pIndata[inlen++] = 0x80; 
	memset(pIndata+inlen,0x00,16);
	/*
	sm4_cbc_encrypt_decrypt(ALG_ENCRYPT,outbuff,iv,pIndata,outbuff,DfGetBeiSu(inlen,16));
	*/
	memcpy(pOutMac,outbuff,4);
	return 0;
}

//========从pbuf1中查找pbuf2字段=返回匹配pbuf1中的末端地址=======
u8 *eMemseek(u8* pbuf1,int len1,u8* pbuf2,int len2)
{
	u8 *p1,*p2,*pEnd;
	int i2=0;
	p1=pbuf1;
	p2=pbuf2;
	pEnd = p1+len1;
	while(p1 < pEnd)
	{
		if(*p1 != *p2)
		{
			if(p2 != pbuf2)
			{
				p2=pbuf2;
				p1=pbuf1;
			}
			else p1++;
			i2=0;
		}
		else
		{
			p1++; 
			if(p2 == pbuf2) //记录串1起点(src1+1)
				pbuf1=p1;
			p2++;
			if(++i2 >= len2)
				return p1;
		}
	}
	return NULL;
}



u32 Conv_Hex4ToNum(u8* pbuf,u8 Inlen)
{
	u32 num=0;
	u8 i;
	for(i=0;i<Inlen;i++)
	{
		num = num*0x100 + pbuf[i];
	}
	return num;
}

u64 Conv_Hex8ToNum(u8* pbuf,u8 Inlen)
{
	u64 num=0;
	u8 i;
	for(i=0;i<Inlen;i++)
	{
		num = num*0x100 + pbuf[i];
	}
	return num;
}


void Conv_NumToHex(u32 num,u8* pbuf,int size)
{
	while(size--)
	{
		pbuf[size]=num&0xff;
		num /= 0x100;
	}
}


void Conv_NumToDecimal(u32 num,u8* pbuf,int size)
{
	while(size--)
	{
		pbuf[size]=((num%10)<<4)|((num/10)%10);
		num /= 100;
	}
}

u32 Conv_DecimalToNum(u8* pbuf,int size)
{
	int i;
	u32 num=0;
	for(i=0;i<size;i++)
	{
		num = num*100 + (pbuf[i]>>4)*10 + (pbuf[i]&0x0f);
	}
	return num;
}



void Conv_BcdToStr(u8* bcd,s16 blen,char* str)
{
	u16		i=0;
	u16		j=0;
	const char BcdToAsicCode[16]={"0123456789ABCDEF"};
	for(i=0;i<blen;i++)
	{
		str[j++]=BcdToAsicCode[bcd[i]/16];	//取高4位
		str[j++]=BcdToAsicCode[bcd[i]&0x0f]; //取低4位
	}
	str[j++]=0;
}


void Conv_buffXor(u8* destination,u8* source1,u8* source2,int len)
{
	while(len--)
	{
		*destination = *source1 ^ *source2;
		destination++;
		source1++;
		source2++;
	}
}



//==============pSendBuf 前面预留4个字节空间[55 02 Len[2]]===========================
//====按照【.02 Len[2] Data[Len] crc 03】结构发送,传入Data与Len===================
int	API_Uart_PackSend(u8* pSendBuf,u32 sendLen)
{
	{
		u8 crc=0;
		u16 i;
		*(--pSendBuf) = sendLen&0xff;
		*(--pSendBuf) = sendLen/256;
		sendLen += 2;
		for(i=0;i<sendLen;i++)
			crc ^= pSendBuf[i];
		pSendBuf[i++]=crc;
		pSendBuf[i++]=0x03;
		*(--pSendBuf) = 0x02;
		*(--pSendBuf) = 0x55;
		sendLen += 4;
	}
	//sci_send_data(pSendBuf,sendLen);
	return (sendLen-4);
}

//==============按照【.02 Len[2] Data[Len] crc 03】结构接收，返回Data数据================
u8*	API_Uart_PackRecv(u8* pRecvBuf,u32 *recvLen)
{
	int recvSize,len,offset;
	u8 *p;
	u8 crc;
	recvSize = *recvLen;
	if(recvSize < 5) return NULL;
	memset(pRecvBuf,0x00,5);
	offset = 0;
	len = 0;
	while(1)
	{	
		if(len >= offset)
		{
			//sci_receive_data(pRecvBuf,5);
			len = 0;
			offset = 5;
		}
		if(pRecvBuf[len] == 0x02)
		{
			if(len > 0)
			{
				offset=5-len;
				memcpy(pRecvBuf,pRecvBuf+len,offset);				
				//sci_receive_data(pRecvBuf+offset,5-offset);
				offset = 5;
			}	
			break;
		}
		len++;
	}
	len=pRecvBuf[1]*256 + pRecvBuf[2];
	if(recvSize < (offset+len))
		return NULL;
	//sci_receive_data(pRecvBuf+offset,len);
	p = pRecvBuf+1;
	crc = *p++;
	crc ^= *p++;
	for(offset=0;offset<len;offset++)
		crc ^= *p++;
//	if(crc == *p)
	{
		*recvLen = len;
		return pRecvBuf+3;
	}	
	//LOG(LOG_ERROR,"###ERR recv crc[%x] != [%x],ulen[%d] ##\r\n",*p,crc,len);
//	return NULL;
}



//====================================================================
//功能:    保存参数到文件系统
//输入数据:pfilePath(文件名),pBuff(参数缓冲区),Inlen(文件长度)
//输出数据:写入结果 0 为成功
//作者:     邓国祖	---	
//创作时间:  20150916
//---------------------------------------------------------------
int APP_FileSaveBuff(const char * pfilePath,u32* pBuff,int len)
{
	gFILE*	fb;
	int 	ret;
	fb = API_fopen(pfilePath,F_WRONLY|F_CREAT);
	if(fb == NULL)
	{
		TRACE("APP FileSaveBuff Open[%s] fail\r\n",pfilePath);
		return -1;
	}
	ret=API_fwrite(fb,pBuff,len);
	API_fclose(fb);
	if(len != ret)
	{
		TRACE("APP FileSaveBuff WriteFile[%s] Err ret[%d != %d]\r\n",pfilePath,len,ret);
		return -2;
	}
	return 0;
}

//====================================================================
//功能:    从文件系统读参数
//输入数据:pfilePath(文件名),pBuff(参数缓冲区),pOutlen(输入文件长度)
//输出数据:读取结果 0 为成功
//作者:     邓国祖	---	
//创作时间:  20150916
//---------------------------------------------------------------
int APP_FileReadBuff(const char * pfilePath,u32* pBuff,int size)
{
	gFILE*	fd;
	int 	ret=0;
	fd = API_fopen(pfilePath,F_RDONLY);
	if(fd == NULL)
	{
		TRACE("APP FileReadBuff Open[%s]fail, \r\n",pfilePath);
		return -1;
	}
	if(pBuff)
	{
		ret=API_fread(fd, pBuff, size);
		if(size != ret)
		{
			TRACE("APP FileReadBuff Read[%s] Err nReadSize[%d,%d]\r\n",pfilePath,ret,size);
			ret = -1;
		}
		else ret = 0;
	}
	else 
	{
		ret = gfs_GetSize(fd);
	}
	API_fclose(fd);
	return ret;
}


int APP_FileReadCont(const char * pfilePath,u32* pBuff,u32 num)
{
	gFILE*	fd;
	fd = API_fopen(pfilePath,F_RDONLY|F_CONT,num,2);
	if(fd)
	{
		int ret=API_fread(fd,pBuff,num); // 0 表示最新存进去的数据
		API_fclose(fd);
		return ret;
	}
	return -1;
}

int APP_FileSaveCont(const char * pfilePath,u32* pBuff,u32 num)
{
	gFILE*	fd;
	fd = API_fopen(pfilePath,F_WRONLY|F_CREAT|F_CONT,num,2);
	if(fd)
	{
		int ret=API_fwrite(fd, pBuff, num); // 0 表示最新存进去的数据
		API_fclose(fd);
		return ret;
	}
	return -1;
}


