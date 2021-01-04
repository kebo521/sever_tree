/*****************************检索服务******************************
//功能-------  用于将任意字符串组生成树形引导表，提高检索效率
//使用-------  可用于菜单检索，脚本POS脚本解析器表达式函数引导
//作者-------  邓国祖
//创作时间---20161213
******************************************************************************/
#include "comm.h"

//----------定义字符结构----------
typedef union
{
	char	B1Char[2];
	u16		B2Char;	//字符值
}DfTagCharUnit;

//----------定义字符串中字符坐标----------
typedef struct
{
	u16		pY;	//字符组选择
	u16		pX;	//字符串偏移
}DfStringOffset;

//----------定义基本元素----------
typedef struct
{
	DfTagCharUnit		TC;			//字符值
	u16 				Next;		//相对位置,当TagChar=='\0' 时，他就是fIndex 功能索引,
}DfBuffTreeUnit;
//----------定义树型Buff结构----------
typedef struct
{
	u16		Number;
	DfBuffTreeUnit btu[0];
}DfBuffTreeNode;
//----------定义文件结构-----------------
typedef struct
{
	u16		Len;
	u8		Buff[0];
}DfBuffTreeProc;


//-------定义分散结构------------------
typedef struct _DfTreeNode
{
	struct _DfTreeNode *pFilial;	//深度衍生
	struct _DfTreeNode *pBrother;	//旁边延生
	DfTagCharUnit		TC;	//字符值
	char				EndChar;
	u8					Num;
	void 				*pNext[0];		
}DfTreeNode;




void APP_U32Xor(u32* pbuf1, u32* pbuf2, int Inle)
{
	int i;
	for (i = 0; i<Inle; i++)
		pbuf1[i] ^= pbuf2[i];
}

//===========================排列字符=======================================================
void ArrayStringGroup(u16 MaxNum,const char* pStringGroup[],DfStringOffset *pOffset)
{
	u16 i,j;
	DfTagCharUnit TagChar,hChar;
	for(j=0;j<MaxNum;j++)
	{		
		TagChar.B1Char[0] = pStringGroup[pOffset[j].pY][pOffset[j].pX];
		if(TagChar.B1Char[0]&0x80)
			TagChar.B1Char[1]= pStringGroup[pOffset[j].pY][pOffset[j].pX+1];
		else TagChar.B1Char[1]=0;
		
		for(i=j+1;i<MaxNum;i++)
		{
			hChar.B1Char[0]=pStringGroup[pOffset[i].pY][pOffset[i].pX];
			if(hChar.B1Char[0]&0x80)
				hChar.B1Char[1]=pStringGroup[pOffset[i].pY][pOffset[i].pX+1];
			else hChar.B1Char[1]=0;
				
			if(TagChar.B2Char > hChar.B2Char)
			{//小端排列,交换pOffset参数
				APP_U32Xor((u32*)&pOffset[j], (u32*)&pOffset[i], sizeof(DfStringOffset) / sizeof(u32));
				APP_U32Xor((u32*)&pOffset[i], (u32*)&pOffset[j], sizeof(DfStringOffset) / sizeof(u32));
				APP_U32Xor((u32*)&pOffset[j], (u32*)&pOffset[i], sizeof(DfStringOffset) / sizeof(u32));
				TagChar.B2Char = hChar.B2Char;
			}
		}
	}
}
//===========================统计相同字符个数==============================================
u16 CountCharSameNumber(u16 MaxNum,const char* pStringGroup[], DfStringOffset *pOffset,u16 *pCountTable,DfTagCharUnit* pOutChar)
{
	u16 CountTag;
	DfTagCharUnit TagChar,hChar;
	u16 i=1,num=1,offset;

	TagChar.B1Char[0] = pStringGroup[pOffset[0].pY][pOffset[0].pX++];
	if(TagChar.B1Char[0]&0x80)
		TagChar.B1Char[1]= pStringGroup[pOffset[0].pY][pOffset[0].pX++];
	else TagChar.B1Char[1]=0;
	
	CountTag=pCountTable[0];
	for(i=1;i<MaxNum;i++)
	{
		if(CountTag != pCountTable[i]) break;
		offset= pOffset[i].pX;
		
		hChar.B1Char[0]=pStringGroup[pOffset[i].pY][offset++];
		if(hChar.B1Char[0]&0x80)
			hChar.B1Char[1]=pStringGroup[pOffset[i].pY][offset++];
		else hChar.B1Char[1]=0;

		if(TagChar.B2Char != hChar.B2Char) break;
		pOffset[i].pX =offset;
		num++;
	}
	if(pOutChar) pOutChar->B2Char=TagChar.B2Char;
	
	return num;
}

//=============================统计字符种类======================================================
u16 CountStringDifferentNumber(u16 MaxNum,const char* pStringGroup[], DfStringOffset *pOffset,u16 *pCountTable)
{
	u16 i=0,num=0;
	u16 Tag,hChar;
	Tag=0xffff;
	while(i<MaxNum)
	{
		hChar=pStringGroup[pOffset[i].pY][pOffset[i].pX];
		if(hChar >= 0x80)
			hChar = (hChar<<8) + pStringGroup[pOffset[i].pY][pOffset[i].pX+1];
		
		if(Tag != hChar)
		{
			Tag = hChar;
			num++;
		}
		//-----建立统计表--------
		if(pCountTable) pCountTable[i]= Tag;
		i++;
	}
	return num;
}

