/*****************************结构定义******************************

******************************************************************************/

#ifndef _HARD_INFO_
#define _HARD_INFO_
//#include "ks_type.h"

//============================系统控制信息=====================
typedef struct
{
	u8 OffTranLimTimes;		//连续离线交易参数限制 TAG=0x5F1A
	u8 OffTranLimAmt[4];	//连续离线支付金额 TAG=0x5F1B
	u8 HandLimTimes;		//转手次数限制 TAG=0x5F1C						****应该在币串中，由币限制
	u8 PayLimTime;			//单个币串离线交易时间限制数 TAG=0x5F1D	
	u8 SinglePayLimAmt[4];	//单笔支付金额限制 TAG=0x5F1E
	u8 DayPayLimAmt[4];		//日累计支付金额限额 TAG=0x5F1F
	u8 MonthPayLimAmt[4];	//月累计支付金额限额 TAG=0x5F20
	u8 YearPayLimAmt[6];	//年累计支付金额限额 TAG=0x5F21
	u8 WalletEffectiveData[7]; //钱包生效日期 TAG=0x5F22
	u8 LastTranTime[7];		//上笔交易日期 TAG=0x5F23
	u8 OffTranLimTime[2];	//连续离线连续交易时间限制数 TAG=0x5F24
	u8 NoPwdLimAmt[3];		//免密限额 TAG=0x5F25
}SystemControlInfo;	//系统控制信息 TAG=0x71

typedef struct
{
	u32 uOffTranLimAmt;		//连续离线支付金额[4] TAG=0x5F1B
	u32 uSinglePayLimAmt;	//单笔支付金额限制[4] TAG=0x5F1E
	u32 uDayPayLimAmt;		//日累计支付金额限额[4] TAG=0x5F1F
	u32 uMonthPayLimAmt;	//月累计支付金额限额[4] TAG=0x5F20
	u32 uYearPayLimAmt;		//年累计支付金额限额[6] TAG=0x5F21
	u32 uNoPwdLimAmt;		//免密限额[3] TAG=0x5F25
	u16 uOffTranLimTime;	//连续离线连续交易时间限制数[2] TAG=0x5F24
	u8 OffTranLimTimes;		//连续离线交易参数限制 TAG=0x5F1A
	u8 HandLimTimes;		//转手次数限制 TAG=0x5F1C						****应该在币串中，由币限制
	u8 PayLimTime;			//单个币串离线交易时间限制数 TAG=0x5F1D	
	u8 WalletEffectiveData[7]; //钱包生效日期 TAG=0x5F22
	u8 LastTranTime[7];		//上笔交易日期 TAG=0x5F23

	u8 NoUser;			//
}uSystemControlInfo;	//系统控制信息 TAG=0x71

//============================系统控制实现信息==================
typedef struct
{
	u8 Bal[9];				//钱包金额 TAG=0x5F30
	u8 AvailableBal[9];		//可用金额 TAG=0x5F31
	u8 InTransmitBal[9];	//在途金额 TAG=0x5F32
	u8 FreezeBal[9];		//冻结金额 TAG=0x5F33
	u8 OffTranTimes;		//离线交易次数计数器 TAG=0x5F34
	u8 OffTranAmt[4];		//离线交易金额计数器 TAG=0x5F35
	u8 DayPayAmt[4];		//日累计交易已支付金额计数器 TAG=0x5F36
	u8 MonthPayAmt[4];		//月累计交易已支付金额计数器 TAG=0x5F37
	u8 YearPayAmt[6];		//年累计交易已支付金额计数器 TAG=0x5F38
	u8 LastUpdateTime[7];	//上次更新日期 TAG=0x5F39
	u8 ATC[4];				//应用交易计数器 TAG=0x5F3A
}SystemRealizeInfo;	//系统控制实现信息

typedef struct
{
	u32 uBal;				//钱包金额[9] TAG=0x5F30
	u32 uAvailableBal;		//可用金额[9] TAG=0x5F31
	u32 uInTransmitBal;		//在途金额[9] TAG=0x5F32
	u32 uFreezeBal;			//冻结金额[9] TAG=0x5F33
	u32 uOffTranAmt;		//离线交易金额计数器[4] TAG=0x5F35
	u32 uDayPayAmt;			//日累计交易已支付金额计数器[4] TAG=0x5F36
	u32 uMonthPayAmt;		//月累计交易已支付金额计数器[4] TAG=0x5F37
	u32 uYearPayAmt;		//年累计交易已支付金额计数器[6] TAG=0x5F38
	u32 uATC;				//应用交易计数器[4] TAG=0x5F3A
	u8 OffTranTimes;		//离线交易次数计数器 TAG=0x5F34
	u8 LastUpdateTime[7];	//上次更新日期[YY YY MM DD hh mm ss] TAG=0x5F39	
	//------------------------------
	u8 LastTradeIndex[6];	//上次联机交易索引
	u8 nouse[2];
	u32 VoucherBal[115];	//凭证余额[4]
}uSystemRealizeInfo;	//系统控制实现信息

