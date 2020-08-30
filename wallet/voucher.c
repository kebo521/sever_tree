//#include "includes.h"
#include "ks_type.h"
#include "gMem.h"
#include "hardinfo.h"
#include "app_sdk.h"
#include "voucher.h"

/*
void PaymentVoucherData(uWalletFD pFD,u8 Amount[9],u8 TransactionIndex[16],u8 CreditorPubKey[64],u8 *pOutPay)
{
	TransSubChain* pSub = (TransSubChain*)pOutPay;
	pSub->aTag = 0x51;
	pSub->aLen = 9;
	memcpy(pSub->Amount,Amount,sizeof(pSub->Amount));
	pSub->tTag = 0x52;
	pSub->tLen = 16;
	memcpy(pSub->TransactionIndex,TransactionIndex,sizeof(pSub->TransactionIndex));
	pSub->cTag = 0x53;
	pSub->cLen = 64;
	memcpy(pSub->CreditorPubKey,CreditorPubKey,sizeof(pSub->CreditorPubKey));
	
	pSub->dTag = 0x54;
	pSub->dLen = 72;
	memcpy(pSub->DebtorSign,OID_SM_SIGN,8);

	
	api_md_hash(MODE_SM3,pOutPay,11+18+66,&pSub->DebtorSign[8]);
	sm2_sign(pFd->pNewKey->d,sign,sign);
}
*/
char* GetWalletName(int index,u8* pWalletID)
{
	static char sName[32];
	if(index == WAL_HEAD)
		sprintf(sName,"%d.wal",Conv_DecimalToNum(pWalletID,8));
	else if(index == WAL_REAL)
		sprintf(sName,"%d.cnt",Conv_DecimalToNum(pWalletID,8));
	else if(index == WAL_CERK)
		sprintf(sName,"%d.cer",Conv_DecimalToNum(pWalletID,8));
	else 
		sprintf(sName,"%d_%d.dc",Conv_DecimalToNum(pWalletID,8),index);
	return sName;
}

int TransSubChainSign(SM2_PRIVKEY *pKey,uBalVoucher* pBal,TransSubChain* pCurrSun)
{
	u8* 		pData;
	unsigned	offset;
	u16 		i,max;
	offset = sizeof(DfDC_bt)+ (sizeof(pBal->tranChain[0].tTransSub.DebtorSign) * pBal->SubChainNun) + sizeof(TransSubChain);
	pData = gMalloc(offset);
	memcpy(pData,&pBal->tDc.dc,sizeof(pBal->tDc.dc));
	offset = sizeof(pBal->tDc.dc);
	max = pBal->SubChainNun;
	for(i=0;i<max;i++)
	{
		memcpy(pData+offset,pBal->tranChain[i].tTransSub.DebtorSign,sizeof(pBal->tranChain[i].tTransSub.DebtorSign));
		offset += sizeof(pBal->tranChain[i].tTransSub.DebtorSign);
	}
	memcpy(pData+offset,pCurrSun->Amount,sizeof(pCurrSun->Amount));
	offset += sizeof(pCurrSun->Amount);
	memcpy(pData+offset,pCurrSun->TransactionIndex,sizeof(pCurrSun->TransactionIndex));
	offset += sizeof(pCurrSun->TransactionIndex);
	memcpy(pData+offset,pCurrSun->CreditorPubKey,sizeof(pCurrSun->CreditorPubKey));
	offset += sizeof(pCurrSun->CreditorPubKey);
	api_sm_sign2(pKey->d,pData,offset,&pCurrSun->DebtorSign[8]);
	memcpy(pCurrSun->DebtorSign,OID_SM_SIGN,8);
	gFree(pData);
	return 0;
	
}

