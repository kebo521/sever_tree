//====================================================================
/*****************************���ʽ����������******************************
//����-------  �ű����Խ��������������ʽ����
//����-------  �˹���
//����ʱ��---  20170106
******************************************************************************/
#ifndef _EXPRES_FUNCTION_
#define _EXPRES_FUNCTION_


typedef EXP_UNIT* (*FUNC_StrExp)(EXP_UNIT*); 

typedef struct
{
	char*	pFunName;			//���ȼ�
	FUNC_StrExp pFunExp;	//������	
}EXP_FunctionMatchTbl;

extern EXP_UNIT* EXP_StrFun(char** pStr,char* pEnd);
extern EXP_UNIT* EXP_Fsave(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_Fread(EXP_UNIT* pInPar);


extern void EXP_StrFunTreeInit(void);
extern EXP_UNIT* EXP_StrFunTree(char** pStr,char* pEnd);
extern void EXP_StrFunTreeEnd(void);

#endif
