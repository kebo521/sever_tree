/*
 * The gTree filesystem
 *
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GFS_H_
#define GFS_H_

#define GFS_BASIC_NULL		0xFFFF	//����
#define GFS_BASIC_GUID		0xEEEE	//����
#define GFS_BASIC_DATA		0xDDDD	//����
#define GFS_BASIC_BAD		0x4884	//����


#define GFS_INDEX_NULL 		GFS_BASIC_NULL
#define GFS_BLOCK_SIZE 		512		//4096
#define GFS_PAGE_SIZE 		4		//256


#define GFS_BIT_FLAG 		8	//32
#define GFS_FILE_MAX 		92	//124


// File types �ļ��б��	[f]�ļ���,[e]��ִ���ļ���[s]�ַ����ļ���[b]�������ļ�
enum fType {
    GFS_TYPE_FLD	= 'f',	//Folder
    GFS_TYPE_BIN	= 'b',
	GFS_TYPE_STR	= 's',
	GFS_ENC_STR		= 'd',	//����Կ�ļ�
	GFS_TYPE_EXE	= 'e',	//��ִ���ļ�
};

typedef struct
{
	u16	head;	//��ͷ:[FFFF] �Ѿ������[48||84] ���飬[EEEE] �������ݣ�[DDDD] ʹ������
	u16	index;	//ƽ���¼�� -> �ļ�
	u8	flagG;			// 'G'	//�������
	u8	flagClear;		//�Ѿ����,[FF]δ�����55 �Ѿ����
	u16	BrIndex;		//�������ļ�ĸ���
}DfGTreeFsBlockHead;


//----------�����ļ�ϵͳ��ͷ--HEAD--256-----
typedef struct
{
	DfGTreeFsBlockHead t;
	u16		BlockIndex[GFS_FILE_MAX];	//һ����ڣ�ֱ�Ӷ�Ӧ�ļ����ļ���
	//-------64byte-----------
	u32		NullBit[GFS_BIT_FLAG];	//�տ� block
	u32		EraseBit[GFS_BIT_FLAG];	//��Ҫ����Ŀ�
}DfGTreeFsGlobal;


typedef struct
{
	u16 gfsCoutStart;	//�ļ���������ַ��ƽ�������
	u16 gfsCoutSize;	//�ļ��������ռ��С
	u16 gfsCoutIndex;	//�ļ�������ƽ�������
	u16 gfsBlockStart;	//�ļ�������ʼ��ַ
	u16 gfsBlockSize;	//�ļ����ݿռ��С
	u16 gfsNewIndex;	//�ļ��ռ�ƽ�������
	DfGTreeFsGlobal g;	//�ļ����������ݣ�����ƽ���������
}DfGTreeFsBasi;

//=================�����ļ���========================
#define GTREE_FS_NAME_MAX	32
#define GTREE_FS_UNIT_SIZE	256	//2048

typedef struct
{
	char	filepath[GTREE_FS_NAME_MAX-4];	//�ļ��������ļ���
	char	check[3];	//�����ļ�У��ֵ
	char	fType;		//fType�ļ��б��	[f]�ļ���,[e]��ִ���ļ���[s]�ַ����ļ���[b]�������ļ�
}DfGTreeFsFileName;

//----------�����ļ�ϵͳ����--DATA-----------
typedef struct	//	GTREE_FS_UNIT_SIZE
{
	DfGTreeFsFileName name;
	u32		InvLen;		//�������ȣ��ļ������м���FLASH �����ȡ��(~InvLen)
	u16 	fdIndex[(GTREE_FS_UNIT_SIZE-GTREE_FS_NAME_MAX-4)/2]; //�ļ����ݿ�����
}DfgFsUnitHead;

typedef struct	//	GFS_BLOCK_SIZE
{
	DfgFsUnitHead h;  //	GTREE_FS_UNIT_SIZE
	u8	fData[GFS_BLOCK_SIZE-GTREE_FS_UNIT_SIZE];	//�ļ����ݣ����2048
}DfgFsUnitBlock;

enum FlagTagH {
    FLAG_ERASE		= 0x08,	
    FLAG_UPTITLE	= 0x04,
    FLAG_NET_HEAD	= 0x02,
    FLAG_SWITCH_D	= 0x80,
    FLAG_NEW_DATA	= 0x40,
    FLAG_NEW_GUIDE	= 0x10,
};

typedef struct	
{
	u32		fLen;	//�ļ���ǰ���볤�ȣ�ȡ��(~fInvLen)
	u32		fOffset;	//�ļ���ǰƫ��
	u16 	IndexGuide;	//���������
	u16 	IndexEntry;	//�ļ����
	
	u16 	fOpenFlag;
	u16 	IndexLast;	//��һ������Ŀռ�
	
	u16 	IndexB;		//������
	u16 	bOffet;		//��ƫ��	

	u8		flagUpT;	//���±���
	u8		flagUpH;	//����ͷ����,0x80 ���飬0x40 �������飬0x20 ��д�붯����0x10 �и���ǰ������
	u8		flagUpE;	//���²�����
	u8		flagNew;	//����д������
	//----------Block ������ݼ�-[GFS_BLOCK_SIZE]-----------
	DfgFsUnitBlock u;
}gFILE,*LPgFILE;	//�ļ����

#define 	GFS_PATH_TAG				('/')
//========�ļ�·�����===���� 1,�ļ�·����-1���ļ��У��ַ���ƥ�䣬0 ƥ���ַ�------
extern void memcpy_u32(u32* pU1,u32* pU2,int len);
extern void memset_u32(u32* pU1,u32 tag,int len);
extern int memcmp_u32(u32* pU1,u32* pU2,int len);


//extern int gfs_BalancedGlobalInit(void);
extern int gfs_BalancedGlobalSave(int pNew);

extern int gfs_FsReadblock(u16 block,u16 offset,void* buff,u16 size);
extern int gfs_FsWriteblock(u16 block,u16 offset,void* buff,u16 size);
extern int gfs_FsEraseblock(u16 block);

extern int gfs_RecordBlockIndex(u16 i,u16 par);
extern u16 gfs_GetBlockIndex(u16 i);
extern u16 gfs_GetNewBlockNum(void);
extern u16 gfs_ScanFile_BlockIndex(DfGTreeFsFileName *pName,const char *path,char pathType);

extern void gfs_RecordDelete(u16* pIndexArray,int max);
extern void gfs_UndoRecordDelete(u16* pIndexArray,int max);
extern int gfs_CheckEraseBlockBit(void);
extern u16 gfs_GetNextNullBlock(void);

extern int gfs_Init(u16 startCap,u16 fdSize,u16 countSize);
extern int gfs_Fomat(char *pKey);

extern int gfs_SetOffset(gFILE *pfd,int offset, int whence);

//extern gFILE* gfs_RetrieveFile(const char *path, int flags);
//extern gFILE* gfs_RetrievePath(const char *path, int flags);

extern int gfs_checkALL(void);

#endif