/*
typedef struct
{
	u32 uAddre;		//地址信息
	u32 uLen;		//信息长度
}uEflashAddress;	//系统控制实现信息
*/

//============================硬件钱包==================
typedef struct
{
	u8 WalletID[8];			//钱包ID TAG=0x40
	u8 WalletLevel[4];		//钱包等级 TAG=0x5f02	(WL01~WL04：一类~四类)
	u8 WalletType[4];		//钱包类型 TAG=0x5f03	(WT01,WT02,WT09,WT10:个人，子个人，对公，子对公)
	u8 WalletStatus[4];		//钱包状态 TAG=0x5f04	(WS01~WS06:正常，注销，挂起，停用，待起用，其它)
	char WalletName[60];	//钱包名称 TAG=0x5f05	
	char CustTelePhone[35];	//客户手机 TAG=0x5f06	
	char CustEmail[64];		//客户邮箱 TAG=0x5f07
	u8 FinInstCode[14];		//金融机构编码 TAG=0x5f08
	char CustName[64];		//客户名称 TAG=0x5f09
	u8 WalletSetTime[7];	//钱包启用日期 TAG=0x5f0A
	u8 IDType[4];			//证件类型 TAG=0x5f0B	(IT01,IT02....:身份证，军官证...)
	char IDNo[32];			//证件号 TAG=0x5f0C
}WalletInfo;	//硬件钱包 TAG=0x70


//---------uSystemRealizeInfo 	Realize;	//系统控制实现信息


typedef struct
{
	WalletInfo			Info;		//钱包信息
	uSystemControlInfo	Control;	//系统控制实现信息地址 uSystemControlInfo
	u32 	dcNum;					//本机币数量
	u8		dcName[10][32];
}uEflashSystem;	//系统控制实现信息

//============================DC币串==================
typedef struct
{
	u8 veTag;		//0x41
	u8 veLen;		//2
	u8 Version[2];				//版本ID TAG=0x41
	u8 seTag;		//0x42
	u8 seLen;		//32
	char SerialNumber[32];		//冠字号ID TAG=0x42
	u8 trTag;		//0x43
	u8 trLen;		//32
	char TranId[32];				//交易标志ID TAG=0x43
	u8 deTag;		//0x44
	u8 deLen;		//6
	u8 Denomination[6];			//面值ID TAG=0x44
	u8 inTag;		//0x45
	u8 inLen;		//3
	u8 InstructingPartyID[3];	//运营机构标志ID TAG=0x45
	u8 isTag;		//0x46
	u8 isLen;		//6
	u8 IssuerTime[6];			//发行时间ID TAG=0x46 (Unix时间戳，精确到ms)
	u8 prTag;		//0x47
	u8 prLen;		//70
	u8 PbcReserved[70];			//人民银行保留域ID TAG=0x47
	u8 psTag;		//0x48
	u8 psLen;		//64
	u8 PbcSign[64];				//人民银行签名ID TAG=0x48
	u8 owTag;		//0x49
	u8 owLen;		//64
	u8 OwnerID[64];				//所有者标志ID TAG=0x49	(所有者公钥)
	u8 irTag;		//0x4a
	u8 irLen;		//70
	u8 InstructingPartyReserved[70];//运营机构保留域ID TAG=0x4A	
	u8 ipTag;		//0x4b
	u8 ipLen;		//64
	u8 InstructingPartySign[64];	//运营机构签名ID TAG=0x4B	
}DC_bt;	//DC币串 TAG=0x72 len=291

typedef struct
{
	u8 tag;			//0x72
	u8 Len[3];		//291 ,82 xx xx
	DC_bt dc;		//DC交易链  ID TAG=0x60 (169+3)*n
}DfDC_bt;	//DCDC交易链 TAG=0x60


