//====================================================================
/*****************************表达式解析并运算******************************
//功能-------  脚本语言解析器基础，表达式运算
//作者-------  邓国祖
//创作时间---  20170106
******************************************************************************/
#include "comm.h"

//EXP_NAME_DATA expVarALL=NULL;

void EXP_FreeUNIT(EXP_UNIT* pStar,int Num)
{
	EXP_UNIT* pNext;
	while(pStar)
	{
		if(pStar->type <= DATA_DEFINE_STR )//&& pStar->space==SPACE_LINK)
			free(pStar->pStr);
		pNext=pStar->pNext;
		free(pStar);
		pStar=pNext;
		if(--Num == 0) break;
	}
}

//=====================================================================
char* EXP_NumToString(char* pOutStr,long long num, u8 base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits;
	const char small_digits[] = "0123456789abcdef";//ghijklmnopqrstuvwxyz";
	const char large_digits[] = "0123456789ABCDEF";//GHIJKLMNOPQRSTUVWXYZ";
	int i;

	digits = (type & LARGE) ? large_digits : small_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return pOutStr;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0){
		tmp[i++] = digits[((unsigned long)num) % (unsigned int)base];
		num = ((unsigned long)num) / (unsigned int)base;
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT))) {
		while(size-->0) {
			*pOutStr++=' ';
		}
	}
	if (sign) {
		*pOutStr++=sign;
	}
	if (type & SPECIAL) {
		if (base==8) {
			*pOutStr++='0';
		} else if (base==2) {
			*pOutStr++='0';
			*pOutStr++=digits[11];
		} else if (base==16) {
			*pOutStr++='0';
			*pOutStr++=digits[33];
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0) {
			*pOutStr++=c;
		}
	}
	while (i < precision--) {
		*pOutStr++='0';
	}
	while (i-- > 0) {
		*pOutStr++=tmp[i];
	}
	while (size-- > 0) {
		*pOutStr++=' ';
	}
        return pOutStr;
}


u32 Exp_StrToNum(char** pStr,char* pEnd,u8 progress,int* pDecimals)
{
	u32 uNum=0;
	char *p= *pStr;
	//TRACE_HEC("Exp_StrToNum:",*pStr,pEnd-*pStr);
	if(progress==0)
	{
		if(*p == '0')
		{
			p++;
			if(*p=='x' || *p=='X')
			{//--16进制度处理-----
				p++;
				progress=16;
			}
			else
			{//--8进制度处理-----
				progress=8;
			}
		}
		else //if((*pStr)>='1' && (*pStr)<='9')
		{//--10进制度处理-----
			progress=10;
		}
		//------处理浮点参数--------
		if(pDecimals)
		{
			if(*p == '.') progress=10;
		}
	}
	//--------执行转换-----------------
	while(p < pEnd)
	{
		if(*p>='0' && *p<='9')
		{
			uNum =uNum*progress + *p-'0';
		}
		else if(progress==16)
		{
			if(*p>='A' && *p<='F')
			{
				uNum =uNum*progress + *p-'A'+10;
			}
			else if(*p>='a' && *p<='f')
			{
				uNum =uNum*progress + *p-'a'+10;
			}
			else break;
		}
		else break;
		p++;
	}
	if(pDecimals)
	{
		if(*p == '.')
		{
			TRACE( "Exp_StrToNum Decimals");
			p++;
			*pDecimals=1;
			while(p < pEnd)
			{
				if(*p>='0' && *p<='9')
				{
					*pDecimals =*pDecimals * progress + *p-'0';
				}
				else break;
				p++;
			}
		}
		else *pDecimals=0;	//没有小数
	}
	*pStr = p;
	TRACE( "Exp_StrToNum:[%d]",uNum);
	return uNum;
}