//================创建连续空间树形节点======递归调用========================================================
u8* CreateBuffTreeNode(u16 MaxNum,const char* pStringGroup[], DfStringOffset *pOffset,u16 *pCountTable,u8* pTagBuff,u8* pTagEnd)
{
	u16 TypeChar,i,IndexStard,NextTypeChar;
	DfBuffTreeNode* pCurrent;
	ArrayStringGroup(MaxNum,pStringGroup,pOffset);
	TypeChar=CountStringDifferentNumber(MaxNum,pStringGroup,pOffset,pCountTable);

	pCurrent=(DfBuffTreeNode*)pTagBuff;
	pTagBuff += sizeof(DfBuffTreeNode)+TypeChar*sizeof(DfBuffTreeUnit);
	if(pTagBuff > pTagEnd)
	{
		TRACE("CreateBuffTreeNode pTagBuff No ");
		return NULL;
	}
	pCurrent->Number=TypeChar; 
	for(IndexStard=0,i=0;i<TypeChar;i++)
	{
		NextTypeChar=CountCharSameNumber(MaxNum-IndexStard,pStringGroup,&pOffset[IndexStard],&pCountTable[IndexStard],&pCurrent->btu[i].TC);
		if(pCurrent->btu[i].TC.B2Char == 0x00) 
		{
			pCurrent->btu[i].Next= pOffset[IndexStard].pY;
		}
		else
		{
			pCurrent->btu[i].Next=pTagBuff-(u8*)pCurrent;
			pTagBuff=CreateBuffTreeNode(NextTypeChar,pStringGroup, &pOffset[IndexStard],&pCountTable[IndexStard],pTagBuff,pTagEnd);
			if(pTagBuff == NULL)
			{
				TRACE("CreateBuffTreeNode [%s]pTagBuff NULL",pCurrent->btu[i].TC.B1Char);
				return NULL;
			}
		}
		IndexStard += NextTypeChar;
	}
	return pTagBuff;
}

DfBuffTreeProc* CreateBuffTree(const char* pStringGroup[],u16 StringNum)
{
	DfBuffTreeProc *pStardBuff;
	DfStringOffset *pOffset;
	u8* pBuff;
	u16 *pCountTable,i;
	pCountTable=(u16*)malloc(StringNum*sizeof(DfTagCharUnit));
	pOffset=(DfStringOffset*)malloc(StringNum*sizeof(DfStringOffset));
	for(i=0;i<StringNum;i++)
	{
		pCountTable[i]=0;
		pOffset[i].pX = 0;
		pOffset[i].pY = i;
	}
	pStardBuff=(DfBuffTreeProc *)malloc(0xFFFF);
	pBuff=CreateBuffTreeNode(StringNum,pStringGroup,pOffset,pCountTable,pStardBuff->Buff,pStardBuff->Buff + 0xFFFF-sizeof(pStardBuff->Len));
	free(pOffset);
	free(pCountTable);
	if(pBuff==NULL)
	{
		free(pStardBuff);
		pStardBuff=NULL;
	}
	else
	{
		pStardBuff->Len=pBuff - pStardBuff->Buff;
		pStardBuff=(DfBuffTreeProc *)realloc(pStardBuff,pStardBuff->Len+sizeof(pStardBuff->Len));
		
	}
	return pStardBuff;
}


//===================查找目标字符串查对应节点====================================================
DfBuffTreeNode* FindStringBuffNode(const char* pTargetChars ,u16 CharsLen,DfBuffTreeNode *pStardNode)
{
	u16 Offset=0;
	DfTagCharUnit hChar;
	u16 left,right,middle;
	//------找到节点--------
	while(Offset < CharsLen)
	{
		hChar.B1Char[0]=pTargetChars[Offset++];
		if(hChar.B1Char[0]&0x80)
			hChar.B1Char[1]=pTargetChars[Offset++];
		else hChar.B1Char[1]=0;

		left=0;
		right=pStardNode->Number-1;
		for(;;)
		{
			middle=(left+right)/2;
			if(hChar.B2Char == pStardNode->btu[middle].TC.B2Char)
			{
				if(pStardNode->btu[middle].Next == 0x0000)
				{
					TRACE("FindStringBuffNode[%s] Next Err",pStardNode->btu[middle].TC.B1Char);
					return NULL;
				}
				pStardNode=(DfBuffTreeNode*)(((u8*)pStardNode)+pStardNode->btu[middle].Next);
				break;
			}
			if(hChar.B2Char > pStardNode->btu[middle].TC.B2Char)
				left=middle+1;
			else
				right=middle-1;
			if(left > right) 
				return NULL;
		}
	}
	return pStardNode;
}


//============检索字符串中下一个存在字符并通过pOutStr输出全部可能=========================
void FindSameStringBuffContent(char* pTargetstring,DfBuffTreeNode *pStardNode,char *pOutStr)
{
	int i,CharsLen;
	CharsLen=API_strlen(pTargetstring);
	pStardNode=FindStringBuffNode(pTargetstring,CharsLen,pStardNode);
	API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;
	if(pStardNode)
	{
		*pOutStr++ = '\n';
		for(i=0;i< pStardNode->Number ;i++)
		{
			API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;
			if(pStardNode->btu[i].TC.B1Char[0])
			{
				*pOutStr++ = pStardNode->btu[i].TC.B1Char[0];
				if(pStardNode->btu[i].TC.B1Char[1])
					*pOutStr++ = pStardNode->btu[i].TC.B1Char[1];
			}
			else
				*pOutStr++ ='>';
			*pOutStr++ ='\n';
		}
	}
	else
	{
		API_memcpy(pOutStr," NULL\n",6); pOutStr += 6;
	}
	*pOutStr ='\0';
}

