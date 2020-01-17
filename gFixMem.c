//====================================================================
//====================================================================
/*****************************���������ڴ����******************************
//����-------  ���ڶ�����̬���빫���ڴ�
//����-------  �˹���
//����ʱ��--20190402
******************************************************************************/
#ifndef NULL
#define	NULL				(0)
#endif
//#include "gFixMem.h"

//-------------------�ڴ�����ṹ----------------------------------------
typedef struct _DfFlagGuideRAM
{ //---��pNext�����м��λλ�ñ���Խ�����----------------------------- 
	struct _DfFlagGuideRAM  *pNfree;			//�����:��һ�����нڵ�(���һ���ڵ�ΪEND,ΪNULL��ʾռ��)
} DfFlagGuideRAM;
//-------------------�ڴ����ָ�붨��----------------------------------------
static DfFlagGuideRAM *pAppRamGuideHead;		//--��ʼ��ַ---
static DfFlagGuideRAM *pAppRamGuideEnd;			//--�յ��ַ---
//-------------------���ٴ����������----------------------------------------
static DfFlagGuideRAM *pRamQuick=NULL;	//���ٴ��������е�ַ���
//----------------�쳣������Ϣ��ʾ-----------
static PFUNC_SHOW_MSG pFunAbnormalMsg=NULL;
void gLoadFunAllocShowMsg(PFUNC_SHOW_MSG pFun)
{
	pFunAbnormalMsg=pFun;
}
#define SHOW_MEM_ALLOC_MSG(pTitle,pMsg,Mpar)		{if(pFunAbnormalMsg) (*pFunAbnormalMsg)(pTitle,pMsg,Mpar);}
//====================================================================
//����:     �˹���	---
//����:    ���ڳ�ʼ���ڴ�ʹ�õ�����
//��������:mem_addr������������׵�ַ,mem_size,����ʹ�õ��ڴ��С,particle_size ��С���뵥Ԫ,ȡֵΪ2^(2+n)
//�������:��
//����ʱ��:  	20130604
//---------------------------------------------------------------
int gMemAllocInit(void* mem_addr,unsigned mem_size,unsigned particle_size)
{
	unsigned i,num;
	DfFlagGuideRAM *pNext;
	if(((unsigned)mem_addr)&(sizeof(char*)-1)) 
	{
		SHOW_MEM_ALLOC_MSG("�ڴ�����ʼ��","�ڴ�[%x]����Ҫ��",(int)mem_addr);
		return -1;
	}
	particle_size = (particle_size+3)&(~3);
	if(mem_size < particle_size)
	{
		SHOW_MEM_ALLOC_MSG("�ڴ�����ʼ��","����[m,%d < p,%d]",(int)mem_size,particle_size);
		return 0;
	}
	pAppRamGuideHead=(DfFlagGuideRAM *)mem_addr;		//(mem_addr+3)&(~3);
	pAppRamGuideEnd=(char*)pAppRamGuideHead + mem_size;			//--�յ��ַ---
	//------------��С��Ԫ------------------
	i=particle_size;
	num=0;
	pNext=pAppRamGuideHead;
	while(1)
	{
		pNext->pNfree = (DfFlagGuideRAM *)((char*)pNext + i);
		num++;
		i += particle_size;
		if(i > mem_size) break;
		pNext = pNext->pNfree;
	}
	pNext->pNfree=NULL;
	//-------------���ٴ���------------
	pRamQuick= pAppRamGuideHead;
	return num;
}

//====================================================================
//����:    ��������̶��ڴ湦�ܣ�
//��������:��
//�������:������������׵�ַ����СΪparticle_size
//---------------------------------------------------------------
void *gfixMalloc(void)
{
	register DfFlagGuideRAM *pGuideRAM=pRamQuick;
	//-----�������-------
	if(pGuideRAM==NULL)
	{
		SHOW_MEM_ALLOC_MSG("�ڴ�����","�ռ�������%x",pGuideRAM);
		return pGuideRAM;
	}
	pRamQuick=pGuideRAM->pNfree;
	return pGuideRAM;
}

//====================================================================
//����:    �����ͷ�֮ǰ������ڴ�
//��������:gfixFree������������׵�ַ�������ͷŶ�Ӧ�õ�����
//�������:��
//---------------------------------------------------------------
void gfixFree(void *pfree)
{
	register DfFlagGuideRAM *pGuideRAM=(DfFlagGuideRAM *)pfree;
	//---------�������ָ�����Ч��---------------------------------------
	if(pGuideRAM <pAppRamGuideHead || pGuideRAM >=pAppRamGuideEnd) 
	{
		SHOW_MEM_ALLOC_MSG("�ڴ��ͷ�","�ڴ�[%x]�Ƿ�",pfree);
		return;
	}
	pGuideRAM->pNfree = pRamQuick;
	pRamQuick=pGuideRAM;
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
	int MaxNum=0;
	DfFlagGuideRAM *pGuideRAM;
	pGuideRAM=pRamQuick;
	if(pStrout)
	{
		*pStrout++ ='V';
		*pStrout++ ='F';
		*pStrout++ =':';
		*pStrout++ ='[';
	}
	while(pGuideRAM != NULL)
	{
		MaxNum++;
		if(pStrout) *pStrout++ ='_';
		pGuideRAM = pGuideRAM->pNfree;
	}
	
	if(pStrout)
	{
		*pStrout++ =']';
		*pStrout++ ='\0';
	}
	return MaxNum;
}