//=================拷贝字符串，解析转义字符 =================================
u32 EXP_StrCop(char* pOut,char* pIn,u32 InLen)
{
	u32 i,j;
	for(j=0,i=0;i<InLen;i++)
	{
		if(pIn[i] < 0x20)	//' '
			continue;
		if(pIn[i] != '\\')
			pOut[j++]=pIn[i];
		else switch(pIn[++i])
		{
			case 'a':
				pOut[j++]='\a';
				break;
			case 'b':
				pOut[j++]='\b';
				break;
			case 'f':
				pOut[j++]='\f';
				break;
			case 'n':
				pOut[j++]='\n';
				break;
			case 'r':
				pOut[j++]='\r';
				break;
			case 't':
				pOut[j++]='\t';
				break;
			case 'v':
				pOut[j++]='\v';
				break;
			case '0':
				pOut[j++]='\0';
				break;
			case 'x':
			case 'X':
				{
					u8 aH,aL;
					aH=pIn[i+1];
					aL=pIn[i+2];
					if(aH >= 'a')
						aH=aH-'a'+10;
					else if(aH >= 'A')
						aH=aH-'A'+10;
					else aH=aH-'0';

					if(aL >= 'a')
						aL=aL-'a'+10;
					else if(aL >= 'A')
						aL=aL-'A'+10;
					else aL=aL-'0';
					
					pOut[j++]=aH*16 + aL;
					i += 2;
				}
				break;
			default:
				if(pIn[i]>='1' && pIn[i]<='7')
				{
					pOut[j++]=(pIn[i+1]&0x0f)*64 + (pIn[i+2]&0x0f)*8 + (pIn[i+3]&0x0f);
					i += 3;
				}
				else
				{
					pOut[j++]=pIn[i];
				}
		}
	}
	pOut[j]='\0';
	TRACE("StrCop[%s]",pOut);
	return j;
}

char* EXP_CheckTagBuff(char** pStr,char* pEnd,char startTag,char endTag)
{
	int uNum;
	char *pE;
	char* p=*pStr;
	while(p < pEnd)
	{
		if(*p == startTag) break; //找到tag符号
		if(*p > 0x20) //跳过 空格 换行符等
		{
			TRACE("Scand [%c] Err No[%c]",startTag,*p);
			return NULL;
		}
		p++;
	}
	p++; //跳过 startTag
	if(p < pEnd)
	{
		*pStr=p;//计录起点
		for(uNum=1,pE=++p;pE<pEnd;pE++)
		{
			if(*pE == startTag) uNum++;
			else if(*pE == endTag) uNum--;
			if(uNum==0) return pE;//返回终点
		}	
	}
	TRACE("Scand No Tag[%c,%c]",startTag,endTag);
	return NULL;
}

