//====================================================================
/*****************************表达式解析并运算******************************
//功能-------  脚本语言解析器基础，表达式运算
//作者-------  邓国祖
//创作时间---  20170106
******************************************************************************/
#ifndef _EXPRES_FUNCTION_
#define _EXPRES_FUNCTION_


typedef EXP_UNIT* (*FUNC_StrExp)(EXP_UNIT*); 

typedef struct
{
	char*	pFunName;			//优先级
	FUNC_StrExp pFunExp;	//处理函数	
}EXP_FunctionMatchTbl;

extern EXP_UNIT* EXP_StrFun(char** pStr,char* pEnd);
extern EXP_UNIT* EXP_Fsave(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_Fread(EXP_UNIT* pInPar);


extern void EXP_StrFunTreeInit(void);
extern EXP_UNIT* EXP_StrFunTree(char** pStr,char* pEnd);
extern void EXP_StrFunTreeEnd(void);

#endif
