//====================================================================
/*****************************���ʽ����������******************************
//����-------  �ű����Խ��������������ʽ����
//����-------  �˹���
//����ʱ��---  20170106
******************************************************************************/
#ifndef _EXPRES_BASICS_
#define _EXPRES_BASICS_


typedef enum
{
	DATA_DEFINE_BUFF,
	DATA_DEFINE_STR,
	DATA_DEFINE_BIN,
	DATA_DEFINE_INT,
	DATA_DEFINE_BOOL,

	DATA_DEFINE_RULES,			//�ǹ淶����--������ʽ��+ - * / ��
	DATA_DEFINE_TAG,			//�ǹ淶����,��ʾTAG��ǩ��
	DATA_DEFINE_ASIC4,			//�ǹ淶����,4�ֽ��ַ�
	
	DATA_DEFINE_DECIMAL_12_0 = 10,
	DATA_DEFINE_DECIMAL_12_1,
	DATA_DEFINE_DECIMAL_12_2,
	DATA_DEFINE_DECIMAL_12_3,
	DATA_DEFINE_DECIMAL_12_4,
	DATA_DEFINE_DECIMAL_12_5
}DATA_DEFINE_TYPE;

//=======���ݿռ�����===============
typedef enum
{
	SPACE_LINK,	//��ʱ�����ռ�
	SPACE_NAME, //��������ռ�
}DATA_SPACE_TYPE;

//=========�淶����=================
//���ݴ洢����	b	1	INT	0-�ṹ��
//					1��ans��a��s��an��as
//					2��n 
//					3��b 
//					4��cn
typedef enum
{
	DATA_STORAGE_STRUCT,
	DATA_STORAGE_ANS,
	DATA_STORAGE_N,
	DATA_STORAGE_B,
	DATA_STORAGE_CN,
}DATA_STORAGE_TYPE;

//��������	b	1	INT	1-VAR  2-FIX
typedef enum
{
	CPOS_DATA_LEN_VAR = 1,
	CPOS_DATA_LEN_FIX
}DATA_LEN_TYPE;


//========�߼���ϵ����===========
typedef void* (*FUNC_ExpVoid)(void*); 
typedef struct
{
	char ExpChars[2];		//�߼������
	s8 ParOffset;			//����ƫ����Ϊ������Ϊ��
	u8	Priority;			//���ȼ�
	FUNC_ExpVoid pStrExp;	//������	
}EXP_OperSymbolMatchTbl;

//======���ݻ�Դ(�����������߼���ϵ)===============
typedef struct _EXP_UNIT
{
	u8 type;		//�������ͣ�{@link DATA_DEFINE_TYPE}
	u8 space;		//�ռ����ͣ�{@link DATA_SPACE_TYPE}
	u16 Len;		//�������� ��󳤶�֧��4096
	int decimals;		//С��λ���������֧��С�������λ����Tag��ǩ�б�ʾƫ����
	union
	{
		EXP_OperSymbolMatchTbl* pRule;	//��һ�����������(Type==DATA_DEFINE_RULES)
		char *pStr;						//�ַ���(Type==DATA_DEFINE_STR)
		u8 *pBuff;					//�����ַ���(Type==DATA_DEFINE_BUFF)
		char Asi[4];					//����4���ַ����ڴ�����ַ�(Type==DATA_DEFINE_ASIC4)
		int	iData;						//�з�����ֵ(Type==DATA_DEFINE_INT)
		u32 uData;						//�޷�����ֵ,TAG��ǩ���������
		u32 Tag;						//TAG��ǩֵ(Type==DATA_DEFINE_TAG)
		BOOL Result;					//���ؽ��1��0(Type==DATA_DEFINE_BOOL)
	};
	struct _EXP_UNIT* pNext;
}EXP_UNIT;

//=====����һ�������ṹ������ʱ��������==ͨ��:var����=======
typedef struct _EXP_NAME_DATA
{
	struct _EXP_NAME_DATA* pLast;
	char uName[12];
	EXP_UNIT tUnit;
}EXP_NAME_DATA;






extern void EXP_FreeUNIT(EXP_UNIT* pStar,int Num);
extern char* EXP_NumToString(char* pOutStr,long long num, u8 base, int size, int precision, int type);
extern u32 Exp_StrToNum(char** pStr,char* pEnd,u8 progress,int* pDecimals);
extern EXP_UNIT* EXP_StrALL(char** pStr,char* pEnd);

extern void Trace_PutMsg(EXP_UNIT* pUnit);

extern int APP_Test_EXP(char *);
#endif
