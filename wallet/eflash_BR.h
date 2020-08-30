
#define GFS_BASIC_NULL		0xFFFF	//无数
#define GFS_BASIC_GUID		0xEEEE	//引导
#define GFS_BASIC_DATA		0xDDDD	//数据
#define GFS_BASIC_BAD		0x4884	//坏块

typedef struct
{
	u16	head;	//块头:[FFFF] 已经察除，[48||84] 坏块，[EEEE] 引导数据，[DDDD] 使用数据
	u16	pgSize;	//块大小
	int count;
}eFlashCheck;
//=====================================================================
#define GFS_PAGE_SIZE			128

#define GFS_COUNTER_ADDR			(DCBC_END_BASE)
#define GFS_COUNTER_SIZE			(DCBC_PAGE_SIZE*2)

//====================================流水记录 可以查记录===================================
typedef struct
{
	int count;			//写入计数器
	u16 blockStart;		//文件引导区地址，平衡记数器
	u16 parLen;			//参数长度	= aSize
	u16 pgSize;			//页数长度	= 256 *((aSize+255)/256)
	u16 PageNum;		//单页数量  	= 4096/pgSize
	u16 PageMax;		//最大单页	= PageNum * block
	u16 PageIndex;		//文件引导区平衡记数器
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