int ReadPaymentVoucher(uBalVoucher* pBal,TransSubChain* pCurrSun,PaymentVoucher* pPayment)
{
	DfTranSubChain* pSubChain;
	u8* 			pData;
	u16 			i,max;
	max = (sizeof(DfTranSubChain) * (pBal->SubChainNun+1));
	pPayment->ChainTag = 0x60;
	if(max > 255)
	{
		pPayment->ChainLen[0] = 0x82;
		pPayment->ChainLen[1] = max/0x100;
		pPayment->ChainLen[2] = max&0xff;
		pData = &pPayment->ChainLen[3];
	}
	else if(max > 127)
	{
		pPayment->ChainLen[0] = 0x81;
		pPayment->ChainLen[1] = max;
		pData = &pPayment->ChainLen[2];
	}
	else
	{
		pPayment->ChainLen[0] = max;
		pData = &pPayment->ChainLen[1];
	}
	pSubChain = (DfTranSubChain*)pData;
	max = pBal->SubChainNun;
	for(i=0;i<max;i++)
	{
		memcpy(&pSubChain[i],&pBal->tranChain[i],sizeof(pBal->tranChain[i]));
	}
	pSubChain[i].tag = 0x73;
	pSubChain[i].Len[0] = 0x81;
	pSubChain[i].Len[1] = sizeof(TransSubChain);
	memcpy(&pSubChain[i].tTransSub,pCurrSun,sizeof(TransSubChain));
	//----------填支出凭证头----------------
	max = (u8*)&pSubChain[i+1] - (u8*)&pPayment->tDc;
	pPayment ->PayTag = 0x63;
	pPayment ->PayLen[0] = 0x82;
	pPayment ->PayLen[1] =max/0x100;
	pPayment ->PayLen[2] =max&0xff;
	return ((u8*)&pSubChain[i+1] - (u8*)pPayment);
}

int InDcVerify(DfDC_bt* pDc)
{
	int offset;
	u8 signbuff[64];
	SM2_PRIVKEY *pKey=NULL;
	if(pDc->tag != 0x72) 
		return -4;
	//---------验证币串--------------
	//pKey	//取人行公钥
	offset = pDc->dc.PbcSign - &pDc->dc.veTag;
	api_md_hash(MODE_SM3,(u8*)&pDc->dc,offset,signbuff);
	if(SUCCESS_ALG != sm2_verify(pKey->xQ,signbuff,pDc->dc.PbcSign))
	{//---------验证人行签名--------------
		return -1;
	}
	//pKey	//营机构公钥
	offset = pDc->dc.InstructingPartySign - &pDc->dc.veTag;
	api_md_hash(MODE_SM3,(u8*)&pDc->dc,offset,signbuff);
	if(SUCCESS_ALG != sm2_verify(pKey->xQ,signbuff,pDc->dc.InstructingPartySign))
	{//---------验证运营机构签名--------------
		return -1;
	}
	return 0;
}


int PaymentVoucherVerify(PaymentVoucher *pPaymet,unsigned PayLen)
{
	if(InDcVerify(&pPaymet->tDc) == 0)
	{
		u8 *pData,*pLen;
		DfTranSubChain* pSubChain;
		SM2_PRIVKEY *pKey;
		unsigned offsta,offset;
		u16 i,max;
		u8 signbuff[64];
		//--------找到子链-------------
		pLen=pPaymet->ChainLen;
		parse_asn1_len(offset,pLen);
		max = offset/sizeof(DfTranSubChain);
		if(max < 1)
		{
			return -2;
		}
		pData = gMalloc(PayLen); 
		memcpy(pData,&pPaymet->tDc.dc,sizeof(pPaymet->tDc.dc));
		offsta = sizeof(pPaymet->tDc.dc);
		pKey = (SM2_PRIVKEY *)pPaymet->tDc.dc.OwnerID;	//取所有者公钥
		pSubChain = (DfTranSubChain*)pLen;
		for(i=0;i<max;i++)
		{
			offset = offsta;
			memcpy(pData+offset,pSubChain->tTransSub.Amount,sizeof(pSubChain->tTransSub.Amount));
			offset += sizeof(pSubChain->tTransSub.Amount);
			memcpy(pData+offset,pSubChain->tTransSub.TransactionIndex,sizeof(pSubChain->tTransSub.TransactionIndex));
			offset += sizeof(pSubChain->tTransSub.TransactionIndex);
			memcpy(pData+offset,pSubChain->tTransSub.CreditorPubKey,sizeof(pSubChain->tTransSub.CreditorPubKey));
			offset += sizeof(pSubChain->tTransSub.CreditorPubKey);
			api_md_hash(MODE_SM3,pData,offset,signbuff);
			if(SUCCESS_ALG != sm2_verify(pKey->xQ,signbuff,pSubChain->tTransSub.DebtorSign+8))
			{//---------验证子链签名--------------
				gFree(pData);
				return -3;
			}
			//----------------下一条用上一条的签名值做数据------------------
			memcpy(pData+offsta,pSubChain->tTransSub.DebtorSign,sizeof(pSubChain->tTransSub.DebtorSign));
			offsta += sizeof(pSubChain->tTransSub.DebtorSign);
			pKey = (SM2_PRIVKEY *)pSubChain->tTransSub.CreditorPubKey;	//取所公钥验证下一条
			//-----------------------------------------------
			pSubChain++;	//下一条子链
		}
		gFree(pData);
		return i;
	}
	return -1;
}

