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

#define GFS_BASIC_NULL		0xFFFF	//无数
#define GFS_BASIC_GUID		0xEEEE	//引导
#define GFS_BASIC_DATA		0xDDDD	//数据
#define GFS_BASIC_BAD		0x4884	//坏块


#define GFS_INDEX_NULL 		GFS_BASIC_NULL
#define GFS_BLOCK_SIZE 		512		//4096
#define GFS_PAGE_SIZE 		4		//256


#define GFS_BIT_FLAG 		8	//32
#define GFS_FILE_MAX 		92	//124


// File types 文件夹标记	[f]文件夹,[e]可执行文件，[s]字符串文件，[b]二进制文件
enum fType {
    GFS_TYPE_FLD	= 'f',	//Folder
    GFS_TYPE_BIN	= 'b',
	GFS_TYPE_STR	= 's',
	GFS_ENC_STR		= 'd',	//加密钥文件
	GFS_TYPE_EXE	= 'e',	//可执行文件
};

typedef struct
{
	u16	head;	//块头:[FFFF] 已经察除，[48||84] 坏块，[EEEE] 引导数据，[DDDD] 使用数据
	u16	index;	//平衡记录器 -> 文件
	u8	flagG;			// 'G'	//正常标记
	u8	flagClear;		//已经察除,[FF]未察除，55 已经察除
	u16	BrIndex;		//记数器文件母入口
}DfGTreeFsBlockHead;


//----------定义文件系统的头--HEAD--256-----
typedef struct
{
	DfGTreeFsBlockHead t;
	u16		BlockIndex[GFS_FILE_MAX];	//一级入口，直接对应文件或文件夹
	//-------64byte-----------
	u32		NullBit[GFS_BIT_FLAG];	//空块 block
	u32		EraseBit[GFS_BIT_FLAG];	//需要察除的块
}DfGTreeFsGlobal;


typedef struct
{
	u16 gfsCoutStart;	//文件引导区地址，平衡记数器
	u16 gfsCoutSize;	//文件引导区空间大小
	u16 gfsCoutIndex;	//文件引导区平衡记数器
	u16 gfsBlockStart;	//文件内容起始地址
	u16 gfsBlockSize;	//文件内容空间大小
	u16 gfsNewIndex;	//文件空间平衡记数器
	DfGTreeFsGlobal g;	//文件引导区内容，存于平衡记数器中
}DfGTreeFsBasi;

//=================定义文件体========================
#define GTREE_FS_NAME_MAX	32
#define GTREE_FS_UNIT_SIZE	256	//2048

typedef struct
{
	char	filepath[GTREE_FS_NAME_MAX-4];	//文件夹名或文件名
	char	check[3];	//加密文件校验值
	char	fType;		//fType文件夹标记	[f]文件夹,[e]可执行文件，[s]字符串文件，[b]二进制文件
}DfGTreeFsFileName;

//----------定义文件系统数据--DATA-----------
typedef struct	//	GTREE_FS_UNIT_SIZE
{
	DfGTreeFsFileName name;
	u32		InvLen;		//返数长度，文件递增中减少FLASH 察除。取长(~InvLen)
	u16 	fdIndex[(GTREE_FS_UNIT_SIZE-GTREE_FS_NAME_MAX-4)/2]; //文件内容块索引
}DfgFsUnitHead;

typedef struct	//	GFS_BLOCK_SIZE
{
	DfgFsUnitHead h;  //	GTREE_FS_UNIT_SIZE
	u8	fData[GFS_BLOCK_SIZE-GTREE_FS_UNIT_SIZE];	//文件内容，最大2048
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
	u32		fLen;	//文件当前返码长度，取长(~fInvLen)
	u32		fOffset;	//文件当前偏移
	u16 	IndexGuide;	//索引块入口
	u16 	IndexEntry;	//文件入口
	
	u16 	fOpenFlag;
	u16 	IndexLast;	//上一次申请的空间
	
	u16 	IndexB;		//块索引
	u16 	bOffet;		//块偏移	

	u8		flagUpT;	//更新标题
	u8		flagUpH;	//更新头参数,0x80 换块，0x40 有新增块，0x20 有写入动作，0x10 有更新前引导区
	u8		flagUpE;	//更新察除标记
	u8		flagNew;	//重新写入内容
	//----------Block 存放内容集-[GFS_BLOCK_SIZE]-----------
	DfgFsUnitBlock u;
}gFILE,*LPgFILE;	//文件句柄

#define 	GFS_PATH_TAG				('/')
//========文件路径检查===返回 1,文件路径，-1非文件夹，字符不匹配，0 匹配字符------
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
