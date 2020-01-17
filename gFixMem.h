/*****************************�ڴ����******************************
//����-------  ���ڶ�����̬���빫���ڴ�
//����-------  �˹���
//����ʱ��--20190402
******************************************************************************/

#ifndef _RAM_MANAGEMENT_
#define _RAM_MANAGEMENT_

#ifdef __cplusplus
extern "C" {
#endif


//====================================================================
//����:    ���ڳ�ʼ���ڴ����ռ� (���������ڴ����Ŀռ�)
//��������:mem_addr������������׵�ַ,mem_size,����ʹ�õ��ڴ��С,particle_size ��С���뵥Ԫ,ȡֵΪ2^(2+n)
//�������:����Ϊ0�ɹ�������ʧ��
//����ʱ��:  	20130604
//---------------------------------------------------------------
extern int gMemAllocInit(void* mem_addr,unsigned mem_size,unsigned particle_size);

//====================================================================
//����:    �������������ڴ�ռ�(ʵ�ֱ�׼ malloc)
//��������:silen ��Ҫ�����ڴ��С
//�������:������������׵�ַ��ʹ��ʱ�����ܳ������뷶Χ��������ʧ��ʱ����Ϊ�յ�ַ,
//---------------------------------------------------------------
extern void *gfixMalloc(void);

//====================================================================
//����:    �����ͷ���������ڴ�(ʵ�ֱ�׼ free)
//��������:pfree������������׵�ַ��Ϊ�ղ�������
//---------------------------------------------------------------
extern void gfixFree(void *pfree);

//====================================================================
//����:    ������ʾ����ʹ�þ���(��Ҫ���ڵ���)
//---------------------------------------------------------------
extern int gCheckAllocStatus(char *pStrout,int *pNum,int *pMaxNum,int *UseLen);

//==============��ʾ��Ϣ�ṹ��ʾ========================================
typedef void (*PFUNC_SHOW_MSG)(char*,char*,int);	//pTitle,pMsg,par
/* demo
void UI_ShowMemMsg(char* pTitle,char* pMsg,int Mpar)
{
	char showErr[24];
	API_sprintf(showErr,pMsg,Mpar);
	APP_ShowMsg(pTitle,showErr,10000);
}
*/
//====================================================================
//����:    �쳣��Ϣ������ܺ���
//��������:pFun ��Ϣ�쳣ʱ����Ҫ������ܲ���
//�������:��
//ע:���ܺ����ṹ(pTitle,pMsg,Mpar)�ֱ�����ʾ���⣬��ʾ���ݣ��ṹ����ʾ����
//---------------------------------------------------------------
void gLoadFunAllocShowMsg(PFUNC_SHOW_MSG pFun);



#ifdef __cplusplus
}
#endif

#endif	//_RAM_MANAGEMENT_