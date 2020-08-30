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
	u8 				keyflag;	//��Կ��־����1�ֽڣ�
	u8 				CreditorInsPartyID[8];	//
	u8 				CreditorWalletID[8];	//
	u8 				amount[6];	//���׽�6�ֽڣ�
	u8 				RandB[8];	//APP�����
	u8 				TimeStamp[6];	//APP�����
}DfOnLineRecvInit;		//Ǯ����ʼ�� 0x50:

typedef struct
{
	u8 				ATC[4];	//��Կ��־����1�ֽڣ�
	u8 				keyVersion;	//DATA�е�1�ֽ�ָ�������߽�����Կ
	u8 				AlgoFlag;	//DATA�е�1�ֽ�ָ�����㷨��־ 
	u8 				InsPartyID[14];	//
	u8 				WalletID[8];	//
	u8 				WallAmount[6];	//DCEP�Ҵ��Ŀ������
	u8 				LastTradeIndex[6];	//�ϴ�������������
	u8 				Random[4];	//APP�����
	u8 				MAC1[4];	
	u8 				Certification[];	
}DfOnLineRecvRet;		//Ǯ����ʼ�� 0x50:

//======================����֧��==========================
typedef struct
{
	u8					amount[6];		//���׽�6�ֽڣ�
	u8					TimeStamp[6];	//ʱ��� ��6�ֽڣ�
}DfPaymentInit; 	

typedef struct
{
	u8					Random[4];		//����������4�ֽڣ�
	u8					CertChainB[];	//���֤���� (��Ӫ��������֤�� + Ӳ��Ǯ������֤��)
}DfPaymentResI;
extern void gkeyAndCheck(char *pPassWord,u8 *pOutChek,u8* pOutKey);

extern void WalletData_Init(void);

extern void WalletReadData(uWalletFD* pFd);

extern int WalletDataParse(u8 *pIn,int inlen,u8 *pOut);
extern int WalletDataParseApdu(DC_APDU *pAPDU,u8 *pOut);

extern void HardWalletPross(void);

#endif
