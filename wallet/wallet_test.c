//#include "includes.h"
#include "ks_type.h"
#include "gMem.h"
#include "hardInfo.h"
#include "app_sdk.h"
#include "voucher.h"
#include "Wallet.h"


u8 Conv_CharToNum(char c)		
{
	if (c>='0' && c<='9')
	{
		return (c-'0');
	}
	else if(c>='a' && c<='f')
	{
		return (c-'a'+10);
	}
	else if(c>='A' && c<='F')
	{
		return (c-'A'+10);
	}
	else return 0;
}

int Conv_StrToBcd_Left(char* str,int slen,u8* bcd)
{
	u8	c;
	int	i,j=0;
	for(i=0;i<slen;i++)
	{
		if(str[i] <= ' ') continue;
		c=(Conv_CharToNum((char)str[i])<<4) | (Conv_CharToNum((char)str[i+1]));
		i++;
		bcd[j++]=c;
	}
	return j;
}


void WalletDataInit(void)
{
	//gfs_Fomat("123456");
	#ifdef WALLET_TEST_DATA
	Test_CreateWallet((u8*)"\x00\x00\x00\x00\x00\x00\x00\x01","证通01收款硬钱包","收款证书",654327);
	pRecvWallet=GetCurrWallet();
	SetCurrWallet(NULL);
	Test_CreateWallet((u8*)"\x00\x00\x00\x00\x00\x00\x00\x02","证通02付款硬钱包","付款证书",80000);
	pPayWallet=GetCurrWallet();
	SetCurrWallet(pRecvWallet);
	#else
	WalletData_Init();
	#endif
}



EXP_UNIT* EXP_WalletInit(EXP_UNIT* pInPar)
{
	WalletDataInit();
	return pInPar;
}

EXP_UNIT* EXP_WalletApdu(EXP_UNIT* pInPar)
{
	u8 *pData;
	int ret;

	if(pInPar->type == DATA_DEFINE_STR)
	{
		pInPar->Len=Conv_StrToBcd_Left(pInPar->pStr,pInPar->Len,pInPar->pBuff);
		pInPar->type = DATA_DEFINE_BUFF;
	}
	TRACE_HEX("WalletIn",pInPar->pStr,pInPar->Len);
	pData=(u8*)malloc(2048);
	ret= WalletDataParseApdu((DC_APDU *)pInPar->pStr,pData);
	TRACE_HEX("WalletOut",pData,ret);
	EXP_FreeUNIT(pInPar->pNext,0); pInPar->pNext=NULL;
	free(pInPar->pStr);
	if(ret >= 0)
	{
		pData[ret]='\0';
		pInPar->type=DATA_DEFINE_STR;
		pInPar->pBuff=pData;
	}
	else
	{
		free(pData);
		pInPar->type=DATA_DEFINE_BOOL;
		pInPar->Result=FALSE;
	}
	return pInPar;
}