typedef struct
{
	u8 aTag;		//0x51
	u8 aLen;		//9
	u8 Amount[9];			//交易金额ID TAG=0x51

	u8 tTag;		//0x52
	u8 tLen;		//16
	u8 TransactionIndex[16];//交易索引ID TAG=0x52 (由收付款双方随机数组合而成)

	u8 cTag;		//0x53
	u8 cLen;		//64
	u8 CreditorPubKey[64];	//收款方公钥ID TAG=0x53

	u8 dTag;		//0x54
	u8 dLen;		//72
	u8 DebtorSign[72];		//付款方签名ID TAG=0x54
}TransSubChain;	//DC交易子链 TAG=0x73 ,len=169




typedef struct
{
	u8 tag;			//0x73
	u8 Len[2];		//169 ,81 A9
	TransSubChain tTransSub;		//DC交易链  ID TAG=0x60 (169+3)*n
}DfTranSubChain;	//DCDC交易链 TAG=0x60
//==================凭证信息集合===本地保存===================================
typedef struct
{
	//u32 VoucherBal;		//凭证余额[4] TAG=0x5A
	DfDC_bt tDc;			//DC币串 TAG=0x72
	//u8 SubBifuStart;	//子链分岔起点(==0 dc本机所有,>0 dc为外来支出凭证)。
	u8 SubChainNun;		//子链数量
	DfTranSubChain tranChain[];	//DCDC交易链 TAG=0x73
}uBalVoucher;	//DC余额凭证(文件存储) TAG=0x61


//==================对外支出凭证===================================
typedef struct
{
	u8 PayTag;			//0x63		
	u8 PayLen[3];		//0x82 xx xx
	DfDC_bt tDc;		//DC币串 TAG=0x72
	u8 SubChainNun;		//子链数量
	u8 ChainTag;		//0x60		
	u8 ChainLen[0];		//0x82 169
//	DfTranSubChain tranChain[0];	//DCDC交易链 TAG=0x60
}PaymentVoucher;	//DC支出凭证 TAG=0x63



typedef struct
{
	u8 TransactionType[4];			//交易类型 TAG=0x07
	u8 TransactionBizType[4];		//业务类型编码 TAG=0x08
	u8 TransactionCategoryPurpose[5];//业务种类编码 TAG=0x09
	u8 Amount[9];					//交易金额 TAG=0x51
	u8 TransactionTimeStamp[6]; 	//交易时间戳 TAG=0x0A
	u8 SerialNumber[32]; 			//冠字号 TAG=42
	u8 TransactionIndex[16]; 		//交易索引 TAG=52	(记录充钱 时可能不 存在)
	u8 DebtorWalletID[8];			//付款钱包ID TAG=01	
	u8 DebtorName[64]; 				//付款钱包名字 TAG=02	
	u8 CreditorWalletID[8];			//收款方钱包ID TAG=03 
	u8 CreditorName[64];			//收款方钱包名称 TAG=04 
	u8 Remark[];		//备注 TAG=74  	(tlv 变长 O 登记APDU获 取的其他 信息，比 如：订单 号、订单 时间、商 户编号、 商户名 称)
}TransactionBill;	//DC交易流水 TAG=0x64


typedef struct
{
	u8 tag; 	//DC交易流水 TAG=0x64
	u8 len[2];	
	TransactionBill tBill;
}DfTransactionBill;	//钱包保存流失



typedef struct
{
	u8 TransactionTimeStamp[6]; 	//交易时间戳 TAG=0x0A
	u8 TransactionBizType[4];		//业务类型编码 TAG=0x08
	u8 TransactionStatus;			//交易状态 TAG=0x5E
	u8 Remark[];		//备注 TAG=74  	(tlv 变长 O 登记APDU获 取的其他 信息，比 如：订单 号、订单 时间、商 户编号、 商户名 称)
}OperationLog;	//操作日志 TAG=0x66



typedef struct
{
	u8 DebtorWalletID[8];			//付款钱包ID TAG=01	
	u8 DebtorName[64]; 				//付款钱包名字 TAG=02	
	u8 CreditorWalletID[8]; 		//收款方钱包ID TAG=03 
	u8 CreditorName[64];			//收款方钱包名称 TAG=04 	
	u8 TransactionType[4];			//交易类型 TAG=0x07
	u8 TransactionBizType[4];		//业务类型编码 TAG=0x08
	u8 TransactionCategoryPurpose[5];//业务种类编码 TAG=0x09
	u8 TransactionTimeStamp[6]; 	//交易时间戳 TAG=0x0A
	u8 MrchntNo[35]; 				//商户编号 TAG=0B
	u8 MrchntName[60];				//商户名称 TAG=0C
	u8 OrderNo[40];					//订单号 TAG=0D
	u8 DebtorPubKeyCert[100]; 		//付款方公钥证书 TAG=0E
	u8 CreditorPubKeyCert[100];		//收款方公钥证书 TAG=0F
	u8 OrderDetails[]; 	//订单详情 TAG=B0 (包括商品名称、商品类型、商品数量、商品单价等)
}ContextData;	//上下文数据（ContextData）为通过APDU指令进行传递交易双方的信息