//============找到所有匹配项，并通过pOutStr输出全部字符=======递归调用=================================
char* FindSameStringGroup(char* pTargetstring,u16 stringLen,DfBuffTreeNode* pTreeNode,char *pOutStr,char *pOutEnd)
{
	u16 i,offset;
	//Max=pTreeNode->Number;
	for(i=0;i< pTreeNode->Number ;i++)
	{
		if(pTreeNode->btu[i].TC.B2Char == 0x0000)
		{
			if((pOutStr + stringLen+2) > pOutEnd) 
			{
				TRACE("FindSameStringGroup Err[not enough space]");
				return NULL;
			}
			API_memcpy(pOutStr,pTargetstring,stringLen);
			pOutStr[stringLen+0]='>';
			pOutStr[stringLen+1]='\n';
			pOutStr += stringLen+2;
		}
		else 
		{
			offset=stringLen;
			pTargetstring[offset++]=pTreeNode->btu[i].TC.B1Char[0];
			if(pTreeNode->btu[i].TC.B1Char[0]&0x80)
				pTargetstring[offset++]=pTreeNode->btu[i].TC.B1Char[1];
			
			pOutStr=FindSameStringGroup(pTargetstring,offset,(DfBuffTreeNode*)((u8*)pTreeNode+pTreeNode->btu[i].Next),pOutStr,pOutEnd);
			if(pOutStr==NULL) return NULL;
		}
	}
	return pOutStr;
}

//=======匹配pTargetstring字符串，并输出与之长度部分匹配的所有字符串---耗用空间比较大-------
char* FindSameBuffContentAll(char* pTargetstring,DfBuffTreeNode* pStardNode)
{
	char sBuff[40]={0};
	u16 CharsLen;
	unsigned int  OutSize;
	char *pStartStr,*pOutStr;
	OutSize=4096*2;
	pStartStr=(char*)malloc(OutSize);
	if(pStartStr==NULL) return NULL;
	//TRACE("FindSameBuffContentAll gPreAuthMemory[%d]",OutSize);
	pOutStr=pStartStr;
	CharsLen=API_strlen(pTargetstring);
	API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen; OutSize -= CharsLen;
	pStardNode=FindStringBuffNode(pTargetstring,CharsLen,pStardNode);
	if(pStardNode)
	{
		*pOutStr++='\n'; OutSize--;
		API_memcpy(sBuff,pTargetstring,CharsLen);
		pOutStr=FindSameStringGroup(sBuff,CharsLen,pStardNode,pOutStr,pOutStr+OutSize);
		if(pOutStr==NULL) 
		{
			free(pStartStr);
			return NULL;
		}
	}	
	*pOutStr++ ='\0';
	OutSize=pOutStr-pStartStr;
	pStartStr=(char*)realloc(pStartStr,OutSize);
	//TRACE("FindSameBuffContentAll realloc[%d]",OutSize);
	return pStartStr;
}


//=================结构转换=分散============Dispersion polymerization 
DfTreeNode *StructureTransformDispe(DfBuffTreeNode *pStardNode)
{
	u32 i;
	DfTreeNode *pStard=NULL,*pTreeNode=NULL;
	for(i=0;i<pStardNode->Number;i++)
	{
		if(pTreeNode ==NULL)
		{
			pTreeNode=(DfTreeNode *)malloc(sizeof(DfTreeNode));
			pStard=pTreeNode;
		}
		else
		{
			pTreeNode->pBrother=(DfTreeNode *)malloc(sizeof(DfTreeNode));
			pTreeNode=pTreeNode->pBrother;
		}
		memset(pTreeNode,0x00,sizeof(DfTreeNode));
		pTreeNode->TC.B2Char=pStardNode->btu[i].TC.B2Char;
		//pTreeNode->Index=0;//pStardNode->btu[i].Next;
		if (pTreeNode->TC.B2Char)
			pTreeNode->pFilial = StructureTransformDispe((DfBuffTreeNode *)((u8*)pStardNode + pStardNode->btu[i].Next));
	}
	pTreeNode->pBrother=NULL;
	return pStard;
}

