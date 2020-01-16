//====================================================================
/*****************************表达式解析并运算******************************
//功能-------  脚本语言解析器基础，表达式运算
//作者-------  邓国祖
//创作时间---  20170106
******************************************************************************/
#include "comm.h"


EXP_UNIT* APP_GetTagMsg(EXP_UNIT* pUNIT)
{
	if(pUNIT->type == DATA_DEFINE_TAG)
	{
		DfTLV_TERM *pTLV;
		pTLV=APP_GetCurrent_TLV(pUNIT->Tag);
		if(pTLV)
		{
			pUNIT->pStr=(char*)malloc(pTLV->Len+1);
			API_memcpy(pUNIT->pStr,pTLV->val,pTLV->Len);
			pUNIT->pStr[pTLV->Len]='\0';
			pUNIT->type = DATA_DEFINE_STR;
		}
		else pUNIT->pStr=NULL;
	}
	return pUNIT;
}


//Fenexist(type) 检测指定type类型的环境（参见设定环境指令的参数值），当环境记录数大于0则为TRUE，否则为FALSE。函数运算结果值的属性为：BOOL
EXP_UNIT* EXP_Fenexist(EXP_UNIT* pUNIT)
{
	
	//--------填充结果------------------------
	pUNIT->type=DATA_DEFINE_BOOL;
	pUNIT->Result=1;
	pUNIT->pNext=NULL;
	return pUNIT;		
}

