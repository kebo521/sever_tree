/*****************************�ṹ����******************************

******************************************************************************/

#ifndef _HARD_INFO_
#define _HARD_INFO_
//#include "ks_type.h"

//============================ϵͳ������Ϣ=====================
typedef struct
{
	u8 OffTranLimTimes;		//�������߽��ײ������� TAG=0x5F1A
	u8 OffTranLimAmt[4];	//��������֧����� TAG=0x5F1B
	u8 HandLimTimes;		//ת�ִ������� TAG=0x5F1C						****Ӧ���ڱҴ��У��ɱ�����
	u8 PayLimTime;			//�����Ҵ����߽���ʱ�������� TAG=0x5F1D	
	u8 SinglePayLimAmt[4];	//����֧��������� TAG=0x5F1E
	u8 DayPayLimAmt[4];		//���ۼ�֧������޶� TAG=0x5F1F
	u8 MonthPayLimAmt[4];	//���ۼ�֧������޶� TAG=0x5F20
	u8 YearPayLimAmt[6];	//���ۼ�֧������޶� TAG=0x5F21
	u8 WalletEffectiveData[7]; //Ǯ����Ч���� TAG=0x5F22
	u8 LastTranTime[7];		//�ϱʽ������� TAG=0x5F23
	u8 OffTranLimTime[2];	//����������������ʱ�������� TAG=0x5F24
	u8 NoPwdLimAmt[3];		//�����޶� TAG=0x5F25
}SystemControlInfo;	//ϵͳ������Ϣ TAG=0x71

typedef struct
{
	u32 uOffTranLimAmt;		//��������֧�����[4] TAG=0x5F1B
	u32 uSinglePayLimAmt;	//����֧���������[4] TAG=0x5F1E
	u32 uDayPayLimAmt;		//���ۼ�֧������޶�[4] TAG=0x5F1F
	u32 uMonthPayLimAmt;	//���ۼ�֧������޶�[4] TAG=0x5F20
	u32 uYearPayLimAmt;		//���ۼ�֧������޶�[6] TAG=0x5F21
	u32 uNoPwdLimAmt;		//�����޶�[3] TAG=0x5F25
	u16 uOffTranLimTime;	//����������������ʱ��������[2] TAG=0x5F24
	u8 OffTranLimTimes;		//�������߽��ײ������� TAG=0x5F1A
	u8 HandLimTimes;		//ת�ִ������� TAG=0x5F1C						****Ӧ���ڱҴ��У��ɱ�����
	u8 PayLimTime;			//�����Ҵ����߽���ʱ�������� TAG=0x5F1D	
	u8 WalletEffectiveData[7]; //Ǯ����Ч���� TAG=0x5F22
	u8 LastTranTime[7];		//�ϱʽ������� TAG=0x5F23

	u8 NoUser;			//
}uSystemControlInfo;	//ϵͳ������Ϣ TAG=0x71

//============================ϵͳ����ʵ����Ϣ==================
typedef struct
{
	u8 Bal[9];				//Ǯ����� TAG=0x5F30
	u8 AvailableBal[9];		//���ý�� TAG=0x5F31
	u8 InTransmitBal[9];	//��;��� TAG=0x5F32
	u8 FreezeBal[9];		//������ TAG=0x5F33
	u8 OffTranTimes;		//���߽��״��������� TAG=0x5F34
	u8 OffTranAmt[4];		//���߽��׽������� TAG=0x5F35
	u8 DayPayAmt[4];		//���ۼƽ�����֧���������� TAG=0x5F36
	u8 MonthPayAmt[4];		//���ۼƽ�����֧���������� TAG=0x5F37
	u8 YearPayAmt[6];		//���ۼƽ�����֧���������� TAG=0x5F38
	u8 LastUpdateTime[7];	//�ϴθ������� TAG=0x5F39
	u8 ATC[4];				//Ӧ�ý��׼����� TAG=0x5F3A
}SystemRealizeInfo;	//ϵͳ����ʵ����Ϣ

