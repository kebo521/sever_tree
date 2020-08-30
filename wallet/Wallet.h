#ifndef _WALLET_H
#define _WALLET_H

#define WALLET_TEST_DATA	"\xca\xd5\xb0\x41\xcb\xaf\x38\xc8\x48\x21\xb7\x3a\xd5\x86\x86\x88\xfd\x3b\xdb\xf5\x2d\xfb\x90\x70\xb0\x5c\x46\xdd\x20\x0a\x02\x20\x27\x45\x8b\xe1\x6b\x53\xd2\x74\x38\x1d\x27\x04\x40\xdf\xd1\xab\xdf\x95\xd7\x63\xbc\x4b\x21\x51\x9d\x50\x22\x3d\xa3\x6d\x85\x60"

#ifdef WALLET_TEST_DATA
extern DfHeadInfo *pRecvWallet;
extern DfHeadInfo *pPayWallet;
extern uWalletFD* gCreateWallet(u8* pWalletID,char* pWalletName,u8* pPWCheck);
extern DfHeadInfo* GetCurrWallet(void);
extern void SetCurrWallet(DfHeadInfo* p);
extern int Test_CreateDc(DfDC_bt* pDc,u8* pOwnerID,u32 amont);
extern void Test_CreateWallet(u8* pWalletID,char *pWalletName,char* pNameCert,u32 amont);
extern int Test_CreatePayment(uBalVoucher* pBal,u32 amont,SM2_PUBKEY *pPuKey,u8 *pOutPay);
#endif


typedef struct
{
	u8 				keyflag;	//密钥标志符（1字节）
	u8 				CreditorInsPartyID[8];	//
	u8 				CreditorWalletID[8];	//
	u8 				amount[6];	//交易金额（6字节）
	u8 				RandB[8];	//APP随机数
	u8 				TimeStamp[6];	//APP随机数
}DfOnLineRecvInit;		//钱包初始化 0x50:

typedef struct
{
	u8 				ATC[4];	//密钥标志符（1字节）
	u8 				keyVersion;	//DATA中第1字节指定的在线交易密钥
	u8 				AlgoFlag;	//DATA中第1字节指定的算法标志 
	u8 				InsPartyID[14];	//
	u8 				WalletID[8];	//
	u8 				WallAmount[6];	//DCEP币串的可用余额
	u8 				LastTradeIndex[6];	//上次联机交易索引
	u8 				Random[4];	//APP随机数
	u8 				MAC1[4];	
	u8 				Certification[];	
}DfOnLineRecvRet;		//钱包初始化 0x50:

//======================离线支付==========================
typedef struct
{
	u8					amount[6];		//交易金额（6字节）
	u8					TimeStamp[6];	//时间戳 （6字节）
}DfPaymentInit; 	

typedef struct
{
	u8					Random[4];		//付款方随机数（4字节）
	u8					CertChainB[];	//付款方证书连 (运营机构交易证书 + 硬件钱包交易证书)
}DfPaymentResI;
extern void gkeyAndCheck(char *pPassWord,u8 *pOutChek,u8* pOutKey);

extern void WalletData_Init(void);

extern void WalletReadData(uWalletFD* pFd);

extern int WalletDataParse(u8 *pIn,int inlen,u8 *pOut);
extern int WalletDataParseApdu(DC_APDU *pAPDU,u8 *pOut);

extern void HardWalletPross(void);

#endif
