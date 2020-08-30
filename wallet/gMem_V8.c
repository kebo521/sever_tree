//====================================================================
//====================================================================
/*****************************�����ڴ����******************************
//����-------  ���ڶ�̬���빫���ڴ�
//����-------  �˹���
//����ʱ��--20130604
//�޸�: ( �˹���) 20131213   --V2(���������ͷŵ��ڴ棬���û��ջ���)
//�޸�: ( �˹���) 20131213   --V3(�����ڴ������������ڣ�
								     �������ͷŵ�Ч�����53.00%(V2-59037,V3-27755))
//�޸�: ( �˹���) 20140612   --V4(���ӽṹ�����ϵ�ָ��,�ͷ��ڴ��ٶȴ��������)
//�޸�: ( �˹���) 20150504   --V5(���ӽṹ�����ϵ�ָ�����,�ͷ��ڴ��ٶȴ��������)
											�ۺ��������ͷŵ�Ч�����36.06%(V3-87243,V5-55786),V4���V3��������)
//�޸�: ( �˹���) 20161108   --V6(����Ԥ���ڴ�ռ�������ڴ�ռ�Ԥ���������ܣ�ʹ��Ч�ʲ��䣬���ʲ���)
//�޸�: ( �˹���) 20161118   --V7(����������й������б�(��С����)����һ�������ڴ�ʹ��Ч�ʣ�
									�ۺ��������ͷŵ�Ч�����29.1%(256:V6-7077,V7-5010),V6���V5��ͬ)
//�޸�: ( �˹���) 20170224   --V8(��V7��(12 byte)�����ϣ�����4���ֽ�(8 byte)�Ľڵ�ռ䣬��߿ռ������ʣ�
								    �ۺ��������ͷŵ�Ч�ʽ���13%(WinDow:10*10000*256:V7=6205,V8=6821)��
******************************************************************************/
#ifndef NULL
#define	NULL				(0)
#endif
#include "gMem.h"

