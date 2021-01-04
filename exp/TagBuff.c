//====================================================================
/*****************************树形检索服务***内存预授***************************
//功能-------  用于将任意字符串组生成树形连续空间引导表（地址交换），提高检索效率
//使用-------  可用于菜单检索，脚本POS脚本解析器表达式函数引导，检索表可同步到文件系统
//作者-------  邓国祖
//创作时间---20161214
******************************************************************************/
#include "comm.h"

//=============标签引导数量======================
#define LABEL_GUIDE_NUM	31
//----------字义结构----------
typedef struct _DfTagLenValue
{
	struct _DfTagLenValue* pNext;
	DfTLV_TERM tlv;
}DfTagLenValue;
static DfTagLenValue* *pLabelGuide=NULL; 


//===============当前数据空间==================================
DfTLV_TERM* APP_GetCurrent_TLV(u32 Tag)
{
	DfTagLenValue* pTLV;
	if(pLabelGuide==NULL) return NULL;
	pTLV=pLabelGuide[Tag%LABEL_GUIDE_NUM];
	while(pTLV)
	{
		if(pTLV->tlv.Tag == Tag)
		{
			return &pTLV->tlv;
		}
		pTLV=pTLV->pNext;
	}
	return NULL;
}


int APP_SetCurrent_TLV(DfTLV_TERM* pTlv)
{
	DfTagLenValue *pNext,*pLast,*pNew;
	u32 Guide;
	if(pLabelGuide==NULL)
	{	
		pLabelGuide=(DfTagLenValue* *)malloc(LABEL_GUIDE_NUM*sizeof(DfTagLenValue*));
		if(pLabelGuide==NULL) return -1;
		for(Guide=0;Guide<LABEL_GUIDE_NUM;Guide++)
			pLabelGuide[Guide]=NULL;
	}
	Guide=pTlv->Tag%LABEL_GUIDE_NUM;
	pNext=pLabelGuide[Guide];
	pLast=NULL;
	while(pNext)
	{
		if(pNext->tlv.Tag == pTlv->Tag) break;
		pLast=pNext;
		pNext=pNext->pNext;
	}

	if(pNext!=NULL)
	{
		if(pNext->tlv.Len >= pTlv->Len)
		{
			API_memcpy(&pNext->tlv,pTlv,sizeof(DfTLV_TERM)+pTlv->Len);
			return 0;
		}
		//-------------
		if(pLast)
			pLast->pNext=pNext->pNext;
		else pLabelGuide[Guide]=NULL;
		free(pNext);
		while(pLast->pNext) pLast=pLast->pNext;
		
	}
	//--------------New---------------------------------------
	{
		pNew=(DfTagLenValue*)malloc(sizeof(DfTagLenValue) + pTlv->Len);
		if(pNew==NULL) return 2;
		pNew->pNext=NULL;
		API_memcpy(&pNew->tlv,pTlv,sizeof(DfTLV_TERM)+pTlv->Len);
		
		if(pLast)
		{
			pLast->pNext=pNew;
		}
		else
		{
			pLabelGuide[Guide]=pNew;
		}
	}
	return 0;
}



int APP_RemoveCurrent_TLV(u32 Tag)
{
	DfTagLenValue *pNext,*pLast;
	if(pLabelGuide==NULL) return 1;
	pNext=pLabelGuide[Tag%LABEL_GUIDE_NUM];
	pLast=NULL;
	while(pNext)
	{
		if(pNext->tlv.Tag == Tag)
		{
			if(pLast)
			{
				pLast->pNext=pNext->pNext;
			}
			else
			{
				pLabelGuide[Tag%LABEL_GUIDE_NUM]=pNext->pNext;
			}
			free(pNext);
			return 0;
		}
		pLast=pNext;
		pNext=pNext->pNext;
	}
	return 1;
}



int APP_RemoveAll_TLV(void)
{
	DfTagLenValue *pNext,*pTlv;
	int i;
	if(pLabelGuide==NULL) return 1;
	for(i=0;i<LABEL_GUIDE_NUM;i++)
	{
		pNext=pLabelGuide[i];
		pLabelGuide[i]=NULL;
		while(pNext)
		{
			pTlv=pNext;
			pNext=pNext->pNext;
			free(pTlv);
		}
	}
	free(pLabelGuide);
	pLabelGuide=NULL;
	return 0;
}




u32 APP_StrGetNum(char* p,u8 progress,char guide)
{
	u32 len,uNum=0;
	//------计算字符长度---------
	if(guide)
	{
		len=0;
		while(*p++ != guide);
		while(p[len] != guide) len++;
	}
	else len=10;//U32最大存取数据
	//--------执行转换-----------------
	while(len--)
	{
		if(*p>='0' && *p<='9')
		{
			uNum =uNum*progress + *p-'0';
		}
		else if(progress==16)
		{
			if(*p>='A' && *p<='F')
			{
				uNum =uNum*progress + *p-'A'+10;
			}
			else if(*p>='a' && *p<='f')
			{
				uNum =uNum*progress + *p-'a'+10;
			}
			else 
				break;
		}
		else 
			break;
		p++;
	}
	return uNum;
}


char* APP_StrGetStr(char* p,int* pOutLen,char guide)
{
	int len=0;
	//------计算字符长度---------
	while(*p++ != guide);
	while(p[len] != guide) len++;
	//--------执行转换-----------------
	if(pOutLen) *pOutLen=len;
	return p;
}