//======处理参数=============
EXP_UNIT* EXP_StrToPar(char** pStr,char* pEnd)
{
	u16 sLen;
	s16 offset;
	char* p=*pStr;
	EXP_UNIT* pUNIT=NULL;
	if(*p=='\'')
	{
		for(p++,sLen=0;p[sLen] != '\'';sLen++)
		{
			if(p[sLen] == '\\') sLen++;
			if (&p[sLen] >= pEnd) break;
		}
		pUNIT=(EXP_UNIT*)malloc(sizeof(EXP_UNIT));
		pUNIT->pNext=NULL;
		pUNIT->type=DATA_DEFINE_STR;
		if(sLen==0)
		{
			pUNIT->pStr=NULL;
			pUNIT->Len=sLen;
		}
		else
		{
			pUNIT->pStr=(char*)malloc(sLen+1);		// len+'\0'
			pUNIT->Len=EXP_StrCop(pUNIT->pStr,p,sLen);
		}
		p += sLen+1; //跳过\'
	}
	else if(*p=='\"')
	{
		for(p++,sLen=0;p[sLen] != '\"';sLen++)
		{
			if (&p[sLen] >= pEnd) break;
		}
		pUNIT=(EXP_UNIT*)malloc(sizeof(EXP_UNIT));
		pUNIT->pNext=NULL;
		pUNIT->type=DATA_DEFINE_STR;
		if(sLen==0)
		{
			pUNIT->pStr=NULL;
			pUNIT->Len=sLen;
		}
		else
		{
			u16 i,j;
			pUNIT->pStr=(char*)malloc(sLen+1);		// len+'\0'
			for(j=0,i=0;i<sLen;i++)
			{
				if(p[i] >= 0x20)	//' '
					pUNIT->pStr[j++]=p[i];
			}
			pUNIT->pStr[j]='\0';
			pUNIT->Len=j;
		}
		p += sLen+1; //跳过\'
	}
	else if(*p=='$')
	{
		p++;
		pUNIT=(EXP_UNIT*)malloc(sizeof(EXP_UNIT));
		pUNIT->pNext=NULL;
		pUNIT->Tag=Exp_StrToNum(&p,pEnd,16,NULL);
		pUNIT->type=DATA_DEFINE_TAG;
		offset=-1;
		if(*p=='[')
		{
			p++;
			offset=Exp_StrToNum(&p,pEnd,0,NULL);
			while(*p++ != ']'); //滤过‘]'
		}
		//-----还需有取出Tag值----offset=-1 取所有-----------
		pUNIT->decimals=offset;
	}
	else if((*p >='0' && *p <='9')|| *p == '-')
	{//--8、10、16进制度处理-----
		if(*p == '-')	//负数
		{
			p++;
			offset=-1;
		}
		else offset=1;
		pUNIT=(EXP_UNIT*)malloc(sizeof(EXP_UNIT));
		pUNIT->pNext=NULL;
		pUNIT->iData=Exp_StrToNum(&p,pEnd,0,&pUNIT->decimals);
		if(offset<0)
		{
			pUNIT->iData = pUNIT->iData*offset;
			pUNIT->decimals = pUNIT->decimals*offset;
		}
		pUNIT->type=DATA_DEFINE_INT;
	}
	else if(*p == 'f')
	{//---函数处理-------
		pUNIT=EXP_StrFun(&p,pEnd);
		//pUNIT=EXP_StrFunTree(&p,pEnd);
	}
	else if(API_memcmp(p,"if",2) == 0)
	{//---条件语句处理-------
		char *pE;
		p += 2;	//跳过 if
		TRACE("EXP_StrToPar if(){}");
		pE=EXP_CheckTagBuff(&p,pEnd,'(',')');
		pUNIT=EXP_StrALL(&p,pE);
		p=pE+1;
		pE=EXP_CheckTagBuff(&p,pEnd,'{','}');
		if(pE==NULL)
		{//----如果没有大阔号就检查分号---
			pE=p;
			while(*pE != ';') pE++;
		}
		TRACE("EXP_StrToPar Result(%d){%B}",pUNIT->Result,p,pE-p);
		if(pUNIT->Result == TRUE)
		{
			EXP_FreeUNIT(pUNIT,0);//释放逻辑试
			pUNIT=EXP_StrALL(&p,pE);
		}
		p = pE+1; //跳过 '}'
	}
	else if(API_memcmp(p,"for",3) == 0)
	{//---循环语句for处理-------
		char *pE;
		p += 3;	//跳过 for
		TRACE("EXP_StrToPar for(){}");
		pE=EXP_CheckTagBuff(&p,pEnd,'(',')');
		pUNIT=EXP_StrALL(&p,pE);
		p=pE+1;
		pE=EXP_CheckTagBuff(&p,pEnd,'{','}');
	}
	else if(API_memcmp(p,"while",5) == 0)
	{//---循环语句while处理-------
		char *pE;
		p += 5;	//跳过 while
		TRACE("EXP_StrToPar while(){}");
		pE=EXP_CheckTagBuff(&p,pEnd,'(',')');
		pUNIT=EXP_StrALL(&p,pE);
		p=pE+1;
		pE=EXP_CheckTagBuff(&p,pEnd,'{','}');
	}
	else if(API_memcmp(p,"int",5) == 0)
	{//---定义参数-------
		
	}
	else 
	{
		TRACE( "EXP_StrToPar Err *p=[%x]:%c",*p,*p);
	}
	while(*p==' ') p++;
	*pStr = p;
	return pUNIT;
}