typedef struct
{
	u32 uBal;				//Ǯ�����[9] TAG=0x5F30
	u32 uAvailableBal;		//���ý��[9] TAG=0x5F31
	u32 uInTransmitBal;		//��;���[9] TAG=0x5F32
	u32 uFreezeBal;			//������[9] TAG=0x5F33
	u32 uOffTranAmt;		//���߽��׽�������[4] TAG=0x5F35
	u32 uDayPayAmt;			//���ۼƽ�����֧����������[4] TAG=0x5F36
	u32 uMonthPayAmt;		//���ۼƽ�����֧����������[4] TAG=0x5F37
	u32 uYearPayAmt;		//���ۼƽ�����֧����������[6] TAG=0x5F38
	u32 uATC;				//Ӧ�ý��׼�����[4] TAG=0x5F3A
	u8 OffTranTimes;		//���߽��״��������� TAG=0x5F34
	u8 LastUpdateTime[7];	//�ϴθ�������[YY YY MM DD hh mm ss] TAG=0x5F39	
	//------------------------------
	u8 LastTradeIndex[6];	//�ϴ�������������
	u8 nouse[2];
	u32 VoucherBal[115];	//ƾ֤���[4]
}uSystemRealizeInfo;	//ϵͳ����ʵ����Ϣ

/*
typedef struct
{
	u32 uAddre;		//��ַ��Ϣ
	u32 uLen;		//��Ϣ����
}uEflashAddress;	//ϵͳ����ʵ����Ϣ
*/

//============================Ӳ��Ǯ��==================
typedef struct
{
	u8 WalletID[8];			//Ǯ��ID TAG=0x40
	u8 WalletLevel[4];		//Ǯ���ȼ� TAG=0x5f02	(WL01~WL04��һ��~����)
	u8 WalletType[4];		//Ǯ������ TAG=0x5f03	(WT01,WT02,WT09,WT10:���ˣ��Ӹ��ˣ��Թ����ӶԹ�)
	u8 WalletStatus[4];		//Ǯ��״̬ TAG=0x5f04	(WS01~WS06:������ע��������ͣ�ã������ã�����)
	char WalletName[60];	//Ǯ������ TAG=0x5f05	
	char CustTelePhone[35];	//�ͻ��ֻ� TAG=0x5f06	
	char CustEmail[64];		//�ͻ����� TAG=0x5f07
	u8 FinInstCode[14];		//���ڻ������� TAG=0x5f08
	char CustName[64];		//�ͻ����� TAG=0x5f09
	u8 WalletSetTime[7];	//Ǯ���������� TAG=0x5f0A
	u8 IDType[4];			//֤������ TAG=0x5f0B	(IT01,IT02....:���֤������֤...)
	char IDNo[32];			//֤���� TAG=0x5f0C
}WalletInfo;	//Ӳ��Ǯ�� TAG=0x70


//---------uSystemRealizeInfo 	Realize;	//ϵͳ����ʵ����Ϣ


typedef struct
{
	WalletInfo			Info;		//Ǯ����Ϣ
	uSystemControlInfo	Control;	//ϵͳ����ʵ����Ϣ��ַ uSystemControlInfo
	u32 	dcNum;					//����������
	u8		dcName[10][32];
}uEflashSystem;	//ϵͳ����ʵ����Ϣ