//-------------------�ڴ�����ṹ----------------------------------------
typedef struct _DfFlagGuideRAM
{ //---��pNext�����м��λλ�ñ���Խ�����----------------------------- 
	struct _DfFlagGuideRAM  *pNfree;			//�����:��һ�����нڵ�(���һ���ڵ�ΪEND,ΪNULL��ʾռ��)
	unsigned short			uLast;				//�����:�����һ����ַ(��һ���ڵ�ΪNULL)
	unsigned short			uNext;				//�����:�����һ��㳤��(���һ���ڵ�ΪEND)(��һ���:MsgData+uNext)
	char 	MsgData[0];  //�����С,һ���������Ҫ����С�ռ�
} DfFlagGuideRAM;
//-------------------�ڴ����ָ�붨��----------------------------------------
static char* pAppRamGuideHead;		//--��ʼ��ַ---
static char* pAppRamGuideEnd;		//--�յ��ַ---
//-------------------���ٴ����������----------------------------------------
static DfFlagGuideRAM *pRamQuick;	//���ٴ��������е�ַ���
//-------------------������С��Ԫ---------------------------------
static unsigned uAppRamParticleSize;
//----------------�쳣������Ϣ��ʾ-----------
static PFUNC_SHOW_MSG pFunAbnormalMsg=NULL;
void gLoadFunAllocShowMsg(PFUNC_SHOW_MSG pFun)
{
	pFunAbnormalMsg=pFun;
}
#define SHOW_MEM_ALLOC_MSG(pTitle,pMsg,Mpar)		{if(pFunAbnormalMsg) (*pFunAbnormalMsg)(pTitle,pMsg,Mpar);}
//====================================================================
//����:     �˹���	---
//����:    ���½ڵ��������������б�
//��������:pGuideHead���м���Ľڵ�
//�������:��
//����ʱ��:  	20161118
//---------------------------------------------------------------
static void gIncreaseEmptyNode(DfFlagGuideRAM *pGuideRAM)
{
	DfFlagGuideRAM *pNext,*pLastfree;
	if((char*)pRamQuick == pAppRamGuideEnd)
	{
		pGuideRAM->pNfree =pRamQuick;
		pRamQuick=pGuideRAM;
		return;
	}
	pNext=pRamQuick;
	pLastfree=NULL;
	//---������С����ռ�ֲ�----
	while(1)
	{
		if(pGuideRAM->uNext <= pNext->uNext)
		{
			pGuideRAM->pNfree = pNext;
			if(pLastfree)
			{
				pLastfree->pNfree=pGuideRAM;
			}
			else
			{
				pRamQuick=pGuideRAM;
			}
			return;
		}
		if(pNext->pNfree == (DfFlagGuideRAM *)pAppRamGuideEnd)
		{
			pGuideRAM->pNfree =(DfFlagGuideRAM *)pAppRamGuideEnd;
			pNext->pNfree=pGuideRAM;
			return;
		}
		pLastfree=pNext;
		pNext=pNext->pNfree;
	}	
}
//====================================================================
//����:     �˹���	---
//����:    �������������б��Ƴ��ڵ�
//��������:pGuideHead�����Ƴ��Ľڵ�
//�������:��
//����ʱ��:  	20161118
//---------------------------------------------------------------
static void gDeleteEmptyNode(DfFlagGuideRAM *pGuideRAM)
{
	DfFlagGuideRAM *pNext,*pLastfree;
	pNext=pRamQuick;
	pLastfree=NULL;
	while(pNext != pGuideRAM)
	{
		pLastfree=pNext;
		pNext=pNext->pNfree;
	}	
	if(pLastfree)
	{
		pLastfree->pNfree=pGuideRAM->pNfree;
	}
	else
	{
		pRamQuick=pGuideRAM->pNfree;
	}
	pGuideRAM->pNfree=NULL;
}
//====================================================================
//����:     �˹���	---
//����:    �ͷ������ڵ㣬�������ڿսڵ�
//��������:pGuideRAM�����ͷŵĽڵ�
//�������:��
//����ʱ��:  	20161121
//---------------------------------------------------------------
static void gFreePhysicalNode(DfFlagGuideRAM *pGuideRAM)
{
	//------�ͷŽڵ�ռ�---------------------------
	if(((char*)pGuideRAM + pGuideRAM->uNext) != pAppRamGuideEnd)
	{
		DfFlagGuideRAM *pNext=(DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
		if(pNext->pNfree)
		{//------ɾ������(���)------------
			//------ɾ�����й���-------------
			gDeleteEmptyNode(pNext);
			//------ɾ��������------------
			pGuideRAM->uNext += pNext->uNext;
			if(((char*)pGuideRAM+ pGuideRAM->uNext) != pAppRamGuideEnd) 
				((DfFlagGuideRAM *)((char*)pGuideRAM+ pGuideRAM->uNext))->uLast =pGuideRAM->uNext;
		}
	}
	if(pGuideRAM->uLast != NULL)
	{
		DfFlagGuideRAM *pLast=(DfFlagGuideRAM *)((char*)pGuideRAM  - pGuideRAM->uLast);;
		if(pLast->pNfree)//(pGuideRAM->pLast->pLast==NULL)
		{//------ɾ������(��ǰ)------------
			//------ɾ�����й���-------------
			gDeleteEmptyNode(pLast);
			//------ɾ��������------------
			pLast->uNext += pGuideRAM->uNext;
			if (((char*)pLast + pLast->uNext) != pAppRamGuideEnd)
				((DfFlagGuideRAM *)((char*)pLast + pLast->uNext))->uLast = pLast->uNext;
			pGuideRAM=pLast;
		}
	}
	gIncreaseEmptyNode(pGuideRAM);
}

//====================================================================
//����:     �˹���	---
//����:    �ָ�ռ�
//��������:pGuideRAM��Ҫ�ָ�Ŀռ�ڵ㣬silen Ҫ�ָ�Ĵ�С
//�������:��
//����ʱ��:  	20170217
//---------------------------------------------------------------
void gMemSplitSpace(DfFlagGuideRAM *pGuideRAM,unsigned silen)
{
	DfFlagGuideRAM *pNext,*pNew;
	pNext=(DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
	pGuideRAM->uNext=silen;
	pNew=(DfFlagGuideRAM *)((char*)pGuideRAM + silen);
	pNew->uLast=silen;
	pNew->uNext=(char*)pNext-(char*)pNew;
	if(((char*)pNext) != pAppRamGuideEnd)
	{
		if(pNext->pNfree)
		{
			//------ɾ�����й���-------------
			gDeleteEmptyNode(pNext);
			//------ɾ������(���)------------
			pNew->uNext += pNext->uNext;
			if(((char*)pNext + pNext->uNext) != pAppRamGuideEnd)
				((DfFlagGuideRAM *)((char*)pNext + pNext->uNext))->uLast = pNew->uNext;
		}	
		else
		{
			pNext->uLast = pNew->uNext;
		}
	}
	gIncreaseEmptyNode(pNew);
}
//====================================================================
//����:     �˹���	---
//����:    ���ڳ�ʼ���ڴ�ʹ�õ�����
//��������:mem_addr������������׵�ַ,mem_size,����ʹ�õ��ڴ��С,particle_size ��С���뵥Ԫ,ȡֵΪ2^(2+n)
//�������:��
//����ʱ��:  	20130604
//---------------------------------------------------------------
int gMemAllocInit(void* mem_addr,unsigned mem_size,unsigned particle_size)
{
	if(((unsigned)mem_addr)&(sizeof(char*)-1)) 
	{
		pAppRamGuideEnd=NULL;
		pRamQuick=NULL;
		SHOW_MEM_ALLOC_MSG("�ڴ�����ʼ��","�ڴ�[%x]����Ҫ��",(int)mem_addr);
		return -1;
	}
	if(mem_size>0xffff)
	{	
		pAppRamGuideEnd=NULL;
		pRamQuick=NULL;
		SHOW_MEM_ALLOC_MSG("�ڴ�����ʼ��","������[%x]����",(int)mem_size);
		return -2;
	}
	pAppRamGuideHead=(char*)mem_addr;//(mem_addr+3)&(~3);
	pAppRamGuideEnd=pAppRamGuideHead+mem_size;
	//-------------���ٴ���------------
	pRamQuick=(DfFlagGuideRAM *)pAppRamGuideHead;
	pRamQuick->pNfree=(DfFlagGuideRAM *)pAppRamGuideEnd;//����������Ŀ���
	pRamQuick->uNext=mem_size;
	pRamQuick->uLast=NULL;	//��һ��ָ���
	//------------��С��Ԫ------------------
	uAppRamParticleSize=4-1;
	{
		unsigned bit3,i,Max;
		for(bit3=0x00000004,Max=0;Max<32;Max++)
			if((bit3<<Max) >= mem_size) break;
		for(bit3=0x00000004,i=0;i<Max;i++)
			if((bit3<<i) >= particle_size) break;
		if(i == Max) return 1;
			particle_size = (bit3<<i);
		if(particle_size >= mem_size) return 2;
	}
	uAppRamParticleSize = particle_size-1;
	return 0;
}

//====================================================================
//����:    ��������̶��ڴ湦�ܣ�
//��������:silen ��Ҫ�����ڴ��С
//�������:������������׵�ַ������ʧ��ʱ����Ϊ�յ�ַ
//����:     �˹���	---	
//����ʱ��:  	20140612	-V4
//---------------------------------------------------------------
void *gMalloc(unsigned silen)
{
	DfFlagGuideRAM *pGuideRAM,*pLastfree;//register 
	//------���볤������4������(����ָ��ƫ��)--------------
	silen =sizeof(DfFlagGuideRAM)+((silen+uAppRamParticleSize)&(~uAppRamParticleSize)); 
	//-----�������-------
	pGuideRAM=pRamQuick;
	pLastfree=NULL;
	while((char*)pGuideRAM != pAppRamGuideEnd)
	{
		if(pGuideRAM->uNext >= silen)
		{
			//------ɾ�����й���=gDeleteEmptyNode--------
			if(pLastfree) 
				pLastfree->pNfree=pGuideRAM->pNfree;
			else //if(pRamQuick==pGuideRAM) 
				pRamQuick=pGuideRAM->pNfree;
			pGuideRAM->pNfree=NULL;	//����ָ��Ϊ�գ��ǿ��У���ʾռ�á�
			//--------����Ƿ�����ٷָ�--------------
			if(pGuideRAM->uNext > (silen+sizeof(DfFlagGuideRAM)))
			{//---���ڵĿռ䣬��������һ�����ݿռ���--------
				DfFlagGuideRAM *pNext,*pNew;
				pNext=(DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
				pGuideRAM->uNext=silen;
				pNew=(DfFlagGuideRAM *)((char*)pGuideRAM + silen);
				pNew->uLast = silen;
				pNew->uNext = (char*)pNext - (char*)pNew;
				if(((char*)pNext) != pAppRamGuideEnd)
				{
					pNext->uLast = pNew->uNext;
				}
				gIncreaseEmptyNode(pNew);
			}
			return (void*)pGuideRAM->MsgData;
		}
		pLastfree=pGuideRAM;
		pGuideRAM=pGuideRAM->pNfree;
	}
	SHOW_MEM_ALLOC_MSG((char*)"�ڴ�����",(char*)"����%d�ռ䲻��",silen);
//	FreeCheckRAM();
	return NULL;
}

//====================================================================
//����:    �����ͷ�֮ǰ������ڴ�
//��������:pfree������������׵�ַ�������ͷŶ�Ӧ�õ�����
//�������:��
//����:     �˹���	---	
//����ʱ��:  	20140612	-V4
//---------------------------------------------------------------
void gFree(void *pfree)
{
	DfFlagGuideRAM *pGuideRAM;//,*pLast;//register 
	//---------�������ָ�����Ч��---------------------------------------
	if((char*)pfree <pAppRamGuideHead || (char*)pfree >pAppRamGuideEnd) return;
	pGuideRAM=(DfFlagGuideRAM *)(((char*)pfree)-(sizeof(DfFlagGuideRAM)));
	//---------����Ѿ��ͷ�ֱ�ӷ���---------
	if(pGuideRAM->pNfree)
	{
		SHOW_MEM_ALLOC_MSG((char*)"�ڴ��ͷ�",(char*)"�ظ��ͷ�%xͬһ�ռ�",(int)pfree);
		return;
	}
	gFreePhysicalNode(pGuideRAM);
//	TRACE("find OVER_[%x]\r\n",pfree);
//	UI_ShowInfo("find OVER");
}
//====================================================================
//����:    ����Ԥ��һ�������ڴ�ռ䣬
//��������:sPreLen ��ҪԤ���ڴ��С(0Ϊϵͳ���ռ�)
//�������:������������׵�ַ������ʧ��ʱ����Ϊ�յ�ַ��OutSizeԤ�ڿռ����ֵ
//����:     �˹���	---	
//����ʱ��:  	20161108	-V6
//---------------------------------------------------------------
void *gPralloc(unsigned sPreLen,unsigned *OutSize)
{
	DfFlagGuideRAM *pGuideRAM,*pLastfree;//register 
	if((char*)pRamQuick == pAppRamGuideEnd)
	{
		SHOW_MEM_ALLOC_MSG((char*)"�ڴ�Ԥ��",(char*)"�޿��пռ�[%d]",sPreLen);
		return NULL;
	}
	//-----�������-------
	pGuideRAM=pRamQuick;
	pLastfree=NULL;
	if(sPreLen)
	{
		//-���볤������4������(����ָ��ƫ��)-
		sPreLen =sizeof(DfFlagGuideRAM)+((sPreLen+uAppRamParticleSize)&(~uAppRamParticleSize)); 
		while((char*)pGuideRAM != pAppRamGuideEnd)
		{
			if(pGuideRAM->uNext >= sPreLen)
			{
				//---ɾ�����й���=gDeleteEmptyNode--------
				if(pLastfree) 
					pLastfree->pNfree=pGuideRAM->pNfree;
				else //if(pRamQuick==pGuideRAM) 
					pRamQuick=pGuideRAM->pNfree;
				pGuideRAM->pNfree=NULL; //����ָ��Ϊ��(�ǿ���)��ʾռ�á�
				//-------�������------------
				if(pGuideRAM->uNext > (sPreLen+sizeof(DfFlagGuideRAM)))
				{//---���ڵĿռ䣬��������һ�����ݿռ���--------
					DfFlagGuideRAM *pNext,*pNew;
					pNext=(DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
					pGuideRAM->uNext=sPreLen;
					pNew=(DfFlagGuideRAM *)((char*)pGuideRAM + sPreLen);
					pNew->uLast = sPreLen;
					pNew->uNext = (char*)pNext - (char*)pNew;
					if(((char*)pNext) != pAppRamGuideEnd)
					{
						pNext->uLast= pNew->uNext;
					}
					gIncreaseEmptyNode(pNew);
				}
				*OutSize = pGuideRAM->uNext - sizeof(DfFlagGuideRAM);
				return (void*)pGuideRAM->MsgData; //�践�صĿռ��ַ;
			}
			pLastfree=pGuideRAM;
			pGuideRAM=pGuideRAM->pNfree;
		}
	}
	else
	{
		while((char*)pGuideRAM->pNfree != pAppRamGuideEnd)
		{
			pLastfree=pGuideRAM;
			pGuideRAM=pGuideRAM->pNfree;
		}
		//---ɾ�����й���=gDeleteEmptyNode--------
		if(pLastfree) 
			pLastfree->pNfree=pGuideRAM->pNfree;
		else //if(pRamQuick==pGuideRAM) 
			pRamQuick=pGuideRAM->pNfree;
		pGuideRAM->pNfree=NULL; //����ָ��Ϊ��(�ǿ���)��ʾռ�á�
		//----���ÿ���״̬-----------
		*OutSize= pGuideRAM->uNext - sizeof(DfFlagGuideRAM);
		return (void*)pGuideRAM->MsgData;
	}
	SHOW_MEM_ALLOC_MSG((char*)"�ڴ�Ԥ��",(char*)"����%d�ռ䲻��",sPreLen);
	return NULL;
}

//====================================================================
//����:    ��������ڴ�ռ��Ԥ�ڣ�
//��������:pMemory��Ԥ�ڵ��ڴ��ַ   silen ��ҪԤ���ڴ��С(0�ͷ�Ԥ��)
//�������:���ؽ��(0��ʾ�ɹ�)
//����:     �˹���	---	
//����ʱ��:  	20161108	-V6
//---------------------------------------------------------------
void* gRealloc(void *pMemory,unsigned silen)
{
	DfFlagGuideRAM *pGuideRAM;//register 
	//---------�������ָ�����Ч��---------------------------------------
	if((char*)pMemory <pAppRamGuideHead || (char*)pMemory >pAppRamGuideEnd)
	{
		return NULL;
	}
	pGuideRAM=(DfFlagGuideRAM *)(((char*)pMemory)-(sizeof(DfFlagGuideRAM)));
	//---------����Ѿ��ͷ�ֱ�ӷ���---------
	if(pGuideRAM->pNfree)
	{
		SHOW_MEM_ALLOC_MSG((char*)"�ڴ����·���",(char*)"�ռ�[%x]�Ѿ��ͷ�",(int)pMemory);
		return NULL;
	}
	
	if(silen)
	{
		silen =sizeof(DfFlagGuideRAM) + ((silen+uAppRamParticleSize)&(~uAppRamParticleSize)); 
		if(pGuideRAM->uNext < silen)
		{
			if(((char*)pGuideRAM + pGuideRAM->uNext) != pAppRamGuideEnd)
			{
				DfFlagGuideRAM *pNext=(DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
				if(pNext->pNfree)
				{//�����һ���ռ䣬�Ƿ�Ϊ��
					unsigned Len2 = pGuideRAM->uNext + pNext->uNext;
					if(Len2 >= silen)	//�ռ乻
					{
						gDeleteEmptyNode(pNext);
						pGuideRAM->uNext += pNext->uNext;
						if(((char*)pGuideRAM + pGuideRAM->uNext) != pAppRamGuideEnd)
						{
							((DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext))->uLast=pGuideRAM->uNext;
						}
						if(Len2 > (silen+sizeof(DfFlagGuideRAM)))
						{
							gMemSplitSpace(pGuideRAM,silen);
						}
						return pMemory; //���ı��ַ
					}
				}
			}
			//-----���пռ�����ܺϲ��ռ��޷�����-----
			{
				unsigned *p1,*p2,Max;
				p1=(unsigned *)gMalloc(silen);
				if(p1)
				{
					p2=(unsigned *)pGuideRAM->MsgData;
					//-----�ָ�ԭ������----------
					Max=(pGuideRAM->uNext-sizeof(DfFlagGuideRAM))/sizeof(unsigned);
					while(Max--) p1[Max]=p2[Max];
				}
				//-----Ԥ��ȡ��-----�ͷſռ�----
				gFreePhysicalNode(pGuideRAM);
				return p1; //�ı��ַ��������������ڷ���
			}
		}
		else 
		{
			if(pGuideRAM->uNext > (silen+sizeof(DfFlagGuideRAM)))
			{//---���ڵĿռ䣬��������һ�����ݿռ���--------
				gMemSplitSpace(pGuideRAM,silen);
			}
			return pMemory; //���ı��ַ
		}
	}
	//-----Ԥ��ȡ��-----�ͷſռ�----
	gFreePhysicalNode(pGuideRAM);
	return NULL;
}

  


//====================================================================
//����:    ������ʾ����ʹ�����(��Ҫ���ڵ���)
//��������:��
//�������:��
//����:     �˹���	---	
//����ʱ��:  	20130604
//---------------------------------------------------------------
int gCheckAllocStatus(char *pStrout,int *pNum,int *pMaxNum,int *UseLen)
{
	int MaxNum=0,flag=0,gErr=0,uNum=0;
//	char OutStr[20];
	unsigned ULen=0;
	DfFlagGuideRAM *pGuideRAM;
	pGuideRAM=(DfFlagGuideRAM *)pAppRamGuideHead;
	if(pStrout)
	{
		*pStrout++ ='V';
		*pStrout++ ='8';
		*pStrout++ =':';
		*pStrout++ ='[';
	}
	while((char*)pGuideRAM != pAppRamGuideEnd)
	{
		MaxNum++;
		if(pGuideRAM->pNfree)
		{
			if(pStrout) *pStrout++ ='_';
			if(flag) gErr++;
			else flag=1;
		}
		else 
		{
			if(pStrout) *pStrout++ ='8';
			ULen += pGuideRAM->uNext-sizeof(DfFlagGuideRAM);
			uNum++;
			flag=0;
		}
		pGuideRAM = (DfFlagGuideRAM *)((char*)pGuideRAM + pGuideRAM->uNext);
	}
	if(pNum) *pNum=uNum;
	if(pMaxNum) *pMaxNum=MaxNum;
	if(UseLen) *UseLen=ULen;
	
	if(pStrout)
	{
		*pStrout++ =']';
		*pStrout++ ='\0';
	}
	return gErr;
}



