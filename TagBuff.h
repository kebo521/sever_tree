//====================================================================
/*****************************���μ�������***�ڴ�Ԥ��***************************
//����-------  ���ڽ������ַ������������������ռ���������ַ����������߼���Ч��
//ʹ��-------  �����ڲ˵��������ű�POS�ű����������ʽ�����������������ͬ�����ļ�ϵͳ
//����-------  �˹���
//����ʱ��---20161214
******************************************************************************/
#ifndef _TREE_TAG_
#define _TREE_TAG_

typedef enum
{ 
	CURRENT_DATA_SPACE=1,		//1-��ǰ�������ݿռ� 
	EMV_DATA_SPACE=2,			//2_EMV�ں����ݿռ�
	VARIABLE_DATA_SPACE=3,		//3-�����������ݿռ�
	HISTORY_DATA_SPACE=4,		//4-��ʷ�������ݿռ�
}UNIT_SPACE_TYPE;

typedef struct
{
	u32		Tag;	//����??(����D��?������??)
	u16 	Len;	//��y?Y3��?��
	u8		val[0];
}DfTLV_TERM;

//===============20171026===========================
typedef struct
{
	char	nNot[3];
	char	nChar[5];
	int		nInt;
}LOTTERY_DATA_UNIT;

typedef struct _LOTTERY_DATA_Link
{
	struct _LOTTERY_DATA_Link* pNext;
	int	Data;
	int	total;
	LOTTERY_DATA_UNIT lotteryUnit[120];
}LOTTERY_DATA_Link;
extern u32 APP_StrToNum(char* p,u8 len,u8 progress);
extern char* APP_GetBitBuff(u32 uBitData,u32 StartBit,char* pOutBit);
extern char* APP_BitXorBuff_New(char* pBitData,u32 BitLen,u32 StartBit,u32 BitType);
//==========================================================


//extern 
//===========��ǰ�������ݴ����ڴ�======================
extern DfTLV_TERM* APP_GetCurrent_TLV(u32 Tag);
extern int APP_SetCurrent_TLV(DfTLV_TERM* pTlv);
extern int APP_RemoveCurrent_TLV(u32 Tag);
extern int APP_RemoveAll_TLV(void);


extern u32 APP_StrGetNum(char* p,u8 progress,char guide);
extern char* APP_StrGetStr(char* p,int* pOutLen,char guide);
extern int APP_Conv_StrToBcd(char* Indata,u8 Inlen,u8* pBcd);
extern void APP_Conv_BcdToStr(u8* pBcd,int Inlen,char* pOutStr);

extern void APP_fileSave(char* pFileName,void *pInBuff,int Inlen);
extern void* APP_fileRead(char* pFileName,int *pOutLen);



#endif