//=================结构转换=聚合============polymerization 
u8 *StructureTransformPolymer(DfTreeNode *pStardNode,u8* pStardBuff,u8* pStardEnd)
{
	u32 			i;
	DfBuffTreeNode *pStard;
	DfTreeNode *pNext;
	//----------------------------------
	pNext=pStardNode;
	i=0;
	while(pNext)
	{
		i++;
		pNext=pNext->pBrother;
	}
	pStard=(DfBuffTreeNode *)pStardBuff;
	pStardBuff += (sizeof(DfBuffTreeNode) + i*sizeof(DfBuffTreeUnit));
	if(pStardBuff > pStardEnd) 
	{
		TRACE("StructureTransformPolymer Err pStardBuff No Len");
		return NULL;
	}
	pStard->Number=i;
	//----------------------------------
	pNext=pStardNode;
	i=0;
	while(pNext)
	{
		pStard->btu[i].TC.B2Char=pNext->TC.B2Char;
		if(pNext->pFilial)
		{
			pStard->btu[i].Next=pStardBuff-(u8*)pStard;
			pStardBuff=StructureTransformPolymer(pNext->pFilial,pStardBuff,pStardEnd);
			if(pStardBuff==NULL) 
			{
				TRACE("StructureTransformPolymer[%s]Err",pNext->TC.B1Char);
				return NULL;
			}
		}
		else
		//	pStard->btu[i].Next=pNext->Index;
			pStard->btu[i].Next=0x0000;
		
		i++;
		pNext=pNext->pBrother;
	}
	return pStardBuff;
}
//================释放树形字符串======递归调用========================================================
void FreeTreeNode(DfTreeNode *pStardNode)
{
	DfTreeNode *pNode;
	while(pStardNode)
	{
		if(pStardNode->pFilial)
		{
			FreeTreeNode(pStardNode->pFilial);
		}
		pNode=pStardNode;
		pStardNode=pStardNode->pBrother;
		free(pNode);
	}
}
//===================查找目标字符串查对应节点====================================================
DfTreeNode* FindTreeNode(const char* pTargetChars ,u16 CharsLen,DfTreeNode *pStardNode)
{
	u16 Offset=0;
	DfTagCharUnit hChar;
	DfTreeNode *pLastBrother;
	//------找到节点--------
	while(Offset < CharsLen)
	{
		if(pStardNode == NULL) break;
		hChar.B1Char[0] = pTargetChars[Offset++];
		if(hChar.B1Char[0]&0x80)
			hChar.B1Char[1] = pTargetChars[Offset++];
		else hChar.B1Char[1] =0x00;
		
		pLastBrother=NULL;
		while(pStardNode)
		{
			if(hChar.B2Char == pStardNode->TC.B2Char)
			{
				pStardNode=pStardNode->pFilial;
				break;
			}
			pLastBrother=pStardNode;
			pStardNode=pStardNode->pBrother;	
		}
		//if(pStardNode==NULL) return NULL;
		if(pStardNode==NULL)
		{
			if(pLastBrother)
			{
				pLastBrother->pBrother=(DfTreeNode *)malloc(sizeof(DfTreeNode));
				pLastBrother=pLastBrother->pBrother;
				pLastBrother->TC.B2Char =hChar.B2Char;
				
				pLastBrother->pBrother=NULL;
				pLastBrother->pFilial=(DfTreeNode *)malloc(sizeof(DfTreeNode));
				pLastBrother=pLastBrother->pFilial;
				pLastBrother->TC.B2Char =0x0000;
				
				pLastBrother->pBrother=NULL;
				pLastBrother->pFilial=NULL;
			}
			else return NULL;
			pStardNode=pLastBrother;
		}
	}
	return pStardNode;
}


DfTreeNode* UnderstandTreeNode(const char* pTargetChars,const char* pCharEnd,DfTreeNode *pStardNode)
{
	if(pStardNode) 
	{
		u16 Offset=0;
		DfTagCharUnit hChar;
		DfTreeNode *pStard;

		hChar.B1Char[0] = pTargetChars[Offset++];
		if(hChar.B1Char[0]&0x80)
			hChar.B1Char[1] = pTargetChars[Offset++];
		else hChar.B1Char[1] =0x00;
		
		if((pTargetChars+Offset) > pCharEnd) 
			return pStardNode;

		if(hChar.B2Char == pStardNode->TC.B2Char)
		{
			TRACE("Adder[%08X]TagChar[%s]",pStardNode,pStardNode->TC.B1Char);
			pStard=UnderstandTreeNode(pTargetChars+Offset,pCharEnd,pStardNode->pFilial);
			//if(pStard)
			//	pStard->pRelateLast=pStardNode;
			//pStardNode->pRelateLast=pStard;
		}
		else
		{
			pStard=UnderstandTreeNode(pTargetChars,pCharEnd,pStardNode->pFilial);
			if(pStard==NULL)
				pStard=UnderstandTreeNode(pTargetChars,pCharEnd,pStardNode->pBrother);
		}
		return pStard;
	}
	return NULL;
}


//============检索字符串中下一个存在字符并通过pOutStr输出全部可能=========================
u8 FindSameTreeContent(char* pTargetstring,DfTreeNode *pStardNode,char *pOutStr)
{
	int CharsLen;
	CharsLen=API_strlen(pTargetstring);
	//pStardNode=FindTreeNode(pTargetstring,CharsLen,pStardNode);
	pStardNode= UnderstandTreeNode(pTargetstring,pTargetstring+CharsLen,pStardNode);
	API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;

	if(pStardNode)
	{
		*pOutStr++ = '\n';
		while(pStardNode)
		{
			if(pStardNode->TC.B1Char[0])
			{
				*pOutStr++ = pStardNode->TC.B1Char[0];
				if(pStardNode->TC.B1Char[1])
					*pOutStr++ = pStardNode->TC.B1Char[1];
			}
			else
				*pOutStr++ ='>';			
			*pOutStr++ ='\n';
			//pStardNode=pStardNode->pRelateLast;	
		}
		*pOutStr ='\0';
		return 0;
	}
	else
	{
		strcpy(pOutStr," 是什么意思?\n");
		return 1;
	}
	/*
	if(pStardNode)
	{
		*pOutStr++ = '\n';
		while(pStardNode)
		{
			API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;
			if(pStardNode->TagChar == '\0')
				*pOutStr++ ='>';
			else if (pStardNode->TagChar > 0xff)
			{
				TRACE("TagChar[%X]",pStardNode->TagChar);
				*pOutStr++ = pStardNode->TagChar/0x100;
				*pOutStr++ = pStardNode->TagChar&0xff;
			}
			else
				*pOutStr++ =pStardNode->TagChar;
			*pOutStr++ ='\n';
			pStardNode=pStardNode->pBrother;	
		}
	}
	else
	{
		API_memcpy(pOutStr," NULL\n",6); pOutStr += 6;
	}
	*pOutStr ='\0';
	*/
}