//Ffnrecnum(exp) 获取表达式指定的文件记录数，若环境指定的文件不存在，返回记录数为0。函数运算结果值的属性为：INT
EXP_UNIT* EXP_Ffnrecnum(EXP_UNIT* pUNIT)
{

	//--------填充结果------------------------
	pUNIT->type=DATA_DEFINE_INT;
	pUNIT->iData=1;
	pUNIT->pNext=NULL;
	return pUNIT;		
}
//Fenrecnum(type) 获取指定type类型的环境（参见设定环境指令的参数值）的记录数，若环境指定的文件不存在，返回记录数为0。函数运算结果值的属性为：INT
EXP_UNIT* EXP_Fenrecnum(EXP_UNIT* pUNIT)
{
	
	//--------填充结果------------------------
	pUNIT->type=DATA_DEFINE_INT;
	pUNIT->iData=1;
	pUNIT->pNext=NULL;
	return pUNIT;		
}
//Ffnexist(exp) 检测表达式指定的文件记录数大于0则为TRUE，否则为FALSE。函数运算结果值的属性为：BOOL
EXP_UNIT* EXP_Ffnexist(EXP_UNIT* pUNIT)
{
	
	//--------填充结果------------------------
	pUNIT->type=DATA_DEFINE_BOOL;
	pUNIT->Result=1;
	return pUNIT;		
}
//Fputsep(stype, dtype, exp, sep, fieldno, exp2) 将表达式exp2的值，以储类型为stype，定义类型为dtype，变长的对象放置到表达式exp值的fieldno指定的以sep分隔符分隔的子项位置，函数返回类型为exp的定义类型
EXP_UNIT* EXP_Fputsep(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fputvar(stype, dtype, exp,fieldno, exp2) 将表达式exp2的值，以储类型为stype，定义类型为dtype，变长的对象放置到表达式exp值的fieldno指定的LV变长子项位置,函数返回类型为exp的定义类型
EXP_UNIT* EXP_Fputvar(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fputfix(stype, dtype, exp, start, length, exp2) 将表达式exp2的值，以储类型为stype，定义类型为dtype，固定长度为length的对象放置到表达式exp值的下标start开始的位置，函数返回类型为exp的定义类型
EXP_UNIT* EXP_Fputfix(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fgetsep(stype, dtype, exp, sep, fieldno) Exp表达式值的子项采用sep分隔符串进行分隔，取fieldno子项为无名标签对象的值，，该标签存储类型为stype，定义类型为dtype，函数返回类型为dtype
EXP_UNIT* EXP_Fgetsep(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fgetvar(stype, dtype, exp,fieldno) exp表达式值的子项采用lv变长格式，取fieldno子项为无名标签对象的值，，该标签存储类型为stype，定义类型为dtype，函数返回类型为dtype
EXP_UNIT* EXP_Fgetvar(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fgetfix(stype, dtype, exp, start, length)取表达式值的从下标start开始长度为length的值为无名标签对象的值，该标签存储类型为stype，定义类型为dtype，函数返回类型为dtype
EXP_UNIT* EXP_Fgetfix(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}


//Freplace(exp1, start,len, exp2) 表达式exp1从start下标开始，长度len,  用表达式exp2去替换, 当len超过剩余有效数据的最大长度时，则表示后续串整个由exp2替换, 函数返回类型为exp1的定义类型
EXP_UNIT* EXP_Freplace(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fpmatch(exp1, exp2)部分匹配，exp1的值长度为L1,exp2的值长度L2，L1<=L2时，exp1值与exp2值前L1字节相同即为真；L1>L2时,exp1值前L2字节与exp2值相同即为真；其它为假。函数运算结果值的属性为：BOOL
EXP_UNIT* EXP_Fpmatch(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fconv(dtype, exp)强制类型转换，将exp的定义类型强制转换为dtype指定的定义，函数运算结果属性为：dtype定义类型指定的无名标签对象, dtype取值为：同数据字典定义类型的值
EXP_UNIT* EXP_Fconv(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fexist($tag) 检测当前数据来源环境中的标签是否存在，函数运算结果值的属性为：BOOL
EXP_UNIT* EXP_Fexist(EXP_UNIT* pInPar)
{
	return pInPar;		
}

//Flen(expression) 计算表达式值的长度，函数运算结果值的属性为：INT
EXP_UNIT* EXP_Flen(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fcardfmt(expression, left, right, type)left: 左边保留位数，right:右边保留位数，其它号码用*进行屏蔽；若left+right之和大于数据的有效长度,则表示不用屏蔽type: 1-每4位用空格分隔输出 2-直接连续输出
EXP_UNIT* EXP_Fcardfmt(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}


//Fgetdt(fmt) 按照’CCYYMMDDhhmmss’格式的子串取终端时钟
EXP_UNIT* EXP_Fgetdt(EXP_UNIT* pInPar)
{
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fsubstr(expression, start, length)取表达式值的子串
EXP_UNIT* EXP_Fsubstr(EXP_UNIT* pInPar)
{
	int i,start,length;
	if(pInPar==NULL || pInPar->pNext==NULL || pInPar->pNext->pNext==NULL) return NULL;
	start= pInPar->pNext->iData;
	length=pInPar->pNext->pNext->iData;
	EXP_FreeUNIT(pInPar->pNext,0);
	//--------填充结果------------------------
	pInPar->pNext=NULL;
	if(pInPar->type==DATA_DEFINE_TAG)
	{

	}
	else if(pInPar->type==DATA_DEFINE_STR)
	{
		for(i=0;i<length;i++)
		{
			pInPar->pStr[i]=pInPar->pStr[i+start];
		}
		pInPar->pStr[i]='\0';
	}
	return pInPar;		
}

//Fprintf(fmt, …) 按照C库函数printf的格式进行处理
EXP_UNIT* EXP_Fprintf(EXP_UNIT* pInPar)
{
	char *bp,*fmt;
	char c;
	char *p,*pOutStr;
	EXP_UNIT* pUNIT;
	//int i;

	int len;
	unsigned long long num;
	int base;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
						number of chars for from string */
	int qualifier;		/* 'h', 'l', 'L', 'Z' for integer fields */
	if(pInPar==NULL) return NULL;
	if(pInPar->type != DATA_DEFINE_STR) return NULL;

	
	fmt = pInPar->pStr;
	bp=fmt;
	pUNIT=pInPar->pNext;

	
	pOutStr=(char*)malloc(512);
	pInPar->pStr=pOutStr;

	while ( (c = *bp++) ) 
	{
		if (c != '%') 
		{
			*pOutStr++ =c;
			continue;
		}
		
		flags = 0; /* process flags */
		for( ; ; ++bp ) {	/* this also skips first '%' */
			switch (*bp) {
					case '-': flags |= LEFT; continue;
					case '+': flags |= PLUS; continue;
					case ' ': flags |= SPACE; continue;
					case '#': flags |= SPECIAL; continue;
					case '0': flags |= ZEROPAD; continue;
			}
			break;
		}

		/* get field width */
		field_width = -1;
		if((*bp)>='0' && (*bp)<='9')
			field_width = Exp_StrToNum(&bp,bp+20,10,NULL);	//bp will point to next character
		else if (*bp == '*') {
			++bp;
			/* it's the next argument */
			if(pUNIT==NULL) return NULL;
			field_width = pUNIT->iData;
			pUNIT=pUNIT->pNext;
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*bp == '.') {
			++bp;
			if((*bp)>='0' && (*bp)<='9')
				precision = Exp_StrToNum(&bp,bp+20,10,NULL); //bp will point to next character
			else if (*bp == '*') {
				++bp;
				/* it's the next argument */
				if(pUNIT==NULL) return NULL;
				precision = pUNIT->iData;
				pUNIT=pUNIT->pNext;
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*bp == 'h' || *bp == 'l' || *bp == 'L'/* || *bp =='Z'*/) {
			qualifier = *bp;
			++bp;
			if (qualifier == 'l' && *bp == 'l') {
				qualifier = 'L';	// long long type
				++bp;
			}
		}
		/* default base */
		base = 10;
		switch ((c = *bp++)) 
		{
			case 'c': /* %c - show a character */
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						*pOutStr++ =' ';
					}
				}
				if(pUNIT==NULL) return NULL;
				pUNIT=APP_GetTagMsg(pUNIT);
				*pOutStr++ = (char)pUNIT->uData&0xff;
				pUNIT=pUNIT->pNext;
				while (--field_width > 0) {
					*pOutStr++ =' ';
				}
				continue;
			case 's': /* %s - show a string */
				if(pUNIT==NULL) return NULL;
				pUNIT=APP_GetTagMsg(pUNIT);
				p = pUNIT->pStr;
				pUNIT=pUNIT->pNext;
				if (!p)
					p = "<NULL>";
				
				len = API_strlen(p);
				if(len>precision) len=precision;
				
				if (!(flags & LEFT)) {
					while (len < field_width--) {
						*pOutStr++ =' ';
					}
				}
				do {
					*pOutStr++ =*p++;
				} while (*p);
				while (len < field_width--) {
					*pOutStr++ =' ';
				}
				continue;
			
			case '%':/* %% - show a % character */
				*pOutStr++ ='%';
				continue;
				
			case 'o':
				flags |= SPECIAL;
			case 'O':
				base = 8;
				break;

			case 'p':
				flags |= SPECIAL;
				flags |= ZEROPAD;
			case 'X': /* %x - show a value in hex */
				flags |= LARGE;
			case 'x':
				base = 16;
				break;

			
			case 'b': /* %b - show a value in binary */
				flags |= SPECIAL;
			case 'B':
				base = 2;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u': /* %u - show an unsigned decimal value */
			case 'U':
				 break;
			case 'f': /* %f - show an unsigned decimal value */
			case 'F':
				{
					int Integer;
					s16 decimal,i; 
					char buff[16];
					
					if(pUNIT==NULL) return NULL;
					pUNIT=APP_GetTagMsg(pUNIT);
					Integer=pUNIT->iData;
					decimal=pUNIT->decimals;
					pUNIT=pUNIT->pNext;
					TRACE( "EXP_Fprintf Integer[%d],decimal[%d]",Integer,decimal);
					
					if(Integer<0 || decimal<0) 
					{
						Integer=-Integer;
						*pOutStr++ = '-';
					}
					if(Integer)
					{
						if(Integer<0) Integer = -Integer;
						i=sizeof(buff);
						while(Integer)
						{
							buff[--i]=Integer%10+'0';
							Integer /= 10;
						}
						for(;i<sizeof(buff);i++)
						{
							*pOutStr++ =buff[i];
						}
					}
					else
						*pOutStr++ = '0';
					
					if(decimal)
					{
						if(decimal<0) decimal = -decimal;
						i=sizeof(buff);
						while(decimal)
						{
							buff[--i]=decimal%10+'0';
							decimal /= 10;
						}
						*pOutStr++ ='.';
						i++;
						for(;i<sizeof(buff);i++)
						{
							*pOutStr++ =buff[i];
						}
					}
					else
					{
						*pOutStr++ = '.';
						*pOutStr++ = '0';
						*pOutStr++ = '0';
					}					
				}
				continue;
			default: /* %something else not handled ! */
				*pOutStr++ ='?';
				continue;
			}
		if(qualifier == 'L') {
			if(pUNIT==NULL) return NULL;
			num = pUNIT->uData; 			//long long
			pUNIT=pUNIT->pNext;
		} else if (qualifier == 'l') {
			if(pUNIT==NULL) return NULL;
			num = pUNIT->uData;
			pUNIT=pUNIT->pNext;
			if (flags & SIGN)
				num = (signed long) num;
		} else if (qualifier == 'h') {
			if(pUNIT==NULL) return NULL;
			num = pUNIT->uData&0xffff;
			pUNIT=pUNIT->pNext;
			if (flags & SIGN)
				num = (signed short) num;
		} else {
			if(pUNIT==NULL) return NULL;
			num = pUNIT->uData;
			pUNIT=pUNIT->pNext;
			if (flags & SIGN)
				num = (signed int) num;
		}
		pOutStr=EXP_NumToString(pOutStr,num, base, field_width, precision, flags);
	}
	*pOutStr='\0';
	free(fmt);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	return pInPar;
}
//Fgetoffsetl_v3(exp, offset) 
//exp表达式值中需要取出的子项采用变长V格式，
//取指定的offset偏移位置V子项为无名标签对象的值，
//变长子项V的长度=总长度L(exp前3字节，3字节ASCII码，前补0)-offset，
//该标签存储类型为(1-ans、a、s、an、as)，定义类型为(1-STR)，函数返回类型为(1-STR)
EXP_UNIT* EXP_Fgetoffsetl_v3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fputoffsetl_v3(exp1, offset, exp2)
//将表达式exp2的值，以储类型为(1-ans、a、s、an、as)，定义类型为(1-STR)，
//变长的V对象放置到表达式exp1值的指定的offset偏移位置，函数内部计算变长子项V的长度L，
//将长度offset+L(3字节ASCII码，前补0)保存到exp1的前3个字节，函数返回类型为exp1的定义类型，
//字符串总长度3+offset+L保存到固定标签
EXP_UNIT* EXP_Fputoffsetl_v3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fgettlv3(exp, offset, tag1, val1, tag2, val2, ...)
//exp表达式值从offset偏移位置开始的子项采用tlv变长格式(T固定2字节ASCII码、L固定3字节ASCII码、V值域为ASCII码)，
//取以tag1,tag2,...值为标签的标签对象的值保存到val1,val2,...，
//该标签存储类型为(1-ans、a、s、an、as)，定义类型为(1-STR)，函数返回类型为(1-STR)，最多支持10组
EXP_UNIT* EXP_Fgettlv3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fputtlv3(exp, offset, tag1, val1, tag2, val2, ...)
//将表达式val1,val2,...的值按tag1,tag2,...的值为标签，以储类型为(1-ans、a、s、an、as)，
//定义类型为(1-STR)，变长的对象TLV(T固定2字节ASCII码、L固定3字节ASCII码、V值域为ASCII码)
//保存到表达式exp的offset偏移位置，各子项的长度L在函数内部计算，函数返回类型为exp的定义类型，
//TLV串总长度保存到固定标签，最多支持10组
EXP_UNIT* EXP_Fputtlv3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fgetoffsetlv3(exp, offset)
//exp表达式值中需要取出的子项采用变长LV格式，取指定的offset偏移位置LV对象为无名标签对象的值，
//变长对象LV的长度L(offset后紧跟着的前3字节，3字节ASCII码，前补0)，该标签存储类型为(1-ans、a、s、an、as)，
//定义类型为(1-STR)，函数返回类型为(1-STR)
EXP_UNIT* EXP_Fgetoffsetlv3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//Fputoffsetlv3(exp1, offset, exp2)
//将表达式exp2的值，以储类型为(1-ans、a、s、an、as)，定义类型为(1-STR)，
//变长的LV对象放置到表达式exp1值的指定的offset偏移位置，函数内部计算变长对象LV的值V的长度L(3字节ASCII码，前补0)，
//保存到offset偏移位置后紧跟着的前3个字节，变长对象LV的值V保存到长度L之后，函数返回类型为exp1的定义
EXP_UNIT* EXP_Fputoffsetlv3(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//Fscanf(exp, fmt, ...)
//按照C库函数sscanf的格式进行处理，函数运算结果值的属性为：STR
EXP_UNIT* EXP_Fscanf(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//=================快速检索文件内部函数===============================
//FcreateFile(FileName, FileAttr, PerRecLen, MaxRecNum)	//创建快速检索文件函数
EXP_UNIT* EXP_FcreateFile(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//FcreateIdx(FileName，KeyTag，MaxIdxNum)	//创建索引函数
EXP_UNIT* EXP_FcreateIdx(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//FgetRecSum(FileName, TagList, Exp)	//获取记录总数函数
EXP_UNIT* EXP_FgetRecSum(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//FgetFileCtime(FileName, TimeFmt)	//获取快速检索文件创建时间函数
EXP_UNIT* EXP_FgetFileCtime(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//FgetFileMtime(FileName, TimeFmt)//获取快速检索文件修改时间函数
EXP_UNIT* EXP_FgetFileMtime(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}
//===========其他新增内部函数=============
//Fgetwk()	//获取当前星期，其结果集合为：{1,  2,  3….,  7} 属性为INT；
EXP_UNIT* EXP_Fgetwk(EXP_UNIT* pInPar)
{
	return pInPar;		
}


//Fconvwk(type，value)：转换星期到指定值，type属性为INT 值集合为：{1(汉字), 2(英文)}; value
EXP_UNIT* EXP_Fconvwk(EXP_UNIT* pInPar)
{
	return pInPar;		
}
//=====检查相关指令是否存在的函数============
//Fcmdexist( type,  value): type属性为INT 值集合为 {1(应用指令),  2(打印子指令),  3(显示子指令)} ， value属性为INT （指令号）
EXP_UNIT* EXP_Fcmdexist(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//=========检查指定内部函数是否存在的函数=======
//Ffuncexist(name): name 为函数名 其属性为STR；函数运算结果值的属性为：BOOL 若检查函数存在为真，反之为假
EXP_UNIT* EXP_Ffuncexist(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//=====计算从起始时间之后间隔一定时间的时间点======
//FcalTime(Time, Interval, Mode)
/*
计算从起始时间开始，经过间隔时间后的时间点
Time：
	描述：起始时间，格式为‘CCYYMMDDhhmmss’
	参数类型：STR
Interval：
描述：时间间隔，需要累加的时间，可为秒、分、时、天、月、年
参数类型：INT
Mode：
描述：标记累加的单位，分别代表秒、分、时、天、月、年；可为“YY”“MM”“DD”“hh”“mm”“ss”，如果模式错误或者为NULL，则默认为“ss”
参数类型：STR
返回值属性：STR
*/
EXP_UNIT* EXP_FcalTime(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//========计算两个时间点之间的间隔时间==========
//FcalInterval (BeginTime, EndTime, Mode)
/*
计算结束时间减去起始时间的间隔，结束时间应大于起始时间
BeginTime：
	描述：起始时间，格式为‘CCYYMMDDhhmmss’
	参数类型：STR
EndTime：
	描述：起始时间，格式为‘CCYYMMDDhhmmss’
	参数类型：STR
Mode：
描述：间隔的单位，分别代表秒、分、时、天、月、年；可为“YY”“MM”“DD”“hh”“mm”“ss”，如果模式错误或者为NULL，则默认为“ss”
参数类型：STR
		返回值属性：INT
*/
EXP_UNIT* EXP_FcalInterval(EXP_UNIT* pInPar)
{
	
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//模板标签相关内部函数========模板标签：模板标签用于存储多个标签的数据，其值由多个标签TLV串拼接而成。
//FputTLV(Tag, TagListID, DataSource)
/*
	将TagListID指定的标签集合组成TLV串，并将该串作为参数Tag指定的标签的值。
	Tag：
		描述：目的模板标签TAG，该标签存储在当前数据空间中
		参数类型：STR，最长8字节
	TagListID：
		描述：标签列表ID，该ID对应的标签列表中的所有标签按照TLV格式拼接成串
		参数类型：STR， 2个字节长度的ID
	DataSource：
描述：数据来源。
参数类型：INT  ，1-交易上下文环境；2-历史交易环境；
 
返回值：INT，0-成功，1失败
*/
EXP_UNIT* EXP_FputTLV(EXP_UNIT* pInPar)
{
	return pInPar;		
}

//FgetTLV(Tag, DataDest)
/*
将Tag指定的模板标签的值按照TLV串格式解开，解开的标签放到指定的数据空间中。
Tag：
	描述：源模板标签TAG，该标签来源于交易上下文环境
	参数类型：STR,最长8字节
DataDest：
	描述：目的数据空间
	参数类型： INT, 1-当前交易数据空间 3-环境变量数据空间4-历史交易数据空间
	
返回值：INT， 0-成功 1失败
	注：若源模板标签不存在，返回成功，不往目的数据空间写入数据。
	脚本例子：
	<OP CMD='61' CP='FgetTLV(&apos;FF9140&apos;, 1)' PS='动态运算-解包模板标签'/> //将标签FF9140的值按照TLV串格式接口，解开的标签放到当前交易数据空间
*/
EXP_UNIT* EXP_FgetTLV(EXP_UNIT* pInPar)
{

	return pInPar;		
}

//FputLV(Tag, TagListID, DataSource, Oper)
/*
将TagListID指定的标签集合组成TLV串，并将该串以LV的格式以增量方式作为参数Tag指定的标签的值。
Tag：
		描述：目的容器标签TAG，该标签存储在当前数据空间中
		参数类型：STR，最长8字节
	TagListID：
		描述：标签列表ID，该ID对应的标签列表中的所有标签按照TLV格式拼接成串再组成LV格式
		参数类型：STR， 2个字节长度的ID
	DataSource：
描述：数据来源。
参数类型：INT  ，1-交易上下文环境；2-历史交易环境；

Oper：
描述：操作方法
参数类型：INT， 0-新增（需要先清空标签原有的数据） 1-追加
 
返回值：INT，0-成功，1失败
*/
EXP_UNIT* EXP_FputLV(EXP_UNIT* pInPar)
{
	
	return pInPar;		
}

//FgetLV(Tag,  RecNo，DataDest)
/*
按照参数RecNo指定的记录号，将Tag指定的容器标签的值按解开，解开的标签放到指定的数据空间中。
Tag：
	描述：源容器标签TAG，该标签来源于交易上下文环境
	参数类型：STR,最长8字节
RecNo：
	描述：容器标签中的记录号，第一条记录的记录号为1
	参数类型：INT
DataDest：
	描述：目的数据空间
	参数类型： INT, 1-当前交易数据空间 3-环境变量数据空间4-历史交易数据空间
	
返回值：INT， 0-成功 1失败  2-记录不存在
*/
EXP_UNIT* EXP_FgetLV(EXP_UNIT* pInPar)
{
	
	return pInPar;		
}


//FshowMsg(char* pTitle,char* pOk,char* pCancel,char* Msg,int timeOut)
//=====显示测试==================================
EXP_UNIT* EXP_FshowMsg(EXP_UNIT* pInPar)
{
	//int CurrMS,MaxMs;
	EXP_UNIT* pOk;
	EXP_UNIT* pCancel;
	EXP_UNIT* pMsg;
//	EXP_UNIT* pTimeOut;
	pOk		=pInPar->pNext;
	pCancel	=pOk->pNext;
	pMsg	=pCancel->pNext;
//	pTimeOut=pMsg->pNext;
	TRACE(pInPar->pStr);
	TRACE(pOk->pStr);
	TRACE(pCancel->pStr);
	TRACE(pMsg->pStr);
	//pTimeOut->iData;
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	//--------填充结果------------------------
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}


EXP_UNIT* Exp_Beep(EXP_UNIT* pInPar)
{
	//const u16 tFreg[6]={1000,1500,2000,3000,4000,5000};
	int Max;
	EXP_UNIT* pFregIndex;
	EXP_UNIT* pTimeOut;
	EXP_UNIT* pTimes;
//	EXP_UNIT* pInterval;
	pFregIndex	=pInPar;
	pTimeOut	=pFregIndex->pNext;
	pTimes		=pTimeOut->pNext;
//	pInterval	=pTimes->pNext;


	Max=pTimes->iData;
	while(Max--)
	{
		//API_Beep(pFregIndex->iData,pTimeOut->iData);
		//if(Max) //最后一次不延时
		//	API_TimeSleepMS(pInterval->iData);	
	}
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}


EXP_UNIT* EXP_Ftree(EXP_UNIT* pInPar)
{
	if(pInPar!=NULL && pInPar->iData == 1)
		TreeDemoNode1();
	else
		TreeDemoNode2();
	//--------填充结果------------------------
	//pInPar->type=DATA_DEFINE_BOOL;
	//pInPar->Result=1;
	return pInPar;		
}

EXP_UNIT* EXP_Fsave(EXP_UNIT* pInPar)
{//Tag,Data
	DfTLV_TERM *pTLV;
	EXP_UNIT* pData;
	pData	=pInPar->pNext;
	pTLV=(DfTLV_TERM*)malloc(sizeof(DfTLV_TERM)+pData->Len);
	pTLV->Tag=pInPar->Tag;
	pTLV->Len=pData->Len;
	API_memcpy(pTLV->val,pData->pStr,pTLV->Len);
	TRACE("Fsave Tag[%X],Len[%d]", pTLV->Tag, pTLV->Len);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=APP_SetCurrent_TLV(pTLV);
	free(pTLV);
	return pInPar;		
}

EXP_UNIT* EXP_Fread(EXP_UNIT* pInPar)
{//Tag,Data
	DfTLV_TERM *pTLV;
	pTLV=APP_GetCurrent_TLV(pInPar->Tag);
	//--------填充结果------------------------
	while(pTLV)
	{
		if(pInPar->decimals<0) 
			pInPar->decimals=0;
		if(pInPar->decimals >= pTLV->Len)
			break;
		
		pInPar->Len = pTLV->Len-pInPar->decimals;
		pInPar->pStr=(char*)malloc(pInPar->Len+1);
		API_memcpy(pInPar->pStr,pTLV->val + pInPar->decimals,pInPar->Len);
		pInPar->pStr[pInPar->Len]='\0';
		pInPar->decimals=0;
		pInPar->type=DATA_DEFINE_STR;
		TRACE("EXP_Fread[%X],Len[%d]",pTLV->Tag,pInPar->Len);
		return pInPar;		
	}
	pInPar->type=DATA_DEFINE_STR;
	pInPar->pStr=NULL;
	pInPar->Len=0;
	TRACE("EXP_Fread[%X],NULL",pInPar->Tag);
	return pInPar;		
}


EXP_UNIT* EXP_Fb64dec(EXP_UNIT* pInPar)
{//Tag,Data
	char *pOut;
	size_t OutLen;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	//TRACE("EXP_Fb64dec[%B]",pInPar->pStr,pInPar->Len);
	OutLen=pInPar->Len*2;
	pOut=(char*)malloc(OutLen);
	base64_decode((u8*)pOut,&OutLen,(u8*)pInPar->pBuff,pInPar->Len);
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BUFF;
	free(pInPar->pStr);
	pInPar->pStr=pOut;
	pInPar->Len=OutLen;
	return pInPar;		
}

EXP_UNIT* EXP_Fb64enc(EXP_UNIT* pInPar)
{//Tag,Data
	char *pOut;
	size_t OutLen;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	TRACE("EXP_Fb64enc[%B]",pInPar->pStr,pInPar->Len);
	OutLen=pInPar->Len*2;
	pOut=(char*)malloc(OutLen);
	base64_encode((u8*)pOut,&OutLen,pInPar->pBuff,pInPar->Len);
	//--------填充结果------------------------
	free(pInPar->pStr);
	pInPar->pStr=pOut;
	pInPar->Len=OutLen;
	return pInPar;		
}


EXP_UNIT* UrlDecode(EXP_UNIT* pInPar)
{
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	if(pInPar->Len)
	{
		char *pIn,*pEnd,*pOut;
		u32 j=0;
		pIn=pInPar->pStr;
		pEnd=pIn+pInPar->Len;
		pOut=(char*)malloc(pInPar->Len);
		while(pIn < pEnd)
		{
			if(*pIn != '%')
				pOut[j++]=*pIn++;
			else 
			{
				u8 aH,aL;
				pIn++;
				aH=*pIn++;
				aL=*pIn++;
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
			}
		}
		pOut[j]='\0';
		free(pInPar->pStr);
		pInPar->pStr=pOut;
		pInPar->Len=j;
	}
	return pInPar;
}

EXP_UNIT* UrlEncode(EXP_UNIT* pInPar)
{
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	if(pInPar->Len)
	{
		char *pIn,*pEnd,*pOut;
		u32 j=0;
		pIn=pInPar->pStr;
		pEnd=pIn+pInPar->Len;
		pOut=(char*)malloc(pInPar->Len*3);
		while(pIn < pEnd)
		{
			if(*pIn == '+')
			{
				pOut[j++]='%';
				pOut[j++]='2';
				pOut[j++]='B';
			}
			else if(*pIn == '/')
			{
				pOut[j++]='%';
				pOut[j++]='2';
				pOut[j++]='F';
			}
			else 
			{
				pOut[j++] = *pIn;
			}
			pIn++;
		}
		pOut[j]='\0';
		free(pInPar->pStr);
		pInPar->pStr=pOut;
		pInPar->Len=j;
	}
	return pInPar;
}

EXP_UNIT* EXP_FBcdToStr(EXP_UNIT* pInPar)
{//Tag,Data
	char *pOut;
	u16 OutLen;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	OutLen=pInPar->Len*2+1;
	pOut=(char*)malloc(OutLen);
	APP_Conv_BcdToStr(pInPar->pBuff,pInPar->Len,pOut);
	free(pInPar->pBuff);
	pInPar->pStr=pOut;
	pInPar->Len=OutLen;
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_STR;
	return pInPar;		
}

EXP_UNIT* EXP_FStrToBcd(EXP_UNIT* pInPar)
{//Tag,Data
	u8 *pOut;
	u16 OutLen;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	OutLen=pInPar->Len/2;
	pOut=(u8*)malloc(OutLen);
	pInPar->Len=APP_Conv_StrToBcd(pInPar->pStr,pInPar->Len,pOut);
	free(pInPar->pStr);
	pInPar->pBuff=pOut;
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BUFF;
	return pInPar;		
}

EXP_UNIT* EXP_Fscpy(EXP_UNIT* pInPar)
{//Tag,Data
	char *pOut;
	u16 OutLen;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
		if(pInPar->decimals!=0 && pInPar->decimals<pInPar->Len)
		{
			OutLen=pInPar->Len-pInPar->decimals;
			pOut=(char*)malloc(OutLen+1);
			API_memcpy(pOut,pInPar->pStr+pInPar->decimals,OutLen+1);
			free(pInPar->pStr);
			pInPar->pStr=pOut;
			pInPar->Len=OutLen;
		}
	}
	else if(pInPar->type==DATA_DEFINE_BUFF || pInPar->type==DATA_DEFINE_STR)
	{
		OutLen=pInPar->pNext->uData;
		if(OutLen!=0 && OutLen<pInPar->Len)
		{
			pOut=(char*)malloc(pInPar->Len- OutLen +1);
			API_memcpy(pOut,pInPar->pStr+ OutLen,pInPar->Len- OutLen +1);
			free(pInPar->pStr);
			pInPar->pStr=pOut;
			pInPar->Len=pInPar->Len- OutLen;
			EXP_FreeUNIT(pInPar->pNext,0);
			pInPar->pNext=NULL;
		}
	}
	TRACE_HEX("EXP_Fscpy:",pInPar->pBuff,pInPar->Len);
	return pInPar;		
}

//=======Name,pData=================			
EXP_UNIT* EXP_FOutf(EXP_UNIT* pInPar)
{//Tag,Data
	EXP_UNIT *pData;
	size_t OutLen;
	FILE *f;
	//errno_t err;	
	pData	=pInPar->pNext;
	
	TRACE("Out fopen_s->%s\r\n", pInPar->pStr);
	//err = fopen_s(&f, pInPar->pStr, "wb"); //(const char*)name
	f = fopen(pInPar->pStr, "wb"); // linux 对应用法
	if (f == NULL)
	{
		TRACE("打开[%s]文件失败", pInPar->pStr);
	}
	else
	{
		if(pData->type == DATA_DEFINE_TAG)
		{
			pData=EXP_Fread(pData);
		}
		OutLen=fwrite(pData->pBuff,pData->Len, 1, f);
		fclose(f);
		TRACE("EXP_FOutf[%s]Len[%d]",pInPar->pStr,OutLen);
	}
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	free(pInPar->pStr);
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=TRUE;
	return pInPar;		
}

EXP_UNIT* EXP_Finf(EXP_UNIT* pInPar)
{//Tag,Data
	size_t OutLen;
	FILE *f;
//	errno_t err;	11
	TRACE("In fopen_s->%s\r\n", pInPar->pStr);
	//err = fopen_s(&f, pInPar->pStr, "rb"); //(const char*)name
	f = fopen(pInPar->pStr, "rb"); // linux 
	if (f == NULL)
	{
		TRACE("打开[%s]文件失败", pInPar->pStr);
		return NULL;
	}
	free(pInPar->pStr);
	fseek(f, 0, SEEK_END);
	OutLen = ftell(f);
	if(OutLen!=0)
	{
		pInPar->pBuff=(u8*)malloc(OutLen);
		fseek(f, 0, SEEK_SET);
		fread(pInPar->pBuff,OutLen, 1, f);
		fclose(f);
		pInPar->Len=OutLen;
	}
	else
	{
		pInPar->pStr=NULL;
		TRACE("ReadLen =0 Err");
	}
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	//--------填充结果------------------------
	pInPar->type=DATA_DEFINE_BUFF;
	return pInPar;		
}



EXP_UNIT* EXP_FDes(EXP_UNIT* pInPar)
{//Tag,Data
	EXP_UNIT* pKey;
	u8 Key[32];
	u16 KeyLen;
	pKey=pInPar->pNext;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	KeyLen=pKey->Len/2;
	if((KeyLen%8)==0)
	{
		APP_Conv_StrToBcd(pKey->pStr,pKey->Len,Key);
		if(KeyLen==8)
		{
			Des_B1Dec(pInPar->pBuff,Key,pInPar->Len/8);
		}
		else if(KeyLen==16)
		{
			Des_B2Dec(pInPar->pBuff,Key,pInPar->Len/8);
		}
		else if(KeyLen==24)
		{
			Des_B3Dec(pInPar->pBuff,Key,pInPar->Len/8);
		}
	}
	TRACE_HEX("EXP_FDes:",pInPar->pBuff,pInPar->Len);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	return pInPar;		
}

EXP_UNIT* EXP_FEnc(EXP_UNIT* pInPar)
{//Tag,Data
	EXP_UNIT* pKey;
	u8 Key[32];
	u16 KeyLen;
	pKey=pInPar->pNext;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	KeyLen=pKey->Len/2;
	if((KeyLen%8)==0)
	{
		APP_Conv_StrToBcd(pKey->pStr,pKey->Len,Key);
		if(KeyLen==8)
		{
			Des_B1Enc(pInPar->pBuff,Key,pInPar->Len/8);
		}
		else if(KeyLen==16)
		{
			Des_B2Enc(pInPar->pBuff,Key,pInPar->Len/8);
		}
		else if(KeyLen==24)
		{
			Des_B3Enc(pInPar->pBuff,Key,pInPar->Len/8);
		}
	}
	TRACE_HEX("EXP_FEnc:",pInPar->pBuff,pInPar->Len);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	return pInPar;		
}


EXP_UNIT* EXP_FSm4Des(EXP_UNIT* pInPar)
{//Tag,Data
	EXP_UNIT* pKey;
	u8 Sm4Key[16];
	u16 KeyLen;
	pKey=pInPar->pNext;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	APP_Conv_StrToBcd(pKey->pStr,pKey->Len,Sm4Key);
	KeyLen=pKey->Len/2;
	TRACE_HEX("FSm4Des_SmKey:",Sm4Key,KeyLen);
	TRACE_HEX("FSm4Des_IBuff:",pInPar->pBuff,pInPar->Len);
	SM4_DEC(pInPar->pBuff,Sm4Key,pInPar->Len/16);
	TRACE_HEX("FSm4Des_OBuff:",pInPar->pBuff,pInPar->Len);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	return pInPar;		
}

EXP_UNIT* EXP_FSm4Enc(EXP_UNIT* pInPar)
{//Tag,Data
	EXP_UNIT* pKey;
	u16 KeyLen;
	u8 Sm4Key[16];
	
	pKey=pInPar->pNext;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	APP_Conv_StrToBcd(pKey->pStr,pKey->Len,Sm4Key);
	KeyLen=pKey->Len/2;
	TRACE_HEX("Sm4Enc_SmKey:",Sm4Key,KeyLen);
	TRACE_HEX("Sm4Enc_IBuff:",pInPar->pBuff,pInPar->Len);
	SM4_ENC(pInPar->pBuff,Sm4Key,pInPar->Len/16);
	TRACE_HEX("Sm4Enc_OBuff:",pInPar->pBuff,pInPar->Len);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	return pInPar;		
}

//============授权计算========================================
void gDynamicPasswordCheck(u8* sRand,u8 ComIndex,u8 BaudIndex,u16 OperatorIndex,u16 DebugTime,char* pOutData)
{
	u8		bBuff[8]={0},PassData[16],i,hsl,lsl;
	char	sNumH[12],sNumL[12];
	u32		NumL,NumH,sLen,Developer;
	TRACE_HEX("bBuff1",sRand,4);
	//----------产生随机密钥-----------------
	PassData[0]=sRand[0]^sRand[2];
	PassData[1]=sRand[1]^sRand[3];
	for(i=1;i<15;i++)
	{
		PassData[i+1]=PassData[i]+PassData[i-1];
	}
	//------------------------------------------------------
	Developer =(ComIndex<<12)|DebugTime;
	Developer <<= 16;
	Developer |=(BaudIndex<<12)|OperatorIndex;
	i=4;
	//----后4字节数据------
	while(i--)
	{
		bBuff[4+i]=Developer&0xff;
		Developer/=256;
	}
	//----前4字节数据------
	memcpy(bBuff,sRand,4);
	//TRACE_HEX("gDynamicPasswordCheck->bBuff",bBuff,8);
	Des_B2Enc(bBuff,PassData,1);
	//TRACE_HEX("gDynamicPasswordCheck->B2Enc",bBuff,8);
	//-----lenH+DataH+dataL----转成8字节密文数据-----------
	for(NumH=0,NumL=0,i=0;i<4;i++)
	{	
		NumH *= 256;
		NumH += bBuff[i];
		
		NumL *= 256;
		NumL += bBuff[4+i];
	}
	
	hsl=sizeof(sNumH);
	while(NumH)
	{
		sNumH[--hsl]='0'|NumH%10;
		NumH /= 10;
	}
	lsl=sizeof(sNumL);
	while(NumL)
	{
		sNumL[--lsl]='0'|NumL%10;
		NumL /= 10;
	}
	//TRACE_HEX("sNumH",sNumH+hsl,sizeof(sNumH)-hsl);
	//TRACE_HEX("sNumL",sNumL+lsl,sizeof(sNumL)-lsl);
	sLen=sizeof(sNumH)-hsl;
	if(sLen>9) sLen=0;
	pOutData[0]='0'|sLen;
	sLen=sizeof(sNumH)-hsl;
	memcpy(pOutData+1,sNumH+hsl,sLen);
	memcpy(pOutData+1+sLen,sNumL+lsl,sizeof(sNumL)-lsl);
	sLen += 1+sizeof(sNumL)-lsl;
	pOutData[sLen]='\0';
	TRACE_HEX("pOutData",(u8*)pOutData,23);
}

u32 gDynamicPasswordDes(u8* sRand,char* sBuff)
{
	u8		bBuff[8]={0},PassData[16],i,LenL,LenH;	
	u32		NumL,NumH,ReNum=0;
	TRACE_HEX("bBuff1",sRand,4);
	//----------产生随机密钥-----------------
	PassData[0]=sRand[0]^sRand[2];
	PassData[1]=sRand[1]^sRand[3];
	for(i=1;i<15;i++)
	{
		PassData[i+1]=PassData[i]+PassData[i-1];
	}
	//-----lenH+DataH+dataL----转成8字节密文数据-----------
	LenH= ((u8)sBuff[0])&0x0F;	
	if(LenH==0) LenH=10;//取高位长度
	LenL=API_strlen(sBuff+1)&0xff; 
	if(LenL < LenH) return 0;
	LenL -= LenH;
	for(NumH=0,i=0;i<LenH;i++)
	{	
		NumH *= 10;
		NumH += sBuff[1+i]&0x0f;
	}	
	for(NumL=0;i<(LenL+LenH);i++)
	{	
		NumL *= 10;
		NumL += sBuff[1+ i]&0x0f;
	}	
	for(i=0;i<4;i++)
	{
		bBuff[7-i]=NumL%256;
		NumL /= 256;
		bBuff[3-i]=NumH%256;
		NumH /= 256;
	}
	//TRACE_HEX("gDynamicPasswordDes->bBuff",bBuff,8);
	//--------------------------------------------
	Des_B2Dec(bBuff,PassData,1);
	//TRACE_HEX("gDynamicPasswordDes->B2Dec",bBuff,8);
	if(API_memcmp(bBuff,sRand,4)==0)
	{
		for(i=0;i<4;i++)
		{
			ReNum *= 256;
			ReNum += bBuff[i+4];
		}
		return ReNum;
	}
	return 0;
}

EXP_UNIT* EXP_GetDeveloper(EXP_UNIT* pInPar)
{//Tag,Data
	//EXP_UNIT* pKey;
	u16 timeOut=3;//KeyLen,
	char OutStr[24]={0};
	u8 RanData[8];
	if(pInPar->pNext)
		timeOut=pInPar->pNext->iData;
	APP_Conv_StrToBcd(pInPar->pStr,pInPar->Len,RanData);
	gDynamicPasswordCheck(RanData,0,0,1,timeOut,OutStr);
	TRACE("gDynamicPasswordDes[%08X]",gDynamicPasswordDes(RanData,OutStr));
	
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	free(pInPar->pStr);
	pInPar->pStr=(char*)malloc(strlen(OutStr+1)+2);
	strcpy(pInPar->pStr,OutStr);
	return pInPar;		
}


LOTTERY_DATA_Link *pLotteryLink=NULL;
//=====Text================
/*
Name:20161026
Text:
20161026
12345
23456
12344
.....
*/
EXP_UNIT* EXP_LoadLotteryLink(EXP_UNIT* pInPar)
{
	char *pOffset,*pEnd;
	u32 iPeriod,uDataNum;
	LOTTERY_DATA_Link* pInDataLink;
	pInDataLink=(LOTTERY_DATA_Link *)malloc(sizeof(LOTTERY_DATA_Link));
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	TRACE("EXP_LoadLotteryLink Inlen[%d]", pInPar->Len);
	pOffset=pInPar->pStr;
	pEnd = pOffset + pInPar->Len;
	if (*pOffset != '2')
	{
		return pInPar;
	}
	uDataNum=APP_StrToNum(pOffset,8,10);
	pOffset += 8;
	while(*pOffset)
	{
		if(*pOffset++ == '\n') break;
	}
	//--------------------------------
	iPeriod=0;
	pInDataLink->Data=uDataNum;
	pInDataLink->pNext=NULL;
	while(pOffset < pEnd)
	{
		if(*pOffset >='0' && *pOffset <= '9')
		{
			pInDataLink->lotteryUnit[iPeriod].nInt = APP_StrToNum(pOffset,5,10);
			API_memcpy(pInDataLink->lotteryUnit[iPeriod].nChar,pOffset,5);
			pOffset += 5;
			iPeriod++;
		}
		
		while(pOffset < pEnd)
		{
			if(*pOffset++ == '\n') break;
		}
	}
	pInDataLink->total=iPeriod;
	//-------------------------------------------
	TRACE("成功获取[%d]条数据",iPeriod);
	if(pLotteryLink==NULL)
		pLotteryLink=pInDataLink;
	else
	{
		LOTTERY_DATA_Link* pNext=pLotteryLink;
		while(pNext->pNext)
		{
			pNext=pNext->pNext;
		}
		pNext->pNext=pInDataLink;
	}
	//--------填充结果------------------------
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}


//=====cmd:"20161026 001 12345"===============
EXP_UNIT* EXP_LoadLotteryData(EXP_UNIT* pInPar)
{
	char sBuff[7];
	u8 iPeriod;//,uPeriod
	u32 uNumBer,uDataNum;
	LOTTERY_DATA_Link* pInDataLink;	
	TRACE("导入字符[%B]", pInPar->pStr,pInPar->Len);

	if(pInPar->Len > 14)
	{
		uDataNum=APP_StrToNum(pInPar->pStr,8,10);
		iPeriod=APP_StrToNum(pInPar->pStr+8+1,3,10);
		API_memcpy(sBuff,pInPar->pStr+8+1+3+1,5);
		uNumBer=APP_StrToNum(sBuff,5,10);
		//---------------------------------------
		pInDataLink=pLotteryLink;
		while(pInDataLink)
		{
			if(pInDataLink->Data == uDataNum)
				break;
			pInDataLink=pInDataLink->pNext;
		}
		if(pInDataLink==NULL)
		{
			pInDataLink=(LOTTERY_DATA_Link *)malloc(sizeof(LOTTERY_DATA_Link));
			API_memset(pInDataLink,0x00,sizeof(LOTTERY_DATA_Link));
		}
		
		API_memcpy(pInDataLink->lotteryUnit[iPeriod-1].nChar,sBuff,5);
		pInDataLink->lotteryUnit[iPeriod-1].nInt = uNumBer;
		if(iPeriod > pInDataLink->total)
			pInDataLink->total=iPeriod;
	}
	TRACE("成功导入第[%d][%03d]条[%05d]数据", uDataNum,iPeriod,uNumBer);
	//--------填充结果------------------------
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;		
}

//=====cmd:"20161026"=or=cmd:NULL =============
EXP_UNIT* EXP_GetLotteryBit(EXP_UNIT* pInPar)
{
	char *pBit;
	u32 iPeriod,MaxLink,uDataNum;
	LOTTERY_DATA_Link* pInDataLink;	
	if(pInPar)
	{
		TRACE("导入字符[%s]", pInPar->pStr);
		uDataNum=APP_StrToNum(pInPar->pStr,8,10);
		pInDataLink=pLotteryLink;
		while(pInDataLink)
		{
			if(pInDataLink->Data == uDataNum)
				break;
			pInDataLink=pInDataLink->pNext;
		}
		if(pInDataLink==NULL)
		{
			free(pInPar->pStr);
			pInPar->type=DATA_DEFINE_ASIC4;
			API_memcpy(pInPar->Asi,"Nul",sizeof(pInPar->Asi));
			return pInPar;		
		}
		free(pInPar->pStr);
		pInPar->pStr=(char*)malloc(2048);
		sprintf(pInPar->pStr,"Data[%d]total[%03d]\n",pInDataLink->Data,pInDataLink->total);
		pBit=pInPar->pStr+API_strlen(pInPar->pStr);
		for(iPeriod=0;iPeriod<pInDataLink->total;iPeriod++)
			pBit=APP_GetBitBuff(pInDataLink->lotteryUnit[iPeriod].nInt,0x20000,pBit);
		*pBit++='\n';
		*pBit++='\0';	
		pInPar->Len=pBit-pInPar->pStr;
	}
	else
	{
		pInDataLink=pLotteryLink;
		MaxLink=0;
		while(pInDataLink)
		{
			pInDataLink=pInDataLink->pNext;
			MaxLink ++;
		}

		pInPar=(EXP_UNIT* )malloc(sizeof(EXP_UNIT));
		API_memset(pInPar,0x00,sizeof(EXP_UNIT));
		pInPar->type=DATA_DEFINE_STR;
		pInPar->pStr=(char*)malloc(2048*MaxLink);

		pBit=pInPar->pStr;
		pInDataLink=pLotteryLink;
		while(MaxLink--)
		{
			sprintf(pBit,"Data[%d]total[%03d]\n",pInDataLink->Data,pInDataLink->total);
			pBit += API_strlen(pBit);
			for(iPeriod=0;iPeriod<pInDataLink->total;iPeriod++)
				pBit=APP_GetBitBuff(pInDataLink->lotteryUnit[iPeriod].nInt,0x20000,pBit);
			*pBit++='\n';
			
			pInDataLink = pInDataLink->pNext;
		}
		pInPar->Len=pBit-pInPar->pStr;
	}
	return pInPar;
}

EXP_UNIT* EXP_GetLotteryXorBit(EXP_UNIT* pInPar)
{
	u32 iPeriod,MaxLink,uDataNum;
	LOTTERY_DATA_Link* pInDataLink;	
	pInDataLink=pLotteryLink;
	MaxLink=0;
	while(pInDataLink)
	{
		pInDataLink=pInDataLink->pNext;
		MaxLink ++;
	}
	
	pInDataLink=pLotteryLink;
	while(MaxLink--)
	{
		uDataNum=0;
		for(iPeriod=0;iPeriod< pInDataLink->total;iPeriod++)
		{
			uDataNum ^= pInDataLink->lotteryUnit[iPeriod].nInt;
			if((uDataNum & pInPar->uData)==0x00)
			{
				TRACE("LotteryXorBit iPeriod[%d] uDataNum[%x]",iPeriod,uDataNum);
			}
		}
		TRACE("EXP_GetLotteryXorBit...");
		pInDataLink = pInDataLink->pNext;
	}
	return pInPar;
}



//===========数据,起点,控制类=====================
EXP_UNIT* EXP_CheckXorBuff(EXP_UNIT* pInPar)
{
	char* pOutMsg;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	TRACE("EXP_CheckXorBuff Len[%d]",pInPar->Len);
	pOutMsg=APP_BitXorBuff_New(pInPar->pStr,pInPar->Len,pInPar->pNext->uData,pInPar->pNext->pNext->uData);
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	free(pInPar->pStr);
	pInPar->pStr=pOutMsg;
	pInPar->Len=API_strlen(pOutMsg);
	return pInPar;	
}

//===========数据,起点=====================
EXP_UNIT* EXP_CheckXorALL(EXP_UNIT* pInPar)
{
	char* pOutMsg,*pOffset;
	u16 bit1,bit2;
	u16 oneTimes,zeroTimes;
	if(pInPar->type==DATA_DEFINE_TAG)
	{
		pInPar=EXP_Fread(pInPar);
	}
	TRACE("EXP_CheckXorALL Len[%d]",pInPar->Len);
	for(bit1=0x100; bit1<0x1300 ; bit1 += 0x100)
	{
		for(bit2=(bit1>>8)+1; bit2<0x13 ; bit2++)
		{
			pOutMsg=APP_BitXorBuff_New(pInPar->pStr,pInPar->Len,pInPar->pNext->uData,0x14010000|bit1|bit2);
			pOffset=pOutMsg;
			oneTimes=0;
			zeroTimes=0;
			while(*pOffset)
			{
				if(*pOffset=='1')
					oneTimes++;
				else if(*pOffset=='0')
					zeroTimes++;
				pOffset++;
			}
			TRACE("bit1[%d]bit2[%d]oneTimes[%d]zeroTimes[%d]\n[%s]",bit1>>8,bit2,oneTimes,zeroTimes,pOutMsg);
			free(pOutMsg);
		}
	}
	EXP_FreeUNIT(pInPar->pNext,0);
	pInPar->pNext=NULL;
	free(pInPar->pStr);
	pInPar->type=DATA_DEFINE_BOOL;
	pInPar->Result=1;
	return pInPar;	
}

extern EXP_UNIT* EXP_TreeFileInit(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_TreeFileRun(EXP_UNIT* pInPar);

extern EXP_UNIT* EXP_FileSysInit(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysDeinit(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysOpen(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysWrite(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysRead(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysSeek(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysClose(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysRemove(EXP_UNIT* pInPar);
extern EXP_UNIT* EXP_FileSysAutoTest(EXP_UNIT* pInPar);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

const EXP_FunctionMatchTbl FunctionMatchTbl[] = 
{
	"showMsg",		EXP_FshowMsg,
	"beep",			Exp_Beep,
	"tree",			EXP_Ftree,
	"save",			EXP_Fsave,
	"read",			EXP_Fread,
	"b64d",			EXP_Fb64dec,
	"b64e",			EXP_Fb64enc,
	"urld",			UrlDecode,
	"urle",			UrlEncode,
	"bts",			EXP_FBcdToStr,
	"stb",			EXP_FStrToBcd,
	"outf", 		EXP_FOutf,
	"inf", 			EXP_Finf,
	"cpy",			EXP_Fscpy,
	"dec",			EXP_FDes,
	"enc",			EXP_FEnc,
	"sm4e",			EXP_FSm4Enc,
	"sm4d",			EXP_FSm4Des,
	"dev",			EXP_GetDeveloper,
	"loll",			EXP_LoadLotteryLink,
	"lold",			EXP_LoadLotteryData,
	"glb",			EXP_GetLotteryBit,
	"glxb",			EXP_GetLotteryXorBit,
	"cxb",			EXP_CheckXorBuff,
	"cxa",			EXP_CheckXorALL,
	//-----------内部函数------------
	"enexist",			EXP_Fenexist,
	"fnrecnum",			EXP_Ffnrecnum,
	"enrecnum",			EXP_Fenrecnum,
	"fnexist",			EXP_Ffnexist,
	"putsep",			EXP_Fputsep,
	"putvar",			EXP_Fputvar,
	"putfix",			EXP_Fputfix,
	"getsep",			EXP_Fgetsep,
	"getvar",			EXP_Fgetvar,
	"getfix",			EXP_Fgetfix,
	"replace",			EXP_Freplace,
	"pmatch",			EXP_Fpmatch,
	"conv",				EXP_Fconv,
	"exist",			EXP_Fexist,
	"len",				EXP_Flen,
	"cardfmt",			EXP_Fcardfmt,
	"getdt",			EXP_Fgetdt,
	"substr",			EXP_Fsubstr,
	"printf",			EXP_Fprintf,
	//----多渠道交易功能函数集--------
	"getoffsetl_v3",	EXP_Fgetoffsetl_v3,
	"putoffsetl_v3",	EXP_Fputoffsetl_v3,
	"gettlv3",			EXP_Fgettlv3,
	"puttlv3",			EXP_Fputtlv3,
	"getoffsetlv3",		EXP_Fgetoffsetlv3,
	"putoffsetlv3",		EXP_Fputoffsetlv3,
	"scanf",			EXP_Fscanf,
	//-------快速检索文件内部函数---------
	"createFile",		EXP_FcreateFile,
	"createIdx",		EXP_FcreateIdx,
	"getRecSum",		EXP_FgetRecSum,
	"getFileMtime",		EXP_FgetFileMtime,
	//--------其他新增内部函数------------
	"getwk",			EXP_Fgetwk,
	"convwk",			EXP_Fconvwk,
	"cmdexist",			EXP_Fcmdexist,
	"funcexist",		EXP_Ffuncexist,
	"calTime",			EXP_FcalTime,
	"calInterval",		EXP_FcalInterval,
	//---------银联钱包交易内部函数-------
	"putTLV",			EXP_FputTLV,
	"getTLV",			EXP_FgetTLV,
	"putLV",			EXP_FputLV,
	"getLV",			EXP_FgetLV,
	"sigfea",			NULL,

	"tfsinit",			EXP_TreeFileInit,
	"tfsrun",			EXP_TreeFileRun,

	"sinit",			EXP_FileSysInit,
	"sdeinit",			EXP_FileSysDeinit,
	"sopen",			EXP_FileSysOpen,
	"swrite",			EXP_FileSysWrite,
	"sread",			EXP_FileSysRead,
	"sseek",			EXP_FileSysSeek,
	"sclose",			EXP_FileSysClose,
	"sremove",			EXP_FileSysRemove,
	"sauto",			EXP_FileSysAutoTest
};

#pragma GCC diagnostic pop

//=====================------=======================================
EXP_UNIT* EXP_StrFun(char** pStr,char* pEnd)
{
	u16 i,Max;
	u16 uNum,sLen;
	char* p=*pStr;
	char *pE;
	p++;//过滤'F'
	for(sLen=0; p[sLen] >= '0' ; sLen++);	//计算函数名的长度
	Max=sizeof(FunctionMatchTbl)/sizeof(FunctionMatchTbl[0]);
	for(i=0;i<Max;i++)
	{
		uNum=API_strlen(FunctionMatchTbl[i].pFunName);
		if(uNum==sLen && 0==API_memcmp(p,FunctionMatchTbl[i].pFunName,uNum))
		{
			break;
		}
	}
	if(i<Max)
	{
		p += sLen;
		while(*p==' ') p++;	//过滤空格
		if(*p == '(')
		{//-----括号处理--------
			TRACE("EXP_StrFun_StrALL(prosse)");
			for(uNum=1,pE=++p;pE<pEnd;pE++)
			{
				if(*pE == '(') uNum++;
				else if(*pE == ')') uNum--;
				if(uNum==0) break;
			}		
			*pStr=pE+1;	//par+')'
			TRACE("EXP_StrFun---- Run FunName[%s](%B) ",FunctionMatchTbl[i].pFunName,p,pE-p);
			return (*FunctionMatchTbl[i].pFunExp)(EXP_StrALL(&p,pE));
		}
	}
	else 
	{
		TRACE( "EXP_StrFun_StrALL FunNull ");
	}
	//*pStr=p;
	return NULL;
}

/*
static DfBuffTreeProc* pTreeBuffExpFunction=NULL;

int EXP_GetFunIndex(char* pstrTag)
{
	int i,Max;
	Max=sizeof(FunctionMatchTbl)/sizeof(FunctionMatchTbl[0]);
	for(i=0;i<Max;i++)
	{
		if(pstrTag==FunctionMatchTbl[i].pFunName)
			return i;
	}
	return -1;
}
void EXP_StrFunTreeInit(void)
{
	u32 i,Max;
	char* *pStringGroup;
	Max=sizeof(FunctionMatchTbl)/sizeof(FunctionMatchTbl[0]);
	pStringGroup=(char**)malloc(Max*sizeof(char*));
	for(i=0;i<Max;i++)
	{
		pStringGroup[i]=FunctionMatchTbl[i].pFunName;
	}
	//================滤除第一个字符F==========================
	pTreeBuffExpFunction=CreateBuffTree(pStringGroup,Max,1,&EXP_GetFunIndex);
	if(pTreeBuffExpFunction)
	{
		TRACE("EXP_StrFunTreeInit OK");
	}
	free(pStringGroup);
}

EXP_UNIT* EXP_StrFunTree(char** pStr,char* pEnd)
{
	u16 i,uNum;
	char *pE,*p;
	DfBuffTreeNode* pTreeNode;
	if(pTreeBuffExpFunction==NULL) return NULL;
	p=(*pStr)+1; //---跳过'F'---------
	for(i=0; p[i] >= '0' ; i++);	//计算函数名的长度
	pTreeNode=FindBuffNode(p,i,(DfBuffTreeNode*)pTreeBuffExpFunction->Buff);
	p += i;
	while(*p==' ') p++;	//过滤空格
	//TRACE( "EXP_StrFunTree2[%c]",*p);
	if(*p == '(')
	{//-----括号处理--------
		if(pTreeNode->btu[0].TagChar == '\0')
		{
			TRACE( "EXP_StrFun_StrALL(prosse)");
			for(uNum=1,pE=++p;pE<pEnd;pE++)
			{
				if(*pE == '(') uNum++;
				else if(*pE == ')') uNum--;
				if(uNum==0) break;
			}	
			*pStr=pE+1;	//par+')'
			//----------------计算虚化索引位置并取值------------------------------
			i=pTreeNode->btu[0].Next;
			TRACE( "EXP_StrFunTree Run FunName[%s]",FunctionMatchTbl[i].pFunName);
			return (*FunctionMatchTbl[i].pFunExp)(EXP_StrALL(&p,pE));
		}
	}
	TRACE( "EXP_StrFun find fail");
	return NULL;
}


void EXP_StrFunTreeEnd(void)
{
	FreeNull(pTreeBuffExpFunction);
}



void DemoTreeFun(char* sEdit)
{
	char *showMsg=NULL;
	if(sEdit[0]=='F')
	{
		showMsg=FindSameBuffContentAll(sEdit+1,(DfBuffTreeNode*)pTreeBuffExpFunction->Buff);
		if(showMsg!=NULL)
		{
			TRACE_HEC("DemoTreeFun:",showMsg,API_strlen(showMsg));
		}
		free(showMsg);
	}
}
*/

