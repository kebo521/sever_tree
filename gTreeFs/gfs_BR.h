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
#ifndef GFS_BR_H_
#define GFS_BR_H_

typedef struct
{
	u32	Start;
	u32	Size;
	u16	Maxbyte;	//每个数据块大小
	u16	headLen;	//位头标记数据长度
}DfCounterTitle;

typedef struct
{
	u16	effset;	//应急数据末端位置
	u16	head;	//块头:[FFFF] 已经察除，[48||84] 坏块，[EEEE] 引导数据，[DDDD] 使用数据
}DfCounterEnd;

typedef struct
{
	DfCounterTitle title;
	u32* 	pHeadBit;
}DfBalancedHead;

#ifdef GTREE_FILE_ERR_RECORD
extern u32	SaveDataErr,SaveHearErr,EraseTimes;
extern u32 gfs_erase_times;
#endif



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

#endif