//============================DC�Ҵ�==================
typedef struct
{
	u8 veTag;		//0x41
	u8 veLen;		//2
	u8 Version[2];				//�汾ID TAG=0x41
	u8 seTag;		//0x42
	u8 seLen;		//32
	char SerialNumber[32];		//���ֺ�ID TAG=0x42
	u8 trTag;		//0x43
	u8 trLen;		//32
	char TranId[32];				//���ױ�־ID TAG=0x43
	u8 deTag;		//0x44
	u8 deLen;		//6
	u8 Denomination[6];			//��ֵID TAG=0x44
	u8 inTag;		//0x45
	u8 inLen;		//3
	u8 InstructingPartyID[3];	//��Ӫ������־ID TAG=0x45
	u8 isTag;		//0x46
	u8 isLen;		//6
	u8 IssuerTime[6];			//����ʱ��ID TAG=0x46 (Unixʱ�������ȷ��ms)
	u8 prTag;		//0x47
	u8 prLen;		//70
	u8 PbcReserved[70];			//�������б�����ID TAG=0x47
	u8 psTag;		//0x48
	u8 psLen;		//64
	u8 PbcSign[64];				//��������ǩ��ID TAG=0x48
	u8 owTag;		//0x49
	u8 owLen;		//64
	u8 OwnerID[64];				//�����߱�־ID TAG=0x49	(�����߹�Կ)
	u8 irTag;		//0x4a
	u8 irLen;		//70
	u8 InstructingPartyReserved[70];//��Ӫ����������ID TAG=0x4A	
	u8 ipTag;		//0x4b
	u8 ipLen;		//64
	u8 InstructingPartySign[64];	//��Ӫ����ǩ��ID TAG=0x4B	
}DC_bt;	//DC�Ҵ� TAG=0x72 len=291

typedef struct
{
	u8 tag;			//0x72
	u8 Len[3];		//291 ,82 xx xx
	DC_bt dc;		//DC������  ID TAG=0x60 (169+3)*n
}DfDC_bt;	//DCDC������ TAG=0x60


typedef struct
{
	u8 aTag;		//0x51
	u8 aLen;		//9
	u8 Amount[9];			//���׽��ID TAG=0x51

	u8 tTag;		//0x52
	u8 tLen;		//16
	u8 TransactionIndex[16];//��������ID TAG=0x52 (���ո���˫���������϶���)

	u8 cTag;		//0x53
	u8 cLen;		//64
	u8 CreditorPubKey[64];	//�տ��ԿID TAG=0x53

	u8 dTag;		//0x54
	u8 dLen;		//72
	u8 DebtorSign[72];		//���ǩ��ID TAG=0x54
}TransSubChain;	//DC�������� TAG=0x73 ,len=169




typedef struct
{
	u8 tag;			//0x73
	u8 Len[2];		//169 ,81 A9
	TransSubChain tTransSub;		//DC������  ID TAG=0x60 (169+3)*n
}DfTranSubChain;	//DCDC������ TAG=0x60
//==================ƾ֤��Ϣ����===���ر���===================================
typedef struct
{
	//u32 VoucherBal;		//ƾ֤���[4] TAG=0x5A
	DfDC_bt tDc;			//DC�Ҵ� TAG=0x72
	//u8 SubBifuStart;	//�����ֲ����(==0 dc��������,>0 dcΪ����֧��ƾ֤)��
	u8 SubChainNun;		//��������
	DfTranSubChain tranChain[];	//DCDC������ TAG=0x73
}uBalVoucher;	//DC���ƾ֤(�ļ��洢) TAG=0x61


//==================����֧��ƾ֤===================================
typedef struct
{
	u8 PayTag;			//0x63		
	u8 PayLen[3];		//0x82 xx xx
	DfDC_bt tDc;		//DC�Ҵ� TAG=0x72
	u8 SubChainNun;		//��������
	u8 ChainTag;		//0x60		
	u8 ChainLen[0];		//0x82 169
//	DfTranSubChain tranChain[0];	//DCDC������ TAG=0x60
}PaymentVoucher;	//DC֧��ƾ֤ TAG=0x63



typedef struct
{
	u8 TransactionType[4];			//�������� TAG=0x07
	u8 TransactionBizType[4];		//ҵ�����ͱ��� TAG=0x08
	u8 TransactionCategoryPurpose[5];//ҵ��������� TAG=0x09
	u8 Amount[9];					//���׽�� TAG=0x51
	u8 TransactionTimeStamp[6]; 	//����ʱ��� TAG=0x0A
	u8 SerialNumber[32]; 			//���ֺ� TAG=42
	u8 TransactionIndex[16]; 		//�������� TAG=52	(��¼��Ǯ ʱ���ܲ� ����)
	u8 DebtorWalletID[8];			//����Ǯ��ID TAG=01	
	u8 DebtorName[64]; 				//����Ǯ������ TAG=02	
	u8 CreditorWalletID[8];			//�տǮ��ID TAG=03 
	u8 CreditorName[64];			//�տǮ������ TAG=04 
	u8 Remark[];		//��ע TAG=74  	(tlv �䳤 O �Ǽ�APDU�� ȡ������ ��Ϣ���� �磺���� �š����� ʱ�䡢�� ����š� �̻��� ��)
}TransactionBill;	//DC������ˮ TAG=0x64


