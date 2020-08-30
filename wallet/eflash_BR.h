
#define GFS_BASIC_NULL		0xFFFF	//����
#define GFS_BASIC_GUID		0xEEEE	//����
#define GFS_BASIC_DATA		0xDDDD	//����
#define GFS_BASIC_BAD		0x4884	//����

typedef struct
{
	u16	head;	//��ͷ:[FFFF] �Ѿ������[48||84] ���飬[EEEE] �������ݣ�[DDDD] ʹ������
	u16	pgSize;	//���С
	int count;
}eFlashCheck;
//=====================================================================
#define GFS_PAGE_SIZE			128

#define GFS_COUNTER_ADDR			(DCBC_END_BASE)
#define GFS_COUNTER_SIZE			(DCBC_PAGE_SIZE*2)

//====================================��ˮ��¼ ���Բ��¼===================================
typedef struct
{
	int count;			//д�������
	u16 blockStart;		//�ļ���������ַ��ƽ�������
	u16 parLen;			//��������	= aSize
	u16 pgSize;			//ҳ������	= 256 *((aSize+255)/256)
	u16 PageNum;		//��ҳ����  	= 4096/pgSize
	u16 PageMax;		//���ҳ	= PageNum * block
	u16 PageIndex;		//�ļ�������ƽ�������
}DfBalancedRecord;

extern int BalancedAccountInit(DfBalancedRecord *pAccount,u16 BlockStart,u16 BlockNum,u16 aSize);
extern int BalancedAccountRead(DfBalancedRecord *pAccount,int PreviousIndex,void* pBuff);
extern int BalancedAccountSave(DfBalancedRecord *pAccount,void* pBuff);



extern void* fOpen_Record(u32 uAddress,u32 uLen,void *pData,int size);
extern int fRead_Record(void *fd,void *pData);
extern int fWrite_Record(void *fd,void *pData);
extern int fClear_Record(void *fd);
extern int fClose_Record(void *fd);

/*
extern int LIFO_RecordInit(void *pData,int size);
extern int LIFO_RecordRead(void *pData,int index);
extern int LIFO_RecordWrite(void *pData);
*/