//=====处理运算链表=============================
EXP_UNIT* EXP_ProcessList(EXP_UNIT* pStar,int Num)
{
	u8 i,PriorityNum,flagPriority;
	s8 Index;
	EXP_UNIT *pNext,**GroupUNIT;
	u8 *GroupPriority;
	if(Num==0)
	{
		pNext=pStar;
		while(pNext)
		{
			Num++;
			pNext=pNext->pNext;
		}
	}
	if(Num<2) return pStar;

	GroupUNIT=(EXP_UNIT**)malloc(sizeof(EXP_UNIT)*Num);
	GroupPriority=(u8*)malloc(Num);
	for(pNext=pStar,PriorityNum=0,i=0;i<Num;i++)
	{
		GroupUNIT[i]=pNext;
		if(pNext->type==DATA_DEFINE_RULES)
		{
			PriorityNum++;
			GroupPriority[i]=pNext->pRule->Priority;
		}
		else GroupPriority[i]=0;
		pNext=pNext->pNext;
	}
	//------无运算操作----------------------------------
	if(PriorityNum==0) return pStar;
	//-----多个运算符号---------
	while(PriorityNum)
	{
		//-----提取优先级最高的表达试-------
		flagPriority=0xff;
		for(i=0;i<Num;i++)
		{
			if(GroupPriority[i])
			{
				if(flagPriority > GroupPriority[i])
				{
					Index=i;
					flagPriority=GroupPriority[i];
				}
			}
		}
		//------运算表达试-----------------------
		if(Index!=0 && (Index+1)<Num)
		{
			pNext=GroupUNIT[Index+1]->pNext;
			GroupUNIT[Index-1]=(*GroupUNIT[Index]->pRule->pStrExp)(GroupUNIT[Index+ GroupUNIT[Index]->pRule->ParOffset]);
			GroupUNIT[Index-1]->pNext=pNext;
			if(Index >= 2)
			{
				GroupUNIT[Index-2]->pNext=GroupUNIT[Index-1];
			}
			PriorityNum--;	//减少一个运算符号
			//-------删除过程参数----a=bxc------
			Num -= 2;
			for(i=Index;i<Num;i++)
			{
				GroupUNIT[i]=GroupUNIT[i+2];
				GroupPriority[i]=GroupPriority[i+2];
			}
		}
		else break;
	}
	//-----最后一个或唯运算符处理-------
	pStar=GroupUNIT[0];
	free(GroupPriority);
	free(GroupUNIT);
	return pStar;		
}


EXP_UNIT* EXP_StrALL(char** pStr,char* pEnd)
{
	u8	ErrNull,Alternate;
	EXP_UNIT *pStar,*pUNIT=NULL,*pNext=NULL,*pLast=NULL;
	//TRACE( "EXP_StrALL stand ... ");
	//----解析表达试-----
	Alternate=1;
	ErrNull=0;
	pStar=NULL;
	while(*(*pStr) <= ' ') {(*pStr)++;}	//过滤空格与换行符
	while(*pStr < pEnd)
	{
		if(Alternate&0x01)
		{
			pUNIT=EXP_StrToPar(pStr,pEnd);
		}
		else
		{
			pUNIT=EXP_StrToRules(pStr,pEnd);
		}
		Alternate++;
		
		if(pUNIT==NULL)
		{
			if((*pStr) >= pEnd) 
				break;
			if(*(*pStr)=='(')
			{//-----括号处理--------
				u32 uNum;
				char *p;
				TRACE( "EXP_StrALL (...) ");
				for(uNum=1,(*pStr)++,p=*pStr;p<pEnd;p++)
				{
					if(*p == '(') uNum++;
					else if(*p == ')') uNum--;
					if(uNum==0) break;
				}		
				pUNIT=EXP_StrALL(pStr,p); 
				*pStr=p+1;	//par+')'
				Alternate=0;	//同级别下一规则为 逻辑运算式
			}
			else if(*(*pStr)=='{')
			{//-----大括号处理--------
				u32 uNum;
				char *p;
				TRACE( "EXP_StrALL {...} ");
				for(uNum=1,(*pStr)++,p=*pStr;p<pEnd;p++)
				{
					if(*p == '{') uNum++;
					else if(*p == '}') uNum--;
					if(uNum==0) break;
				}		
				pUNIT=EXP_StrALL(pStr,p); 
				*pStr=p+1;	//par+')'
				Alternate=0;	//同级别下一规则为 逻辑运算式
			}
			else if(*(*pStr)=='/')  // 增加屏蔽处理 /* */ or //----
			{
				if((*pStr)[1]== '*')
				{
					u32 uNum;
					char *p;
					TRACE( "EXP_StrALL /*	*/");
					p=(*pStr) +2;
					uNum=1;
					while(p<pEnd){
						if(p[0] == '/'){
							if(p[1] == '*'){
								p += 2;
								uNum++;
							}
						}
						else if(p[0] == '*'){
							if(p[1] == '/'){
								p += 2;
								uNum--;
							}
						}
						if(uNum==0) {
							while(*p <= 0x20) p++;	//过滤 ' ','\r','\n' 等 
							*pStr = p;
							Alternate=1;	//同级别下一规则为 很可能为参数处理
							break;
						}
						p ++;
					}		
				}
				else if((*pStr)[1]== '/')
				{
					char *p;
					TRACE( "EXP_StrALL //---NO----");
					p=(*pStr) +2;
					while(p<pEnd)
					{
						if(*p++ == '\n') break;
					}
					*pStr = p;
					Alternate=1;	//同级别下一规则为 很可能为参数处理
				}		
			}
			
			else //if(*(*pStr)==',')
			{
				ErrNull++;
				if(ErrNull>1)		
				{//----处理分割符号---过程表达试处理-
					TRACE( "EXP_StrALL other[%x][%c] ",*(*pStr) & 0xff,(char)*(*pStr));
					pUNIT=EXP_ProcessList(pStar,0);
					if(pLast)
					{
						pNext=pLast;
						while(pNext->pNext) 
							pNext=pNext->pNext;
						pNext->pNext=pUNIT;
					}
					else pLast=pUNIT;
					(*pStr)++;	//过滤特殊符号
					pStar=NULL;
					pUNIT=NULL;
					pNext=NULL;
					Alternate=1;	//同级别下一规则为 很可能为参数处理
					ErrNull=0;
				}
			}
		}
		
		if(pUNIT)
		{
			if(pStar==NULL)
			{//第一节点
				pStar=pUNIT;
				pNext=pUNIT;
			}
			else if(pNext)
			{
				pNext->pNext=pUNIT;
				//--实现多个参数连接(主要针对函数返回值)----
				while(pUNIT->pNext)
					pUNIT=pUNIT->pNext;
				pNext=pUNIT;
			}
			ErrNull=0;
		}
		while(*(*pStr) == ' ') {(*pStr)++;}	//过滤空格
	}
	//----处理表达试-----
	pStar=EXP_ProcessList(pStar,0);
	if(pLast)
	{
		pNext=pLast;
		while(pNext->pNext) pNext=pNext->pNext;
		pNext->pNext=pStar;
		pStar=pLast;
	}
	//TRACE( "EXP_StrALL End ... ");
	return pStar;
}
