uBalVoucher* PaymentToBalVouche(PaymentVoucher *pPaymet,u32 *pOutLen,u32 *pOutAmount)
{
	DfTranSubChain* pInChain;
	uBalVoucher *pBal;
	u8* pLen;
	u32 Amount1,Amount2;
	u16 i,max;
	//--------------------支出凭证转余额凭证-----------------------------
	pLen=pPaymet->ChainLen;
	parse_asn1_len(i,pLen);
	pInChain = (DfTranSubChain*)pLen;
	max = i/sizeof(DfTranSubChain);
	pBal = gMalloc(sizeof(uBalVoucher) + sizeof(DfTranSubChain)*max);
	memcpy(&pBal->tDc,&pPaymet->tDc,sizeof(pBal->tDc));
	Amount1 = Conv_Hex4ToNum(pPaymet->tDc.dc.Denomination,sizeof(pPaymet->tDc.dc.Denomination));
	pBal->SubChainNun = max;
	for(i=0;i<max;i++)
	{
		Amount2 = Conv_DecimalToNum(pInChain[i].tTransSub.Amount,sizeof(pInChain[i].tTransSub.Amount));
		if(Amount2 > Amount1)	//前串金额不能小于后串金额
			return NULL;
		memcpy(&pBal->tranChain[i],&pInChain[i],sizeof(DfTranSubChain));
		Amount1 = Amount2;
	}
	if(pOutLen) *pOutLen= (u8*)&pInChain[i] - (u8*)pPaymet;
	if(pOutAmount) *pOutAmount = Amount1;
	return pBal;
}
//-----------------------保存凭证-----------------------------------------
int SaveBalVoucher(const char* pName,uBalVoucher *pBal)
{
	int len;
	u32 *pSave=NULL;
	len = sizeof(uBalVoucher)+(pBal->SubChainNun *sizeof(DfTranSubChain));
	if((u32)pBal&0x03)	//检查是否对其
	{
		pSave = (u32 *)gMalloc(len+4);
		memcpy(pSave,pBal,len);
		pBal=(uBalVoucher *)pSave;
	}
	if(len&0x03)
	{
		memset((u8*)pBal + len,0xff,3);
		len += 3;
	}
	len=APP_FileSaveBuff(pName,(u32*)pBal,len);
	if(pSave) gFree(pSave);
	return len;
}

uBalVoucher* ReadBalVoucher(const char* pName)
{
	gFILE*	fd;
	int 	ret,len;
	uBalVoucher *pBal;
	fd = API_fopen(pName,F_RDONLY);
	if(fd == NULL)
	{
		TRACE("Read BalVoucher Open[%s]fail, \r\n",pName);
		return NULL;
	}
	len = gfs_GetSize(fd);
	pBal = (uBalVoucher *)gMalloc(len);
	ret=API_fread(fd,pBal, len);
	API_fclose(fd);
	if(len != ret)
	{
		TRACE("Read BalVoucher  Read[%s] Err len[%d,%d]\r\n",pName,ret,len);
	}
	return pBal;
}



int CheckVoucherAmount(uSystemRealizeInfo* pRealize,u32 Amount)
{
	u16 i,max;
	//-----------检查凭证金额是否满足支付需求--------------------
	max = sizeof(pRealize->VoucherBal)/sizeof(pRealize->VoucherBal[0]);
	for(i=0;i<max;i++)
	{
		if(pRealize->VoucherBal[i] == ~0)
			continue;
		if(pRealize->VoucherBal[i] >= Amount)
			break;
	}
	if(i>=max) return -1;
	return i;
}