typedef struct
{
	u8 tag; 	//DC������ˮ TAG=0x64
	u8 len[2];	
	TransactionBill tBill;
}DfTransactionBill;	//Ǯ��������ʧ



typedef struct
{
	u8 TransactionTimeStamp[6]; 	//����ʱ��� TAG=0x0A
	u8 TransactionBizType[4];		//ҵ�����ͱ��� TAG=0x08
	u8 TransactionStatus;			//����״̬ TAG=0x5E
	u8 Remark[];		//��ע TAG=74  	(tlv �䳤 O �Ǽ�APDU�� ȡ������ ��Ϣ���� �磺���� �š����� ʱ�䡢�� ����š� �̻��� ��)
}OperationLog;	//������־ TAG=0x66



typedef struct
{
	u8 DebtorWalletID[8];			//����Ǯ��ID TAG=01	
	u8 DebtorName[64]; 				//����Ǯ������ TAG=02	
	u8 CreditorWalletID[8]; 		//�տǮ��ID TAG=03 
	u8 CreditorName[64];			//�տǮ������ TAG=04 	
	u8 TransactionType[4];			//�������� TAG=0x07
	u8 TransactionBizType[4];		//ҵ�����ͱ��� TAG=0x08
	u8 TransactionCategoryPurpose[5];//ҵ��������� TAG=0x09
	u8 TransactionTimeStamp[6]; 	//����ʱ��� TAG=0x0A
	u8 MrchntNo[35]; 				//�̻���� TAG=0B
	u8 MrchntName[60];				//�̻����� TAG=0C
	u8 OrderNo[40];					//������ TAG=0D
	u8 DebtorPubKeyCert[100]; 		//�����Կ֤�� TAG=0E
	u8 CreditorPubKeyCert[100];		//�տ��Կ֤�� TAG=0F
	u8 OrderDetails[]; 	//�������� TAG=B0 (������Ʒ���ơ���Ʒ���͡���Ʒ��������Ʒ���۵�)
}ContextData;	//���������ݣ�ContextData��Ϊͨ��APDUָ����д��ݽ���˫������Ϣ


typedef struct
{
	u8 LoginPwd[8];			//��¼���� TAG=C0
	u8 VerifyTimes;			//������֤���� TAG=C1 (ÿ��֤δͨ��һ�Σ������ݼ�1������֤ͨ������0)
}LoginPwdInfo;	//��¼������ϢY TAG=F1         13

typedef struct
{
	u8 TranPwd[8];		//�������� TAG=C2 (����Ǯ���Ľ�������)
	u8 VerifyTimes;		//������֤���� TAG=C1 (ÿ��֤δͨ��һ�Σ������ݼ�1������֤ͨ������0)
}TranPwdInfo;	//����������ϢY TAG=F2         13

typedef struct
{
	u8 DataEncKey[16];		//���ļ�����Կ TAG=CB (���ɻỰ������Կ���Խ��ױ��Ľ��м���)
	u8 MessageAuthKey[16];	//����MAC��Կ TAG=CC (���ɻỰMAC��Կ���Խ��ױ��ļ���У��ֵ)
}AuthKey;	//��֤��ԿY TAG=CA     16 (���ɻỰ��֤��Կ������Ӳ��Ǯ������Ӫ����֮��ĺϷ�����֤)