//------演示字符串-----------
const char *M_TextID_Funtion[]=
{
	"875924691346",	
	"3414325633",		
	"087675",			
	"123233242",			
	"34235664",		
	"1233141", 
	"156889555",		
	"12345342",		
	"12334555",
	"1265444",
	"128899776",
	"16554433",
	"23342423324",
	"235667655",
	"23",
	"234",
	"2345",
	"23456",
	"23457",
	"23458",
	"234589",
	"2345891",
	"2345892",
	"2345893",
	"23458934",
	"23458935",
	"23458936",
	"23458937",
	"2",
	"1",
	"我是谁",
	"我是你",
	"我在哪",
	"1他",
	"我1个条件",
	"我12个条件",
	"我123个1条件",
	"\0"
};


//========输入演示=======================
int TreeDemoNode1(void)
{
	char sEdit[16]={0};
	char *pShowMsg;
	DfBuffTreeProc *pStardNode;
	TRACE("1    建立树型参数文件    1");
	
	pStardNode=CreateBuffTree(M_TextID_Funtion,sizeof(M_TextID_Funtion)/sizeof(M_TextID_Funtion[0]));
	if(pStardNode==NULL) return 1;
	pShowMsg=FindSameBuffContentAll(sEdit,(DfBuffTreeNode *)pStardNode->Buff);
	if(pShowMsg)
	{
	//	printf(pShowMsg);
		puts(pShowMsg);
		free(pShowMsg);
	}
	
	while(1)
	{
		printf("\n请输入：");
		//gets(sEdit);
		scanf("%s",sEdit);
		//scanf_s("%s", sEdit,sizeof(sEdit));
		if(strcmp(sEdit,"exit"))
		{
			pShowMsg=FindSameBuffContentAll(sEdit,(DfBuffTreeNode *)pStardNode->Buff);
			if(pShowMsg)
			{
			//	printf(pShowMsg);
				puts(pShowMsg);
				free(pShowMsg);
			}
		}	
		else break;
	}
	TRACE_HEX("TreeBuff:",(u8*)pStardNode,sizeof(pStardNode->Len)+pStardNode->Len);
	APP_fileSave("TreeBuff.bin",pStardNode,sizeof(pStardNode->Len)+pStardNode->Len);
	free(pStardNode);
	return 0;
}

int TreeDemoNode2(void)
{
	char sEdit[16]={0},sExplain[64]={0},showMsg[256]={0};
	u8 AddFlag=0,*pBuff;
	TRACE("2    建立树型参数    2");
	DfBuffTreeProc *pStardNode;
	DfTreeNode *pTreeNode;
	pStardNode=APP_fileRead("TreeBuff.bin",NULL);
	if(pStardNode==NULL) return 1;
	TRACE_HEX("TreeBuff:",(u8*)pStardNode,sizeof(pStardNode->Len)+pStardNode->Len);
	pTreeNode=StructureTransformDispe((DfBuffTreeNode *)pStardNode->Buff);
	free(pStardNode);
	if(pTreeNode==NULL) return 1;

	//FindSameTreeContent(sEdit,pTreeNode,showMsg);
	while(1)
	{
		if(AddFlag==0)
		{
			printf("\n请输入：");
			scanf("%s", sEdit);
			if(strcmp(sEdit,"exit"))
			{
				AddFlag=FindSameTreeContent(sEdit,pTreeNode,showMsg);
				//printf_s(showMsg);
				puts(showMsg);
			}	
			else break;
		}
		else
		{
			scanf("%s", sExplain);
			AddFlag=0;
		}
	}
	pStardNode=(DfBuffTreeProc *)malloc(0xFFFF);
	memset(pStardNode,0x00,0xfff0);
	
	pBuff=StructureTransformPolymer(pTreeNode,pStardNode->Buff,pStardNode->Buff+0xffff-sizeof(pStardNode->Len));
	FreeTreeNode(pTreeNode);
	if(pBuff==NULL) 
	{
		free(pStardNode);
		return 1;
	}
	pStardNode->Len=pBuff-pStardNode->Buff;
	APP_fileSave("TreeBuff.bin",pStardNode,pBuff-(u8*)pStardNode);
	free(pStardNode);
	return 0;
}


