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

#define GFS_BLOCK_SIZE 4096
#define GFS_INDEX_NULL 0xFFFF


// File types �ļ��б��	[f]�ļ���,[e]��ִ���ļ���[s]�ַ����ļ���[b]�������ļ�
enum fType {
    GFS_TYPE_FLD	= 'f',	//Folder
    GFS_TYPE_BIN	= 'b',
	GFS_TYPE_STR	= 's',
	GFS_TYPE_EXE	= 'e',
};

typedef struct
{
	u16	head;	//��ͷ:[FFFF] �Ѿ������[48||84] ���飬[EEEE] �������ݣ�[DDDD] ʹ������
	u16	index;	//ƽ���¼��
}DfGTreeFsBlockHead;


//----------�����ļ�ϵͳ��ͷ--HEAD--512-----
typedef struct
{
	DfGTreeFsBlockHead t;
	u16		BlockIndex[124];	//һ����ڣ�ֱ�Ӷ�Ӧ�ļ����ļ���
	u16		BlockPath;			//������ڣ�ָ�������֧����BlockIndex����ʱʹ�á�
	u8		flagNot;
	u8		flagClear;		//�Ѿ����,[FF]δ�����55 �Ѿ����
	//-------256byte-----------
	u32		NullBit[32];	//�տ� block
	u32		EraseBit[32];	//��Ҫ����Ŀ�
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
#define GTREE_FS_UNIT_SIZE	2048

typedef struct
{
	char	filepath[GTREE_FS_NAME_MAX-1];	//�ļ��������ļ���
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


typedef struct	
{
	u32		fInvLen;	//�ļ���ǰ���볤�ȣ�ȡ��(~fInvLen)
	u32		fOffset;	//�ļ���ǰƫ��
	u16 	fOpenFlag;
	u16 	IndexT;		//���������
	
	u16 	IndexB;		//������
	u16 	bOffet;		//��ƫ��	
	
	u8		flagUpT;	//���±���
	u8		flagUpH;	//����ͷ����
	u8		flagUpE;	//���²�����
	u8		flagNew;	//����д������
	//----------Block ������ݼ�-[GFS_BLOCK_SIZE]-----------
	DfgFsUnitBlock u;
}DfGFsUnitfd;	//�ļ����

#define 	GFS_PATH_TAG				('/')
//========�ļ�·�����===���� 1,�ļ�·����-1���ļ��У��ַ���ƥ�䣬0 ƥ���ַ�------
extern int gfs_strcmp_path(u8* pPath,u8* strTag,u8 pathTag);
extern void memcpy_u32(u32* pU1,u32* pU2,int len);
extern void memcpy_u8(u8* pU1,u8* pU2,int len);
extern void memset_u32(u32* pU1,u32 tag,int len);
extern void memset_u8(u8* pU1,u8 tag,int len);
extern void strcpy_char(char* pS1,char* pS2);
extern int memcmp_u32(u32* pU1,u32* pU2,int len);


extern u32 gfs_erase_times;

extern int gfs_BalancedGlobalInit(DfGTreeFsGlobal *pGlobal);
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
extern int gfs_RecordBlockEraseBit(u16 EraseBlock);
extern int gfs_CheckEraseBlockBit(int initFalg);
extern u16 gfs_GetNextNullBlock(void);

extern int gfs_Init(u16 startCap,u16 fdSize,u16 countSize);
extern int gfs_Fomat(char *pKey);

extern int gfs_SetOffset(DfGFsUnitfd *pfd,int offset, int whence);

extern DfGFsUnitfd* gfs_RetrieveFile(const char *path, int flags);
extern DfGFsUnitfd* gfs_RetrievePath(const char *path, int flags);


#endif