int FindVoucherNull(uSystemRealizeInfo* pRealize)
{
	u16 i,max;
	//if(pRealize == NULL)
	//	return -5;
	//-----------检查凭证金额是否满足支付需求--------------------
	max = sizeof(pRealize->VoucherBal)/sizeof(pRealize->VoucherBal[0]);
	for(i=0;i<max;i++)
	{
		if(pRealize->VoucherBal[i] == ~0)
			return i;
	}
	return -1;
}


int SaveTransactionBill(const char* pName,DfTransactionBill *pBill,int blen)
{
	gFILE*	fb;
	int 	ret;
	fb = API_fopen(pName,F_WRONLY|F_CREAT|F_APPEND);
	if(fb == NULL)
	{
		TRACE(" Open[%s] fail\r\n",pName);
		return -1;
	}
	ret=API_fwrite(fb,(u32*)pBill,blen);
	API_fclose(fb);
	if(blen != ret)
	{
		TRACE("WriteFile[%s] Err ret[%d != %d]\r\n",pName,blen,ret);
		return -2;
	}
	return ret;
}

int ReadPubKeyCert(u8* pWalletID,CERT_TYPE type,u8 *pCert,int size)
{
	CertKey *CertAll;
	int ret;
	CertAll = (CertKey *)gMalloc(sizeof(CertKey));
	if(CertAll==NULL) return -1;
	ret=APP_FileReadBuff(GetWalletName(WAL_CERK,pWalletID),(u32*)CertAll,sizeof(CertKey));
	while(ret==0)
	{
		ret = 0;
		if(type == CERT_PBC)	//人行公钥证书
		{
			if(CertAll->PbcLen > sizeof(CertAll->PbcPubKeyCert)) break;
			if(size < CertAll->PbcLen) break;
			memcpy(pCert,CertAll->PbcPubKeyCert,CertAll->PbcLen);
			ret = CertAll->PbcLen;
		}
		else if(type == CERT_INS)// 运营机构证书
		{
			if(CertAll->InsLen > sizeof(CertAll->InstructingPatryPubKeyCert)) break;
			if(size < CertAll->InsLen) break;
			memcpy(pCert,CertAll->InstructingPatryPubKeyCert,CertAll->InsLen);
			ret = CertAll->InsLen;
		}
		else if(type == CERT_PERS)//个人公钥证书
		{
			if(CertAll->PerLen > sizeof(CertAll->PersPubKeyCert)) break;
			if(size < CertAll->PerLen) break;
			memcpy(pCert,CertAll->PersPubKeyCert,CertAll->PerLen);
			ret = CertAll->PerLen;
		}
		else if(type == PKEY_PERS)//初始个人私钥参数
		{
			if(CertAll->KeyLen > sizeof(CertAll->PersPriKeyCert)) break;
			if(size < CertAll->KeyLen) break;
			memcpy(pCert,CertAll->PersPriKeyCert,CertAll->KeyLen);
			ret = CertAll->KeyLen;
		}
		break;
	}
	gFree(CertAll);
	return ret;
}


int SavePubKeyCert(u8* pWalletID,CERT_TYPE type,u8 *pCert,int cLen)
{
	CertKey *CertAll;
	CertAll = (CertKey *)gMalloc(sizeof(CertKey));
	memset(CertAll,0xff,sizeof(CertKey));
	APP_FileReadBuff(GetWalletName(WAL_CERK,pWalletID),(u32*)CertAll,sizeof(CertKey));
	if(type == CERT_PBC)	//人行公钥证书
	{
		memcpy(CertAll->PbcPubKeyCert,pCert,cLen);
		CertAll->PbcLen = cLen;
	}
	else if(type == CERT_INS)// 运营机构证书
	{
		memcpy(CertAll->InstructingPatryPubKeyCert,pCert,cLen);
		CertAll->InsLen = cLen;
	}
	else if(type == CERT_PERS)//个人公钥证书
	{
		memcpy(CertAll->PersPubKeyCert,pCert,cLen);
		CertAll->PerLen = cLen;
	}
	else if(type == PKEY_PERS)//初始个人私钥参数
	{
		memcpy(CertAll->PersPriKeyCert,pCert,cLen);
		CertAll->KeyLen = cLen;
	}
	APP_FileSaveBuff(GetWalletName(WAL_CERK,pWalletID),(u32*)CertAll,sizeof(CertKey));
	gFree(CertAll);
	return cLen;
}