/*
void Trace_PutMsg(EXP_UNIT* pUnit)
{
	while(pUnit)
	{
		if(pUnit->type==DATA_DEFINE_STR)
			TRACE( "TextPutMsg Result->STR[%s]",pUnit->pStr);
		if(pUnit->type==DATA_DEFINE_BUFF)
			TRACE( "TextPutMsg Result->BUFF[%B]",pUnit->pBuff,pUnit->Len);
		else if(pUnit->type==DATA_DEFINE_TAG)
			TRACE( "TextPutMsg Result[%x[%d]]",pUnit->Tag,pUnit->decimals);
		else if(pUnit->type==DATA_DEFINE_ASIC4)
			TRACE( "TextPutMsg Asi:%s",pUnit->Asi);
		else if(pUnit->type==DATA_DEFINE_RULES)
			TRACE( "TextPutMsg Result:%c%c",pUnit->pRule->ExpChars[0],pUnit->pRule->ExpChars[1]);
		else
			TRACE( "TextPutMsg pUnit->type[%d],decimals[%d],Result[%d]",pUnit->type,pUnit->decimals,pUnit->Result);
		pUnit=pUnit->pNext;
	}
	//TRACE("TextPutMsg End");
}


		
int APP_Test_EXP(char *p)
{
	EXP_UNIT* pUnit;
	u32		RetLen;
	if(p)
	{
		RetLen = API_strlen(p);
		pUnit=EXP_StrALL(&p,p+RetLen);
		Trace_PutMsg(pUnit);
		EXP_FreeUNIT(pUnit,0);
	}
	else
	{
		char	TestBuff[4096];
		TRACE( "APP_Test_EXP star:");
		while(1)
		{		
			RetLen=sizeof(TestBuff);
			//scanf_s("%s",TestBuff,sizeof(TestBuff));
			scanf("%s",TestBuff);
			//gets(TestBuff);
			RetLen=API_strlen(TestBuff);
			TRACE_HEX("APP_Test_EXP Recv:",(u8*)TestBuff,RetLen);
			if(RetLen>2)
			{
				p=TestBuff;
				pUnit=EXP_StrALL(&p,p+RetLen);
				Trace_PutMsg(pUnit);
				EXP_FreeUNIT(pUnit,0);
			}
			else break;
		}
		TRACE( "APP_Test_EXP End");
	}
	return 0;
}

*/