int APP_Conv_StrToBcd(char* Indata,u8 Inlen,u8* pBcd)
{
	int i,j=0;
	u8 bitH,bitL; 
	for(i=0; i<Inlen; i++)
	{
		if(Indata[i]>='A' && Indata[i]<='F')
			bitH=Indata[i]-'A'+10;
		else if(Indata[i]>='a' && Indata[i]<='f')
			bitH=Indata[i]-'a'+10;
		else if(Indata[i]>='0' && Indata[i]<='9')
			bitH=Indata[i]&0x0f;
		else continue;
		i++;
		if(Indata[i]>='A' && Indata[i]<='F')
			bitL=Indata[i]-'A'+10;
		else if(Indata[i]>='a' && Indata[i]<='f')
			bitL=Indata[i]-'a'+10;
		else if(Indata[i]>='0' && Indata[i]<='9')
			bitL=Indata[i]&0x0f;
		else continue;
		pBcd[j++]=(bitH<<4)|bitL;
	}
	return j;
}


void APP_Conv_BcdToStr(u8* pBcd,int Inlen,char* pOutStr)
{
	const char *Str_digits = "0123456789ABCDEF";
	int i;
	for(i=0; i<Inlen; i++)
	{
		*pOutStr++ =Str_digits[(pBcd[i]>>4)&0x0f];
		*pOutStr++ =Str_digits[pBcd[i]&0x0f];
	}
	*pOutStr++ ='\0';
}

u32 APP_StrToNum(char* p,u8 len,u8 progress)
{
	u32 uNum=0;
	while(len--)
	{
		if(*p>='0' && *p<='9')
		{
			uNum =uNum*progress + *p-'0';
		}
		else if(progress==16)
		{
			if(*p>='A' && *p<='F')
			{
				uNum =uNum*progress + *p-'A'+10;
			}
			else if(*p>='a' && *p<='f')
			{
				uNum =uNum*progress + *p-'a'+10;
			}
			else 
				break;
		}
		else 
			break;
		p++;
	}
	return uNum;
}


char* APP_GetBitBuff(u32 uBitData,u32 StartBit,char* pOutBit)
{
	*pOutBit++='[';
	while(StartBit)
	{
		if(StartBit&uBitData)
			*pOutBit++='1';
		else
			*pOutBit++='0';
		StartBit >>= 1;
	}
	*pOutBit++=']';
	return pOutBit;
}

char* APP_BitXorBuff_New(char* pBitData,u32 BitLen,u32 StartBit,u32 BitType)
{
	//BitType =[24~31]dCycle + [16~23]bitLen + [8~17]bit1 + [0~7]bit2;
	u8 dCycle,bitLen,bit1,bit2,iLen;
	u8 *pBit,*pEnd,*pOut,OutBuff[4096];
	bit2=BitType&0xff;
	BitType >>= 8;
	bit1=BitType&0xff;
	BitType >>= 8;
	bitLen=BitType&0xff;
	BitType >>= 8;
	dCycle=BitType&0xff;
	//TRACE("APP_BitXorBuff_Newd Cycle[%d]bitLen[%d]bit1[%d]bit2[%d]",dCycle,bitLen,bit1,bit2);
	pOut=OutBuff;
	pBit=(u8*)pBitData+StartBit;
	pEnd=(u8*)pBitData+BitLen;
	while((pBit+dCycle) <= pEnd)
	{
		for(iLen=0;iLen<bitLen;iLen++)
		{
			*pOut++ = '0'| (pBit[bit1+iLen] ^ pBit[bit2+iLen]);
		}
		if(bitLen>1) *pOut++=' ';
		pBit += dCycle;
	}
	*pOut++='\0';
	pBit=(u8*)malloc(pOut-OutBuff);
	API_memcpy(pBit,OutBuff,pOut-OutBuff);
	return (char*)pBit;
}



void APP_fileSave(char* pFileName,void *pInBuff,int Inlen)
{
	size_t OutLen;//,SaveLen;
	FILE *f;
	f = fopen(pFileName, "wb"); //(const char*)name
	if (f == NULL)
	{
		TRACE("打开[%s]文件失败",pFileName);
		return ;
	}
	OutLen=fwrite(pInBuff,Inlen, 1, f);
	fclose(f);
	TRACE("fileSave->%s,Inlen[%d],WriteLen[%d]", pFileName,Inlen,OutLen);
}


void* APP_fileRead(char* pFileName,int *pOutLen)
{
	int OutLen,ReadLen;
	FILE *f;
	char *pBuff;
	f = fopen(pFileName, "rb"); //(const char*)name
	if (f == NULL)
	{
		TRACE("打开[%s]文件失败", pFileName);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	OutLen = ftell(f);
	if(OutLen==0) 
	{
		TRACE("In fopen_s->%s,RetLen Err", pFileName);
		return NULL;
	}
	pBuff=(char*)malloc(OutLen);
	fseek(f, 0, SEEK_SET);
	ReadLen=fread(pBuff,OutLen, 1, f);
	fclose(f);
	TRACE("fileRead->%s,fileLen[%d],ReadLen[%d]", pFileName,OutLen,ReadLen);
	
	if(pOutLen)
		*pOutLen=ReadLen;
	return (void*)pBuff;		
}