typedef struct
{
	u16 bitFlag;			//bit0,PersPriKeyCert,bit1=PersPubKeyCert,bit2=PbcPubKeyCert,bit3=InstructingPatryPubKeyCert
	u16	KeyLen;				//����˽Կ����
	u8 PersPriKeyCert[64+32];			//����˽Կ TAG=C9 

	u16	PerLen;				//���˹�Կ֤�鳤��
	u8	PersPubKeyCert[256-2];				//���˹�Կ֤�� TAG=C8 
	
	u16	PbcLen;				//���й�Կ֤�鳤��
	u8	PbcPubKeyCert[256-2];				//�������й�Կ֤�� TAG=C6 
	
	u16	InsLen;				//��Ӫ������Կ֤�鳤��
	u8 InstructingPatryPubKeyCert[256-2];	//��Ӫ������Կ֤�� TAG=C7 (���Դ�Ŷ����Ӫ����֤��)
}CertKey;	//��֤��ԿY TAG=CA     16

typedef struct
{
	u8 PwdVerifyMaxTimes;	//����������֤������ TAG=D0 (��Ӫ����������ã���¼���߽�������������֤δͨ���������������ô���������Ӧ�ñ���ס��)
	u8 SeID[20];			//Ӳ��Ǯ��Ӳ������ID TAG=D1 (������ƶ��û�ʶ����IMSI�������ƶ��û��豸ʶ�����ICCID�ȡ�)
	u8 AID[16];				//Ӧ�ñ�־�� TAG=D2 (�ο�GB/T 16649.5��������Ӧ�ö���)
}OtherKey;	//��֤��ԿY TAG=CA     16

typedef struct
{
	u8 					Enc[16];	//������Կ
	char 				Mac[16];	//Mac��Կ
}DfSessionKey;		//���׻Ự��Կ

typedef struct
{
	uSystemControlInfo	Control;	//������Ϣ
	WalletInfo 			fWallet;	//Ӳ��Ǯ��
}Df_SC_WA;	//Ǯ������

typedef struct
{
	u32 			InvVoucherBal;		//ƾ֤���[4] ����ֵ
	u8	 			InvFlag;			//ƾ֤���ڱ�� ff ������,D ����
	char	 		nUes[3];			//
}Df_DC_NI;	//Ǯ������

typedef struct
{
	WalletInfo			Info;		//Ǯ����Ϣ
	uSystemControlInfo	Control;	//ϵͳ����ʵ����Ϣ��ַ uSystemControlInfo
	//Df_DC_NI			dc[96];
}uWalletHead;		//Ǯ��������




typedef struct
{
	uWalletHead 		*pWallet;		//Ǯ��������
	uSystemRealizeInfo	*pRealizeInfo;	//������Ϣ
	SM2_PRIVKEY			*pKey,*pNewKey;	//Ǯ����˽Կ
	u8					CreditorPubKey[64];	//�տ��Կ
	u8 					WalletID[8];	//Ǯ��ID
	u8 					Enckey[16];	//Ǯ����Կ
	//-----------�����еõ�������--------------
	int					index;
	DfSessionKey		sesKey;			//�Ự��Կ
	u8					TransType[4];	//��������
	u8					TransIndex[16];	//��������
	u32 				RandA[2];		//�տ�������8�ֽڣ�
	u32 				RandB[2];		//����������8�ֽڣ�
	u32 				FactA[4];		//�տ�Ự���� ��16�ֽڣ�
	u32 				FactB[4];		//����Ự���� ��16�ֽڣ�
	u32 				Amount;			//���׽�6�ֽڣ�
	u8 					TimeStamp[6];	//ʱ��� ��6�ֽڣ�
	char 				CreditorInsPartyID[8];	//
	char 				CreditorWalletID[8];	//
}uWalletFD;		//Ǯ��������


typedef struct
{	
	u8 					WalletName[32];	//Ǯ��ID
	u8 					WalletID[8];	//Ǯ��ID
	u8 					PWCheck[4];		//����У��ֵ����Ӧ��Կ����˽Կ
}DfFileHeadInfo;		//������wallet.head �ļ���


typedef struct
{	
	DfFileHeadInfo 		HeadWallet;	//�����ļ�
	uWalletFD 			Wallet;	//Ǯ������������ַ
}DfHeadInfo;			//����������

//=======================================================================
typedef struct
{
	u8 cla;
	u8 ins;
	u8 p1;		
	u8 p2;
	u8 lc[2];
	u8 data[];
	//u8 le;
}DC_APDU;	//DC֧��ƾ֤ TAG=0x63



