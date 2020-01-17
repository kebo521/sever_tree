//====================================================================
/*****************************表达式解析并运算******************************
//功能-------  脚本语言解析器基础，表达式运算
//作者-------  邓国祖
//创作时间---  20170106
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

	DATA_DEFINE_RULES,			//非规范定义--运算表达式，+ - * / 等
	DATA_DEFINE_TAG,			//非规范定义,表示TAG标签，
	DATA_DEFINE_ASIC4,			//非规范定义,4字节字符
	
	DATA_DEFINE_DECIMAL_12_0 = 10,
	DATA_DEFINE_DECIMAL_12_1,
	DATA_DEFINE_DECIMAL_12_2,
	DATA_DEFINE_DECIMAL_12_3,
	DATA_DEFINE_DECIMAL_12_4,
	DATA_DEFINE_DECIMAL_12_5
}DATA_DEFINE_TYPE;

//=======数据空间类型===============
typedef enum
{
	SPACE_LINK,	//临时参数空间
	SPACE_NAME, //定义参数空间
}DATA_SPACE_TYPE;

//=========规范定义=================
//数据存储类型	b	1	INT	0-结构化
//					1－ans、a、s、an、as
//					2－n 
//					3－b 
//					4－cn
typedef enum
{
	DATA_STORAGE_STRUCT,
	DATA_STORAGE_ANS,
	DATA_STORAGE_N,
	DATA_STORAGE_B,
	DATA_STORAGE_CN,
}DATA_STORAGE_TYPE;

//长度类型	b	1	INT	1-VAR  2-FIX
typedef enum
{
	CPOS_DATA_LEN_VAR = 1,
	CPOS_DATA_LEN_FIX
}DATA_LEN_TYPE;


//========逻辑关系处理===========
typedef void* (*FUNC_ExpVoid)(void*); 
typedef struct
{
	char ExpChars[2];		//逻辑表达试
	s8 ParOffset;			//参数偏移左为负，右为正
	u8	Priority;			//优先级
	FUNC_ExpVoid pStrExp;	//处理函数	
}EXP_OperSymbolMatchTbl;

//======数据基源(包括数据与逻辑关系)===============
typedef struct _EXP_UNIT
{
	u8 type;		//参数类型，{@link DATA_DEFINE_TYPE}
	u8 space;		//空间类型，{@link DATA_SPACE_TYPE}
	u16 Len;		//参数长度 最大长度支持4096
	int decimals;		//小数位参数，最大支持小数点后四位，在Tag标签中表示偏移量
	union
	{
		EXP_OperSymbolMatchTbl* pRule;	//下一参数运算规则(Type==DATA_DEFINE_RULES)
		char *pStr;						//字符串(Type==DATA_DEFINE_STR)
		u8 *pBuff;					//缓冲字符号(Type==DATA_DEFINE_BUFF)
		char Asi[4];					//最大存4个字符用于代替短字符(Type==DATA_DEFINE_ASIC4)
		int	iData;						//有符号数值(Type==DATA_DEFINE_INT)
		u32 uData;						//无符号数值,TAG标签必须用这个
		u32 Tag;						//TAG标签值(Type==DATA_DEFINE_TAG)
		BOOL Result;					//返回结果1和0(Type==DATA_DEFINE_BOOL)
	};
	struct _EXP_UNIT* pNext;
}EXP_UNIT;

//=====定义一个参数结构用于临时参数处理==通过:var定义=======
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
