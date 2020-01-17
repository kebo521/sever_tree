//====================================================================
/*****************************表达式解析并运算******************************
//功能-------  脚本语言解析器基础，表达式运算
//作者-------  邓国祖
//创作时间---  20170106
******************************************************************************/
#include "comm.h"



//=========逻辑运算处理====FUNC_StrExp===================================

//=================大于等于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_GreaterThanOrEqual(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else
	{
		iResul=pUNIT1->iData - pUNIT2->iData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	if(iResul<0) pUNIT1->Result=0;
	else pUNIT1->Result=1;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================小于等于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_LessThanOrEqual(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else
	{
		iResul=pUNIT1->iData - pUNIT2->iData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	if(iResul<=0) pUNIT1->Result=1;
	else pUNIT1->Result=0;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================等于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_BeEqual(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_TAG)
	{
		//pUNIT1->pStr=APP_GetCurrent_TLV(pUNIT1->Tag,pUNIT1->decimals,NULL);
		//pUNIT1->type = DATA_DEFINE_STR;

	}
	if(pUNIT2->type == DATA_DEFINE_TAG)
	{
		//pUNIT2->pStr=APP_GetCurrent_TLV(pUNIT2->Tag,pUNIT2->decimals,NULL);
		//pUNIT2->type = DATA_DEFINE_STR;
	}

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return pUNIT1;
	else
	{
		iResul=(pUNIT1->iData - pUNIT2->iData);
	}
	TRACE("EXP_BeEqual result=%d",iResul);
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pStar->pNext,2);
	pStar->pNext=pUNIT2;
	//--------填充结果------------------------
	pStar->type=DATA_DEFINE_BOOL;
	if(iResul) pStar->Result=FALSE;
	else pStar->Result=TRUE;
	return pStar;		
}

//=================不等于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_NoEqual(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else
	{
		iResul=pUNIT1->iData - pUNIT2->iData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	if(iResul) pUNIT1->Result=1;
	else pUNIT1->Result=0;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}
 
//=================与运算赋值========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_AndOperationResult(EXP_UNIT* pStar)
{
	int i;
	u32	TagFlag=0;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	if(pUNIT1->type==DATA_DEFINE_TAG)
	{
		TagFlag=pUNIT1->Tag;
		pUNIT1=EXP_Fread(pUNIT1);
	}
	if(pUNIT2->type==DATA_DEFINE_TAG)
	{
		pUNIT2=EXP_Fread(pUNIT2);
	}

	if((pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR) \
		||(pUNIT1->type ==DATA_DEFINE_BUFF&&pUNIT2->type == DATA_DEFINE_BUFF))
	{
		for(i=0;i<pUNIT1->Len;i++)
		{
			pUNIT1->pStr[i] &= pUNIT2->pStr[i];
		}
		if(TagFlag)
		{
			pUNIT1=EXP_Fsave(pUNIT1);
		}
	}
	else
	{
		pUNIT1->uData |= pUNIT2->uData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	
	return pUNIT1;		
}

//=================或运算赋值========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_OrOperationResult(EXP_UNIT* pStar)
{
	int i;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	u32		TagFlag=0;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	if(pUNIT1->type==DATA_DEFINE_TAG)
	{
		TagFlag=pUNIT1->Tag;
		pUNIT1=EXP_Fread(pUNIT1);
	}
	if(pUNIT2->type==DATA_DEFINE_TAG)
	{
		pUNIT2=EXP_Fread(pUNIT2);
	}

	if((pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR) \
		||(pUNIT1->type ==DATA_DEFINE_BUFF&&pUNIT2->type == DATA_DEFINE_BUFF))
	{
		for(i=0;i<pUNIT1->Len;i++)
		{
			pUNIT1->pStr[i] |= pUNIT2->pStr[i];
		}
		if(TagFlag)
		{
			pUNIT1=EXP_Fsave(pUNIT1);
		}
	}
	else
	{
		pUNIT1->uData |= pUNIT2->uData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	
	return pUNIT1;		
}

//=================异或运算赋值========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_XorOperationResult(EXP_UNIT* pStar)
{
	int i;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	u32		TagFlag=0;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	if(pUNIT1->type==DATA_DEFINE_TAG)
	{
		TagFlag=pUNIT1->Tag;
		pUNIT1=EXP_Fread(pUNIT1);
	}
	if(pUNIT2->type==DATA_DEFINE_TAG)
	{
		pUNIT2=EXP_Fread(pUNIT2);
	}

	if((pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR) \
		||(pUNIT1->type ==DATA_DEFINE_BUFF&&pUNIT2->type == DATA_DEFINE_BUFF))
	{
		for(i=0;i<pUNIT1->Len;i++)
		{
			pUNIT1->pStr[i] ^= pUNIT2->pStr[i];
		}
		if(TagFlag)
		{
			pUNIT1=EXP_Fsave(pUNIT1);
		}
	}
	else
	{
		pUNIT1->uData ^= pUNIT2->uData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	
	return pUNIT1;		
}
 

//=================逻辑与判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_LogicalAnd(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	if(pUNIT1->Result && pUNIT2->Result)
		pUNIT1->Result=1;
	else pUNIT1->Result=0;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================逻辑或判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_LogicalOr(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	if(pUNIT1->Result || pUNIT2->Result)
		pUNIT1->Result=1;
	else pUNIT1->Result=0;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================三目运算========EXP_UNIT_Par1+EXP_UNIT_Rule?+EXP_UNIT_Par2+EXP_UNIT_Rule:+EXP_UNIT_Par3========================
EXP_UNIT* EXP_ThreeItemOperation(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	EXP_UNIT* pUNIT3;
	pUNIT1=pStar;
	pUNIT2=pUNIT1->pNext->pNext;
	pUNIT3=pUNIT2->pNext->pNext;
	if(pUNIT2->pNext->type != DATA_DEFINE_RULES) return NULL;
	if(pUNIT2->pNext->pRule->ExpChars[0] != ':') return NULL;
	
	if(pUNIT1->type == DATA_DEFINE_RULES \
		|| pUNIT2->type == DATA_DEFINE_RULES \
		|| pUNIT3->type == DATA_DEFINE_RULES )
		return NULL;
	if(pUNIT1->type == DATA_DEFINE_STR)
		return NULL;
	if(pUNIT1->Result)
	{//---pUNIT2 赋值到 pUNIT1-------
		API_memcpy(pUNIT1,pUNIT2,sizeof(EXP_UNIT));
		pUNIT1->pNext=pUNIT3->pNext;
		if(pUNIT2->type == DATA_DEFINE_STR)
			pUNIT2->pStr=NULL;
	}
	else
	{//---pUNIT3 赋值到 pUNIT1-------
		API_memcpy(pUNIT1,pUNIT3,sizeof(EXP_UNIT));
		if(pUNIT3->type == DATA_DEFINE_STR)
			pUNIT3->pStr=NULL;
	}
	//------释放参与计算的四个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,4);
	//--------填充结果------------------------
	return pUNIT1;		
}

 //=================条件取反========EXP_UNIT_Rule1!+EXP_UNIT_Par1========================
EXP_UNIT* EXP_Negation(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	pUNIT1=pStar->pNext;
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT1->type == DATA_DEFINE_STR) 
		return NULL;	
	//---pUNIT2 赋值到 pUNIT1-------
	API_memcpy(pStar,pUNIT1,sizeof(EXP_UNIT));
	//------取非操作----------
	pStar->type=DATA_DEFINE_BOOL;
	if(pStar->Result) pStar->Result=0;
	else pStar->Result=1;
	//------释放参与计算的四个节点---------
	EXP_FreeUNIT(pUNIT1,1);
	//--------填充结果------------------------
	return pStar;		
}

//=================乘法运算========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_MultipliesOfTwoPar(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;

	pUNIT1->iData *= pUNIT2->iData;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================除法运算========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_DivisionOfTwoPar(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;

	pUNIT1->iData /= pUNIT2->iData;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================取余数========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_TakeRemainder(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;

	pUNIT1->iData %= pUNIT2->iData;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================加法运算========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_AdditionOfTwoPar(EXP_UNIT* pStar)
{
	//u16 ReLen=0;
	char *pBuff;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	
	if(pUNIT1->type == DATA_DEFINE_TAG)
	{
		//pUNIT1->pStr= APP_GetCurrent_TLV(pUNIT1->Tag,pUNIT1->decimals,&ReLen);
		//pUNIT1->Len =ReLen;
		//pUNIT1->type = DATA_DEFINE_STR;
	}
	if(pUNIT2->type == DATA_DEFINE_TAG)
	{
		//pUNIT2->pStr= APP_GetCurrent_TLV(pUNIT2->Tag,pUNIT2->decimals,&ReLen);
		//pUNIT2->Len =ReLen;
		//pUNIT2->type = DATA_DEFINE_STR;
	}

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		pBuff=(char*)malloc(pUNIT1->Len+pUNIT2->Len+1);
		API_memcpy(pBuff,pUNIT1->pStr,pUNIT1->Len);
		API_memcpy(pBuff+pUNIT1->Len,pUNIT2->pStr,pUNIT2->Len);
		pUNIT1->Len += pUNIT2->Len;
		pBuff[pUNIT1->Len]='\0';
		free(pUNIT1->pStr);
		pUNIT1->pStr=pBuff;
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else pUNIT1->iData += pUNIT2->iData;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}


//=================加法运算========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_SubtractionOfTwoPar(EXP_UNIT* pStar)
{
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	pUNIT1->iData -= pUNIT2->iData;
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================大于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_GreaterThan(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else
	{
		iResul=pUNIT1->iData - pUNIT2->iData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	if(iResul>0) pUNIT1->Result=1;
	else pUNIT1->Result=0;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}

//=================小于判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_LessThan(EXP_UNIT* pStar)
{
	int i,iResul;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;

	if(pUNIT1->type == DATA_DEFINE_STR && pUNIT2->type == DATA_DEFINE_STR)
	{
		for(i=0;;i++)
		{
			iResul=pUNIT1->pStr[i] - pUNIT2->pStr[i];
			if(iResul) break;
			if(pUNIT1->pStr[i]=='\0') break;
		}
	}
	else if(pUNIT1->type == DATA_DEFINE_STR || pUNIT2->type == DATA_DEFINE_STR)
		return NULL;
	else
	{
		iResul=pUNIT1->iData - pUNIT2->iData;
	}
	//-------记录参数后面的链接---------------
	pUNIT2=pUNIT2->pNext;
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->type=DATA_DEFINE_BOOL;
	if(iResul<0) pUNIT1->Result=1;
	else pUNIT1->Result=0;
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}


//=================赋值操作判断========EXP_UNIT_Par1+EXP_UNIT_Rule+EXP_UNIT_Par2========================
EXP_UNIT* EXP_Assignment(EXP_UNIT* pStar)
{
//	u16 ReLen=0;
//	char *pBuff;
	EXP_UNIT* pUNIT1;
	EXP_UNIT* pUNIT2;
	pUNIT1=pStar;
	pUNIT2=pStar->pNext->pNext;
	
	if(pUNIT1->type == DATA_DEFINE_RULES || pUNIT2->type == DATA_DEFINE_RULES)
		return NULL;
	if(pUNIT2->type == DATA_DEFINE_TAG)
	{
		pUNIT2=EXP_Fread(pUNIT2);
	}

	if(pUNIT1->type == DATA_DEFINE_TAG)
	{	
		EXP_FreeUNIT(pUNIT1->pNext,1);
		pUNIT1->pNext = pUNIT2;
		pUNIT1=EXP_Fsave(pUNIT1);
	}
	//------释放参与计算的两个节点---------
	EXP_FreeUNIT(pUNIT1->pNext,2);
	//--------填充结果------------------------
	pUNIT1->pNext=pUNIT2;
	return pUNIT1;		
}


//=========数据源素处理=======================================
const EXP_OperSymbolMatchTbl OperatorSymbolMatchTbl[] = 
{
	//---2-----------------
	{{'>','='},		-1,	5,	(FUNC_ExpVoid)EXP_GreaterThanOrEqual},
	{{'<','='},		-1,	5,	(FUNC_ExpVoid)EXP_LessThanOrEqual},
	{{'=','='},		-1,	5,	(FUNC_ExpVoid)EXP_BeEqual},
	{{'!','='},		-1,	5,	(FUNC_ExpVoid)EXP_NoEqual},
	{{'&','='},		-1,	6,	(FUNC_ExpVoid)EXP_AndOperationResult},
	{{'|','='},		-1,	6,	(FUNC_ExpVoid)EXP_OrOperationResult},
	{{'^','='},		-1,	6,	(FUNC_ExpVoid)EXP_XorOperationResult},
	{{'&','&'},		-1,	7,	(FUNC_ExpVoid)EXP_LogicalAnd},
	{{'|','|'},		-1,	7,	(FUNC_ExpVoid)EXP_LogicalOr},
	{{'?','\0'},	-1,	8,	(FUNC_ExpVoid)EXP_ThreeItemOperation},
	//----1-----------------------
	{{'!','\0'},	0,	2,	(FUNC_ExpVoid)EXP_Negation},
	{{'*','\0'},	-1,	3,	(FUNC_ExpVoid)EXP_MultipliesOfTwoPar},
	{{'/','\0'},	-1,	3,	(FUNC_ExpVoid)EXP_DivisionOfTwoPar},
	{{'%','\0'},	-1, 3,	(FUNC_ExpVoid)EXP_TakeRemainder},
	{{'+','\0'},	-1,	4,	(FUNC_ExpVoid)EXP_AdditionOfTwoPar},
	{{'-','\0'},	-1,	4,	(FUNC_ExpVoid)EXP_SubtractionOfTwoPar},
	{{'>','\0'},	-1,	5,	(FUNC_ExpVoid)EXP_GreaterThan},
	{{'<','\0'},	-1,	5,	(FUNC_ExpVoid)EXP_LessThan},
	//-----3------------------------
	{{'=','\0'},	-1,	9,	(FUNC_ExpVoid)EXP_Assignment},
	//------------------------------------
//	{':','\0'},	-3,	10,	NULL,	//参数隔离符号
//	{',','\0'},	0,	10,	NULL,	//参数隔离符号
};



//========处理逻辑关系===========================
EXP_UNIT* EXP_StrToRules(char** pStr,char* pEnd)
{
	char cTag;
	u8 i,Max;
	char* p=*pStr;
	EXP_UNIT* pUNIT=NULL;
	Max=sizeof(OperatorSymbolMatchTbl)/sizeof(OperatorSymbolMatchTbl[0]);
	for(cTag=*p,i=0;i<Max;i++)
	{
		if(cTag==OperatorSymbolMatchTbl[i].ExpChars[0])
		{
			if(OperatorSymbolMatchTbl[i].ExpChars[1]!='\0')
			{
				if(p[1]==OperatorSymbolMatchTbl[i].ExpChars[1])
				{
					p+=2;
					break;
				}
			}
			else
			{
				p++;
				break;
			}
		}
	}
	if(p>pEnd) return NULL;
	*pStr=p;
	if(i<Max)
	{
		pUNIT=(EXP_UNIT*)malloc(sizeof(EXP_UNIT));
		pUNIT->pNext=NULL;
		pUNIT->type=DATA_DEFINE_RULES;
		pUNIT->pRule=(EXP_OperSymbolMatchTbl*)&OperatorSymbolMatchTbl[i];
		TRACE( "EXP_StrToRules %c%c",OperatorSymbolMatchTbl[i].ExpChars[0],OperatorSymbolMatchTbl[i].ExpChars[1]);
	}
	else
	{
		TRACE("EXP_StrToRules Err cTag[%x]:%c",cTag,cTag);
	}
	while(*p==' ') p++;
	*pStr = p;
	return pUNIT;
}

//===============================================================