typedef struct
{
	u8 WalletID[8];			//Ǯ��ID TAG=0x40
	u8 WalletLevel[4];		//Ǯ���ȼ� TAG=0x5f02	(WL01~WL04��һ��~����)
	u8 WalletType[4];		//Ǯ������ TAG=0x5f03	(WT01,WT02,WT09,WT10:���ˣ��Ӹ��ˣ��Թ����ӶԹ�)
	char WalletName[60];		//Ǯ������ TAG=0x5f05	
	char CustTelePhone[35];	//�ͻ��ֻ� TAG=0x5f06	
	char CustEmail[64];		//�ͻ����� TAG=0x5f07
	u8 FinInstCode[14];		//���ڻ������� TAG=0x5f08
	char CustName[64];		//�ͻ����� TAG=0x5f09
	u8 WalletSetTime[7];	//Ǯ���������� TAG=0x5f0A
	u8 IDType[4];			//֤������ TAG=0x5f0B	(IT01,IT02....:���֤������֤...)
	char IDNo[32];			//֤���� TAG=0x5f0C
}uWalletPort;		//Ǯ��������



//========================================================================

typedef struct
{
	u8 					tag;		
	u8 					fd;			
	u8 					data[];		
}DfCommInPack;		

typedef struct
{
	u8 					RetTag;		
	u8 					errcode;			
	u8 					data[];		
}DfCommOutPack;		

//========================================================

typedef struct
{
	u8 					amount[6];	//���׽�6�ֽڣ�
	u8 					rand[4];	//�տ�������4�ֽڣ�
	u8 					PubKey[64];	//�տ�������64�ֽڣ�
}uPaymentApp;		//Ǯ��������


//======================�ѻ��տ�===========================
typedef struct
{
	u8 					amount[6];		//���׽�6�ֽڣ�
	u8 					RandB[8];		//����������4�ֽڣ�
	u8 					TimeStamp[6];	//ʱ��� ��6�ֽڣ�
	u8 					CertChainB[];	//���֤���� (��Ӫ��������֤�� + Ӳ��Ǯ������֤��)
}DfRecviveInit;		

typedef struct
{
	u8 					wallAmount[6];
	u8 					ATC[2];			//Ӳ��Ǯ�����׼�����
	u8 					RandA[8];		//�տ�������4�ֽڣ�
	u8 					CertChainA[];	//�տ֤���� (��Ӫ��������֤�� + Ӳ��Ǯ������֤��)
	//u8 					FactA[16+96];	//�տ�Ự����
	//u8 					SigA[64];		//ǩ��A
}DfRecviveResI;


typedef struct
{
	u8					RandB[8];		//����������4�ֽڣ�
	u8					CertChainB[];	//���֤���� (��Ӫ��������֤�� + Ӳ��Ǯ������֤��)
	//u8					FactB[16];		//�������ɻỰ��Կ������		
	//u8					sign[64];	//ǩ������
}DfPaymentCerI; 	

typedef struct
{
	u8					FactA[16];		//�������ɻỰ��Կ������
	PaymentVoucher		payVou; 		//DCEP�Ҵ��ͽ�������=֧��ƾ֤
	//u8					MAC1[4];		//
}DfPaymentResO;



typedef struct
{
	u8 					FactB[16+96];	//����Ự����
	u8 					PaymentVoucher[];	//֧��ƾ֤(ASN1 �ṹ)
	//u8 					MAC1[4];
}DfRecviveOperation;		

typedef struct
{
	u8 					MAC2[4];		//
}DfRecviveResOpe;		


typedef struct
{
	//u8 					amount[6];		//���׽�ƾ֤�ܼ���
	u8 					Voucher[128];		//�����֧��ƾ֤(ASN1 �ṹ,���ƾ֤��Ӧ���61��63)
}DfRecviveResSyn;		


typedef struct
{
	u8 					DateTime[7];	//��������ʱ��
	u8 					pDc[];			//�Ҵ�
}DfRecviveDcSyn;		



#endif	//_HARD_INFO_
