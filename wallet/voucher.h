#ifndef VOUCHER_H
#define VOUCHER_H

//==============证书类型定义==================================
typedef enum 
{
	CERT_PBC = 1,	//人行公钥证书
	CERT_INS = 2,	//运营机构证书
	CERT_PERS = 3,	//个人公钥证书
	PKEY_PERS = 4,	//人私钥参数
}CERT_TYPE;

//======国家密码管理局-密码行业标准化技术委员会-密码算法===============
#define OID_SM              "\x2A\x81\x1C\xCF\x55\x01"	//1.2.156.10197.1
#define OID_SM_EN           OID_SM"\x82\x2D"			//n.301
#define OID_SM_SIGN        	OID_SM"\x83\x75"			//n.501

#define parse_pboc_tag(tag,p)	{ tag = *p++; if((tag&0x1f)==0x1f) tag = tag*256 + *p++;}

//#define check_asn1_tag(tag,p)	{ tag = *p++; if((tag&0x1f)==0x1f) tag = tag*256 + *p++;}

#define parse_asn1_len(len,p)	{switch(*p){ \
	case 0x81: p++; len=*p++; break; 	\
	case 0x82: len=p[1]*256 + p[2]; p += 3;break; \
	default: len=*p++; } }

//====================================================================================================
typedef enum 
{
	WAL_HEAD = 0x100000,	//uWalletHead *pWallet;	//钱包引导区
	WAL_REAL = 0x200000,	//uSystemRealizeInfo *pRealizeInfo;	//控制信息
	WAL_CERK = 0x300000,	//SM2_PRIVKEY *pKey + cer
}WALL_SAVE;
extern char* GetWalletName(int index,u8* pWalletID);

extern int TransSubChainSign(SM2_PRIVKEY *pKey,uBalVoucher* pBal,TransSubChain* pCurrSun);
extern int InDcVerify(DfDC_bt* pDc);
extern int ReadPaymentVoucher(uBalVoucher* pBal,TransSubChain* pCurrSun,PaymentVoucher* pPayment);
extern int PaymentVoucherVerify(PaymentVoucher *pPaymet,unsigned PayLen);
extern uBalVoucher* PaymentToBalVouche(PaymentVoucher *pPaymet,u32 *pOutLen,u32 *pOutAmount);
extern int SaveBalVoucher(const char* pName,uBalVoucher *pBal);
extern uBalVoucher* ReadBalVoucher(const char* pName);
extern int CheckVoucherAmount(uSystemRealizeInfo* pRealize,u32 Amount);
extern int FindVoucherNull(uSystemRealizeInfo* pRealize);

extern int SaveTransactionBill(const char* pName,DfTransactionBill *pBill,int blen);

extern int ReadPubKeyCert(u8* pWalletID,CERT_TYPE type,u8 *pCert,int size);
extern int SavePubKeyCert(u8* pWalletID,CERT_TYPE type,u8 *pCert,int cLen);

#endif
