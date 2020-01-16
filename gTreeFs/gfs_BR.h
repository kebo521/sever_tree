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
#endif

extern int BalancedRecordInit(DfBalancedHead* pbh,u16 BlockStart,u16 BlockNum,u16 Maxbyte);
extern int BalancedRecordSave(DfBalancedHead* pbh,void* pBuff); //Len== Maxbyte
extern int BalancedRecordRead(DfBalancedHead* pbh,void* pBuff); //Len== Maxbyte

#endif
