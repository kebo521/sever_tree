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
	u16	Maxbyte;	//ÿ�����ݿ��С
	u16	headLen;	//λͷ������ݳ���
}DfCounterTitle;

typedef struct
{
	u16	effset;	//Ӧ������ĩ��λ��
	u16	head;	//��ͷ:[FFFF] �Ѿ������[48||84] ���飬[EEEE] �������ݣ�[DDDD] ʹ������
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