typedef struct
{
	u8 LoginPwd[8];			//登录密码 TAG=C0
	u8 VerifyTimes;			//连续验证次数 TAG=C1 (每验证未通过一次，则本数据加1；如验证通过，置0)
}LoginPwdInfo;	//登录密码信息Y TAG=F1         13

typedef struct
{
	u8 TranPwd[8];		//交易密码 TAG=C2 (设置钱包的交易密码)
	u8 VerifyTimes;		//连续验证次数 TAG=C1 (每验证未通过一次，则本数据加1；如验证通过，置0)
}TranPwdInfo;	//交易密码信息Y TAG=F2         13

typedef struct
{
	u8 DataEncKey[16];		//报文加密密钥 TAG=CB (生成会话加密密钥，对交易报文进行加密)
	u8 MessageAuthKey[16];	//报文MAC密钥 TAG=CC (生成会话MAC密钥，对交易报文计算校验值)
}AuthKey;	//认证密钥Y TAG=CA     16 (生成会话认证密钥，用于硬件钱包与运营机构之间的合法性认证)



typedef struct
{
	u16 bitFlag;			//bit0,PersPriKeyCert,bit1=PersPubKeyCert,bit2=PbcPubKeyCert,bit3=InstructingPatryPubKeyCert
	u16	KeyLen;				//个人私钥长度
	u8 PersPriKeyCert[64+32];			//个人私钥 TAG=C9 

	u16	PerLen;				//个人公钥证书长度
	u8	PersPubKeyCert[256-2];				//个人公钥证书 TAG=C8 
	
	u16	PbcLen;				//人行公钥证书长度
	u8	PbcPubKeyCert[256-2];				//人民银行公钥证书 TAG=C6 
	
	u16	InsLen;				//运营机构公钥证书长度
	u8 InstructingPatryPubKeyCert[256-2];	//运营机构公钥证书 TAG=C7 (可以存放多家运营机构证书)
}CertKey;	//认证密钥Y TAG=CA     16

typedef struct
{
	u8 PwdVerifyMaxTimes;	//密码连续验证最大次数 TAG=D0 (运营管理机构设置，登录或者交易密码连续验证未通过最大次数。超过该次数，密码应该被锁住。)
	u8 SeID[20];			//硬件钱包硬件介质ID TAG=D1 (如国际移动用户识别码IMSI，国际移动用户设备识别码或ICCID等。)
	u8 AID[16];				//应用标志符 TAG=D2 (参考GB/T 16649.5中描述的应用定义)
}OtherKey;	//认证密钥Y TAG=CA     16

typedef struct
{
	u8 					Enc[16];	//加密密钥
	char 				Mac[16];	//Mac密钥
}DfSessionKey;		//交易会话密钥

typedef struct
{
	uSystemControlInfo	Control;	//控制信息
	WalletInfo 			fWallet;	//硬件钱包
}Df_SC_WA;	//钱包数据

typedef struct
{
	u32 			InvVoucherBal;		//凭证余额[4] 反码值
	u8	 			InvFlag;			//凭证存在标记 ff 不存在,D 存在
	char	 		nUes[3];			//
}Df_DC_NI;	//钱包数据

typedef struct
{
	WalletInfo			Info;		//钱包信息
	uSystemControlInfo	Control;	//系统控制实现信息地址 uSystemControlInfo
	//Df_DC_NI			dc[96];
}uWalletHead;		//钱包引导区




typedef struct
{
	uWalletHead 		*pWallet;		//钱包引导区
	uSystemRealizeInfo	*pRealizeInfo;	//控制信息
	SM2_PRIVKEY			*pKey,*pNewKey;	//钱包公私钥
	u8					CreditorPubKey[64];	//收款方公钥
	u8 					WalletID[8];	//钱包ID
	u8 					Enckey[16];	//钱包密钥
	//-----------交互中得到的数据--------------
	int					index;
	DfSessionKey		sesKey;			//会话密钥
	u8					TransType[4];	//交易类型
	u8					TransIndex[16];	//交易索引
	u32 				RandA[2];		//收款方随机数（8字节）
	u32 				RandB[2];		//付款方随机数（8字节）
	u32 				FactA[4];		//收款方会话因子 （16字节）
	u32 				FactB[4];		//付款方会话因子 （16字节）
	u32 				Amount;			//交易金额（6字节）
	u8 					TimeStamp[6];	//时间戳 （6字节）
	char 				CreditorInsPartyID[8];	//
	char 				CreditorWalletID[8];	//
}uWalletFD;		//钱包引导区