/*
typedef struct
{
	DfTagCharUnit		TC;	//字符值	
	u16					FunIndex;	//功能索引无效为FF(~0)
	void 				*pNext;		//DfStringTreeNode
}DfStringTreeUnit;

typedef struct
{
	u32		Number;
	DfStringTreeUnit ctu[0];
}DfStringTreeNode;


//================创建树形节点======递归调用========================================================
DfStringTreeNode* CreateStringTreeNode(u16 MaxNum,const char* pStringGroup[], DfStringOffset *pOffset,u16 *pCountTable)
{
	u16 TypeChar,i,IndexStard,NextTypeChar;
	DfStringTreeNode* pCurrent;
	ArrayStringGroup(MaxNum,pStringGroup,pOffset);
	TypeChar=CountStringDifferentNumber(MaxNum,pStringGroup,pOffset,pCountTable);
	pCurrent=(DfStringTreeNode*)malloc(sizeof(DfStringTreeNode)+TypeChar*sizeof(DfStringTreeUnit));
	//TRACE("malloc[%x]",pCurrent);
	pCurrent->Number=TypeChar; 
	for(IndexStard=0,i=0;i<TypeChar;i++)
	{
		//pCurrent->ctu[i].level=Offset;
		NextTypeChar=CountCharSameNumber(MaxNum-IndexStard,pStringGroup,&pOffset[IndexStard],&pCountTable[IndexStard],&pCurrent->ctu[i].TagChar);
		if(pCurrent->ctu[i].TC.B2Char == 0x0000) 
		{
			pCurrent->ctu[i].FunIndex = pOffset[IndexStard].pY;
			pCurrent->ctu[i].pNext=NULL;
		}
		else
		{
			pCurrent->ctu[i].FunIndex =0xffff;
			pCurrent->ctu[i].pNext=CreateStringTreeNode(NextTypeChar,pStringGroup, &pOffset[IndexStard],&pCountTable[IndexStard]);
		}
		IndexStard += NextTypeChar;
	}
	return pCurrent;
}

//================创建树形字符串==============================================================
DfStringTreeNode* CreateStringTree(const char* pStringGroup[],u16 StringNum)
{
	DfStringTreeNode *pStardNode;
	DfStringOffset *pOffset;
	u16 i,*pCountTable;
	pCountTable=(u16*)malloc(StringNum*sizeof(u16));
	pOffset=(DfStringOffset*)malloc(StringNum*sizeof(DfStringOffset));
	for(i=0;i<StringNum;i++)
	{
		pCountTable[i]=0;
		pOffset[i].pX = 0;
		pOffset[i].pY = i;
	}
	pStardNode=CreateStringTreeNode(StringNum,pStringGroup,pOffset,pCountTable);
	free(pOffset);
	free(pCountTable);
	return pStardNode;
}

//================释放树形字符串======递归调用========================================================
void FreeStringTreeNode(DfStringTreeNode *pStardNode)
{
	while(pStardNode->Number --)
	{
		if(pStardNode->ctu[pStardNode->Number].pNext)
		{
			FreeStringTreeNode((DfStringTreeNode *)pStardNode->ctu[pStardNode->Number].pNext);
		}
	}
	TRACE("free[%x]",pStardNode);
	free(pStardNode);
}

//===================查找目标字符串查对应节点====================================================
DfStringTreeNode* FindStringNode(const char* pTargetChars ,u16 CharsLen,DfStringTreeNode *pStardNode)
{
	u16 Offset=0,i,Max,hChar;
	//------找到节点--------
	while(Offset<CharsLen)
	{
		if(pStardNode == NULL) break;
		hChar = 0x00ff & pTargetChars[Offset++];
		if(hChar&0x80)
			hChar = hChar*0x100 + (0x00ff&pTargetChars[Offset++]);
		Max=pStardNode->Number;
		for(i=0;i<Max ;i++)
		{
			if(hChar == pStardNode->ctu[i].TagChar)
			{
				//TRACE( "FindStringNode[%d][%c]-[%d]",Offset,pStardNode->ctu[i].TagChar,i);
				pStardNode=(DfStringTreeNode *)pStardNode->ctu[i].pNext;
				break;
			}
		}
		if(i == Max) return NULL;
	}
	return pStardNode;
}


//============检索字符串中下一个存在字符并通过pOutStr输出全部可能=========================
void FindSameStringContent(char* pTargetstring,DfStringTreeNode *pStardNode,char *pOutStr)
{
	int i,CharsLen;
	CharsLen=API_strlen(pTargetstring);
	pStardNode=FindStringNode(pTargetstring,CharsLen,pStardNode);
	
	API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;
	if(pStardNode)
	{
		*pOutStr++ = '\n';
		for(i=0;i< pStardNode->Number ;i++)
		{
			API_memcpy(pOutStr,pTargetstring,CharsLen); pOutStr += CharsLen;
			if(pStardNode->ctu[i].TagChar == '\0')
				*pOutStr++ ='>';
			else if (pStardNode->ctu[i].TagChar > 0xff)
			{
				TRACE("TagChar[%X]",pStardNode->ctu[i].TagChar);
				*pOutStr++ = pStardNode->ctu[i].TagChar/0x100;
				*pOutStr++ = pStardNode->ctu[i].TagChar&0xff;
			}
			else
				*pOutStr++ =pStardNode->ctu[i].TagChar;
			*pOutStr++ ='\n';
		}
	}
	else
	{
		API_memcpy(pOutStr," NULL\n",6); pOutStr += 6;
	}
	*pOutStr ='\0';
}



u32 CountStringSameNumber(char* pStringGroup[],u16 Offset,u16 MaxNum,u16 *pArrayIndexTable)
{
	u16 i,num=1;
	//----统计相同字符个数---------
	for(i=1;i<MaxNum;i++)
	{
		if(API_memcmp(pStringGroup[pArrayIndexTable[0]],pStringGroup[pArrayIndexTable[i]],Offset+1))
			break;
		num++;
	}
	return num;
}

DfStringTreeNode* FindStringNode(char* pTargetChars ,u16 CharsLen,DfStringTreeNode *pStardNode,u16 *pEmptyIndex)
{
	u16 Offset,i,Max;
	//------找到节点--------
	for(Offset=0;Offset<CharsLen;Offset++)
	{
		if(pStardNode == NULL) break;
		Max=pStardNode->Number;
		for(i=0;i<Max ;i++)
		{
			if(pTargetChars[Offset] == pStardNode->ctu[i].TagChar)
			{
				//TRACE( "FindStringNode[%d][%c]-[%d]",Offset,pStardNode->ctu[i].TagChar,i);
				pStardNode=(DfStringTreeNode *)pStardNode->ctu[i].pNext;
				break;
			}
		}
		if(i == Max) return NULL;
	}
	//------
	if(pStardNode)
	{
		if(pEmptyIndex)
		{
			for(i=0;i< pStardNode->Number ;i++)
			{
				if(pStardNode->ctu[i].FunIndex == 0xffff) break;
			}
			if(i< pStardNode->Number)
			{
				*pEmptyIndex=i;
				return pStardNode;
			}
			return NULL;
		}
	}
	return pStardNode;
}

DfStringTreeNode* FindStringNode3(char* pTargetChars ,u16 CharsLen,DfStringTreeNode *pStardNode,u16 MaxNode,u16 *pEmptyIndex)
{
	u16 Offset,i,Max;
	if(pStardNode == NULL)
	{
		pStardNode=(DfStringTreeNode*)malloc(sizeof(DfStringTreeNode)+MaxNode*sizeof(DfStringTreeUnit));
		TRACE( "malloc[%x]",pStardNode);
		pStardNode->Number= MaxNode;
		while(MaxNode--) pStardNode->ctu[MaxNode].level=0xff;
	}
	else
	{//------找到节点--------
		for(Offset=0;Offset<CharsLen;Offset++)
		{
			if(pStardNode == NULL) break;
			
			Max=pStardNode->Number;
			for(i=0;i<Max ;i++)
			{
				if(pTargetChars[Offset] == pStardNode->ctu[i].TagChar)
				{
					//TRACE( "FindStringNode[%d][%c]-[%d]",Offset,pStardNode->ctu[i].TagChar,i);
					if(pStardNode->ctu[i].pNext == NULL)
					{
						//if((Offset+1)!=CharsLen) return NULL;
						pStardNode->ctu[i].pNext=(DfStringTreeNode*)malloc(sizeof(DfStringTreeNode)+MaxNode*sizeof(DfStringTreeUnit));
						pStardNode=(DfStringTreeNode *)pStardNode->ctu[i].pNext;
						TRACE( "malloc[%x]",pStardNode);
						pStardNode->Number= MaxNode;
						while(MaxNode--) pStardNode->ctu[MaxNode].level=0xff;
					}
					else pStardNode=(DfStringTreeNode *)pStardNode->ctu[i].pNext;
					break;
				}
			}
			if(i == Max) return NULL;
		}
	}
	
	//------
	if(pStardNode)
	{
		for(i=0;i< pStardNode->Number ;i++)
		{
			if(pStardNode->ctu[i].level == 0xff) break;
		}
		if(i< pStardNode->Number)
		{
			*pEmptyIndex=i;
			return pStardNode;
		}
		return NULL;
	}
	return pStardNode;
}


DfStringTreeNode* CreateStringTree3(char* pStringGroup[],u16 StringNum)
{
	DfStringTreeNode *pCurrent,*pStardNode=NULL;
	u16 *pArrayIndexTable,*pCountTable;
	u8* pEndFlag;;
	u16 i,MaxI,EndNum=0,ctuIndex,Offset=0;
	u16 TypeChar,IndexStard,NextTypeChar,IndexNum;
	
	pArrayIndexTable=(u16*)malloc(StringNum*sizeof(u16));
	pCountTable=(u16*)malloc(StringNum*sizeof(u16));
	pEndFlag=(u8*)malloc(StringNum*sizeof(u8));
	for(i=0;i<StringNum;i++)
	{
		pArrayIndexTable[i]=i;
		pCountTable[i]=0;
		pEndFlag[i]=0;
	}
	IndexNum=StringNum;
	IndexStard=0;
	while(EndNum < StringNum)
	{
		MaxI=IndexStard+IndexNum;
 		for(i=IndexStard;i<MaxI;i++)
		{
			if(pEndFlag[i]==0)
			{
 				ArrayStringGroup(pStringGroup,Offset,IndexNum,&pArrayIndexTable[i]);
				TypeChar=CountStringDifferentNumber(pStringGroup,Offset,IndexNum,&pArrayIndexTable[i],&pCountTable[i]);
				//----建立结构-------------
				pCurrent =FindStringNode3(pStringGroup[pArrayIndexTable[i]],Offset,pStardNode,TypeChar,&ctuIndex);
				if(pCurrent==NULL)
				{
					TRACE( "QiuKScand Err pIndex[i][%d],rank[%d]",pArrayIndexTable[i],Offset);
					return NULL;
				}
				if(pStardNode==NULL) pStardNode=pCurrent;

				pCurrent->ctu[ctuIndex].level=Offset;
				NextTypeChar=CountCharSameNumber(pStringGroup,Offset,IndexNum-i,&pArrayIndexTable[i],&pCountTable[i],&pCurrent->ctu[ctuIndex].TagChar);
				if(pCurrent->ctu[ctuIndex].TagChar=='\0') 
				{
					pCurrent->ctu[ctuIndex].FunIndex = pArrayIndexTable[i];
					pCurrent->ctu[ctuIndex].pNext=NULL;
					pEndFlag[i]=1;
					if(++EndNum >= StringNum) break;
				}
				else
				{
					Offset++;
					IndexStard = i;
					IndexNum =NextTypeChar;
					break;
				}
			}
		}
		if(i==MaxI)
		{
			Offset=0;
			IndexNum=StringNum;
			IndexStard=0;
		}
	}
	free(pEndFlag);
	free(pCountTable);
	free(pArrayIndexTable);
	return pStardNode;
}

DfStringTreeNode* CreateStringTree2(char* pStringGroup[],u16 StringNum)
{
	char tagChar;
	u16 i,j,jp,EndNum,rank,NumChar,TypeChar;
	u8 *pEndFlag;
	u16* pIndex,*pCountTable;
	DfStringTreeNode *pStard,*pCrun;
	pEndFlag=(u8*)malloc(StringNum*sizeof(u8));
	pIndex=(u16*)malloc(StringNum*sizeof(u16));
	pCountTable=(u16*)malloc(StringNum*sizeof(u16));
	for(i=0;i<StringNum;i++)
	{
		pIndex[i]=i;
		pEndFlag[i]=0;
		pCountTable[i]=0;
	}
	EndNum=0;
	rank=0;

	ArrayStringGroup(pStringGroup,rank,StringNum,pIndex);
	//for(i=0;i<StringNum;i++) TRACE( "pIndex%d[%d]",i,pIndex[i]);
	TypeChar=CountStringDifferentNumber(pStringGroup,rank,StringNum,pIndex,pCountTable);
	
	//----建立结构-------------
	pCrun=(DfStringTreeNode*)malloc(sizeof(DfStringTreeNode)+TypeChar*sizeof(DfStringTreeUnit));
	TRACE( "malloc[%x]",pCrun);
	pCrun->Number=TypeChar;	
	for(j=0;j<TypeChar;j++)
	{
		pCrun->ctu[j].FunIndex = 0xffff;
	}
	
	pStard=pCrun;
	//--------------------------------
	while(EndNum < StringNum)
	{
		jp=0;
		for(i=0;i<StringNum;i++)
		{
			if(pEndFlag[i]==0)
			{
				tagChar=pStringGroup[pIndex[i]][rank];

				//----建立结构--寻找节点-----------
				pCrun=FindStringNode(pStringGroup[pIndex[i]],rank,pStard,&jp);
				if(pCrun==NULL)
				{
					TRACE( "QiuKScand Err pIndex[i][%d],rank[%d]",pIndex[i],rank);
					return NULL;
				}
				pCrun->ctu[jp].TagChar=tagChar;
				pCrun->ctu[jp].level=rank;
				pCrun->ctu[jp].FunIndex=pIndex[i];
				
				if(tagChar=='\0')
				{
					//----建立结构-------------
					pCrun->ctu[jp].pNext=NULL;
					//------------------------------

					pEndFlag[i]=1;
					if(++EndNum >= StringNum) break;
				}
				else
				{
					//NumChar=CountStringSameNumber(pStringGroup,rank,StringNum-i,&pIndex[i]);
					NumChar=CountCharSameNumber(pStringGroup,rank,StringNum-i,&pIndex[i],&pCountTable[i],NULL);
					if(NumChar==0)
					{
						TRACE( "TongJiShuJu Err pIndex[i][%d],rank[%d],tagChar[%c]",pIndex[i],rank,tagChar);
						return NULL;
					}
					//----------------------------------------------------------
					if(NumChar>1) ArrayStringGroup(pStringGroup,rank+1,NumChar,&pIndex[i]);
					
					//----建立结构-------------
					TypeChar=CountStringDifferentNumber(pStringGroup,rank+1,NumChar,&pIndex[i],&pCountTable[i]);
					pCrun->ctu[jp].pNext=malloc(sizeof(DfStringTreeNode)+TypeChar*sizeof(DfStringTreeUnit));
					pCrun=(DfStringTreeNode*)pCrun->ctu[jp].pNext;
					TRACE( "malloc[%x]",pCrun);
					pCrun->Number=TypeChar;	
					for(j=0;j<TypeChar;j++)
					{
						pCrun->ctu[j].FunIndex = 0xffff;
					}
					i += NumChar-1;
				}
			}
		}
		rank++;
	}
	free(pCountTable);
	free(pIndex);
	free(pEndFlag);
	return pStard;
}


void FreeStringTreeNode2(DfStringTreeNode *pStardNode)
{
	u16 i,Max,nlast,init=1;
	DfStringTreeNode *pNext,*pLast;
	TRACE("FreeStringTreeNode2 pStardNode[%x]",pStardNode);
	while(pStardNode)
	{
		if(init)
		{//---从头检索----
			pNext=pStardNode;
			pLast=NULL;
			init=0;
		}
		
		Max=pNext->Number;
		for(i=0;i<Max ;i++)
		{
			if(pNext->ctu[i].pNext)
			{
				//---保存上一结点信息------
				pLast=pNext;
				nlast = i;
				//----指向字节点-----------
				pNext=(DfStringTreeNode *)pNext->ctu[i].pNext;
				//----------
				break;
			}
		}
		if(i == Max)
		{//--------释放本节点----------
			TRACE("free[%x]",pNext);
			free(pNext);
			if(pLast==NULL) break;	//结束检索
			else{//---返回上一级检索------
				pLast->ctu[nlast].pNext=NULL;
				init=1;//从头开始检索
			}
		}
	}
}
*/