typedef struct
{	
	u8 					WalletName[32];	//钱包ID
	u8 					WalletID[8];	//钱包ID
	u8 					PWCheck[4];		//密码校验值，对应密钥加密私钥
}DfFileHeadInfo;		//储存在wallet.head 文件中


typedef struct
{	
	DfFileHeadInfo 		HeadWallet;	//储存文件
	uWalletFD 			Wallet;	//钱包引引导区地址
}DfHeadInfo;			//储存区引导

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
}DC_APDU;	//DC支出凭证 TAG=0x63



typedef struct
{
	u8 WalletID[8];			//钱包ID TAG=0x40
	u8 WalletLevel[4];		//钱包等级 TAG=0x5f02	(WL01~WL04：一类~四类)
	u8 WalletType[4];		//钱包类型 TAG=0x5f03	(WT01,WT02,WT09,WT10:个人，子个人，对公，子对公)
	char WalletName[60];		//钱包名称 TAG=0x5f05	
	char CustTelePhone[35];	//客户手机 TAG=0x5f06	
	char CustEmail[64];		//客户邮箱 TAG=0x5f07
	u8 FinInstCode[14];		//金融机构编码 TAG=0x5f08
	char CustName[64];		//客户名称 TAG=0x5f09
	u8 WalletSetTime[7];	//钱包启用日期 TAG=0x5f0A
	u8 IDType[4];			//证件类型 TAG=0x5f0B	(IT01,IT02....:身份证，军官证...)
	char IDNo[32];			//证件号 TAG=0x5f0C
}uWalletPort;		//钱包引导区



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
	u8 					amount[6];	//交易金额（6字节）
	u8 					rand[4];	//收款方随机数（4字节）
	u8 					PubKey[64];	//收款方随机数（64字节）
}uPaymentApp;		//钱包引导区


//======================脱机收款===========================
typedef struct
{
	u8 					amount[6];		//交易金额（6字节）
	u8 					RandB[8];		//付款方随机数（4字节）
	u8 					TimeStamp[6];	//时间戳 （6字节）
	u8 					CertChainB[];	//付款方证书连 (运营机构交易证书 + 硬件钱包交易证书)
}DfRecviveInit;		

typedef struct
{
	u8 					wallAmount[6];
	u8 					ATC[2];			//硬件钱包交易计数器
	u8 					RandA[8];		//收款方随机数（4字节）
	u8 					CertChainA[];	//收款方证书连 (运营机构交易证书 + 硬件钱包交易证书)
	//u8 					FactA[16+96];	//收款方会话因子
	//u8 					SigA[64];		//签名A
}DfRecviveResI;


typedef struct
{
	u8					RandB[8];		//付款方随机数（4字节）
	u8					CertChainB[];	//付款方证书连 (运营机构交易证书 + 硬件钱包交易证书)
	//u8					FactB[16];		//用于生成会话密钥的因子		
	//u8					sign[64];	//签名数据
}DfPaymentCerI; 	

typedef struct
{
	u8					FactA[16];		//用于生成会话密钥的因子
	PaymentVoucher		payVou; 		//DCEP币串和交易子链=支出凭证
	//u8					MAC1[4];		//
}DfPaymentResO;



typedef struct
{
	u8 					FactB[16+96];	//付款方会话因子
	u8 					PaymentVoucher[];	//支出凭证(ASN1 结构)
	//u8 					MAC1[4];
}DfRecviveOperation;		

typedef struct
{
	u8 					MAC2[4];		//
}DfRecviveResOpe;		


typedef struct
{
	//u8 					amount[6];		//交易金额（凭证总计余额）
	u8 					Voucher[128];		//余额与支出凭证(ASN1 结构,多个凭证对应多个61或63)
}DfRecviveResSyn;		


typedef struct
{
	u8 					DateTime[7];	//更新日期时间
	u8 					pDc[];			//币串
}DfRecviveDcSyn;		



#endif	//_HARD_INFO_
