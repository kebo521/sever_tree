/*
 * The gTree filesystem
 *
 * Copyright (c) 2019 DengGuozu
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
 
#include "communal.h"

#ifdef GTREE_FILE_COURCE

#ifdef GTREE_FILE_ERR_RECORD
u32	SaveDataErr=0,SaveHearErr=0,EraseTimes=0;
#endif

int BalancedRecordInit(DfBalancedHead* pbh,u16 BlockStart,u16 BlockNum,u16 Maxbyte)
{
	u32 h,d,t,v;
	u32 AddrStart,AddrSize;
	if(Maxbyte <4 || pbh==NULL) return -1;
	AddrStart = BlockStart*GFS_BLOCK_SIZE;
	AddrSize = BlockNum*GFS_BLOCK_SIZE;
	SPI_Flash_Read(AddrStart,&pbh->title,sizeof(pbh->title));
	//---------��������headLen��Сh----------------
	t=AddrSize-sizeof(DfCounterTitle)-sizeof(DfCounterEnd);
	v=(8*Maxbyte + 1)*sizeof(pbh->pHeadBit[0]);
	d = t%v;
	h = t/v + (d>0);
	//-----------------------------------------------
	v = h * sizeof(pbh->pHeadBit[0]);	//����v������¼pHeadBitʵ��byte��С
	pbh->pHeadBit=(u32 *)malloc(v);
	if(pbh->title.Start == AddrStart \
		&& pbh->title.Size == AddrSize \
	&& pbh->title.Maxbyte == Maxbyte \
		&& pbh->title.headLen == h)
	{
		SPI_Flash_Read(AddrStart+sizeof(pbh->title),pbh->pHeadBit,v);
		if(pbh->pHeadBit[0] == 0xffffffff)
			return 0;
		return Maxbyte;
	}
	else
	{
		char *pSaveData;
		u32 dataStart,LastIndex;
		DfCounterEnd tcounterEnd;
		pbh->title.Start = AddrStart;
		pbh->title.Size = AddrSize;
		pbh->title.Maxbyte=Maxbyte;
		pbh->title.headLen=h;
		memset_u32(pbh->pHeadBit,0xffffffff,v/sizeof(u32));
		dataStart=sizeof(pbh->title) +v;
		//-------ĩ��ȡ����
		SPI_Flash_Read(AddrStart+AddrSize-sizeof(tcounterEnd),&tcounterEnd,sizeof(tcounterEnd));
		if(tcounterEnd.head == GFS_BASIC_DATA)
		{
			LastIndex =(AddrSize -dataStart-sizeof(DfCounterEnd))/Maxbyte -1;
			if((dataStart+LastIndex*Maxbyte) != (AddrSize-tcounterEnd.effset))
			{
				TRACE("!-BalancedRecordInit[%X!=%X] warning\r\n",dataStart+LastIndex*Maxbyte,(AddrSize-tcounterEnd.effset));
			}
			pbh->pHeadBit[0] &= ~0x01;	//��¼����һ��
			pSaveData=(char *)malloc(dataStart+Maxbyte);
			memcpy_u32((u32*)pSaveData,(u32*)&pbh->title,sizeof(pbh->title)/sizeof(u32));
			memcpy_u32((u32*)(pSaveData+sizeof(pbh->title)),pbh->pHeadBit,v/sizeof(u32));
			SPI_Flash_Read(AddrStart+AddrSize-tcounterEnd.effset,pSaveData+dataStart,Maxbyte);
			dataStart += Maxbyte;
		}
		else 
		{
			pSaveData=(char *)malloc(dataStart);
			memcpy_u32((u32*)pSaveData,(u32*)&pbh->title,sizeof(pbh->title)/sizeof(u32));
			memcpy_u32((u32*)(pSaveData+sizeof(pbh->title)),pbh->pHeadBit,v/sizeof(u32));
			Maxbyte=0;//�����ݣ����
		}
		//---------------д��ͷ��----------------------------
		for(LastIndex=0; LastIndex<dataStart ; LastIndex+=GFS_BLOCK_SIZE)
			SPI_Flash_OneSector((AddrStart+LastIndex)/GFS_BLOCK_SIZE);
		SPI_Flash_Write(AddrStart,pSaveData,dataStart);
		free(pSaveData);
		for( ; LastIndex<AddrSize ; LastIndex+= GFS_BLOCK_SIZE)
			SPI_Flash_OneSector((AddrStart+LastIndex)/GFS_BLOCK_SIZE);
		#ifdef GTREE_FILE_ERR_RECORD
		EraseTimes++;
		#endif
		return Maxbyte;
	}
}

int BalancedRecordSave(DfBalancedHead* pbh,void* pBuff)	//Len== Maxbyte
{
	u16 i,max,index,IndexS;
	u32	bitflag=0x01,addrblock=0;
	u32 offset=0;
	if(pbh==NULL || pbh->pHeadBit==NULL) return -1;
	//------------����������ݶ�-------------------
	max=sizeof(pbh->pHeadBit[0])*8;
	for(index=0;index<pbh->title.headLen;index++)
	{
		if(pbh->pHeadBit[index])
		{
			bitflag=0x01;
			for(i=0;i<max;i++)
			{
				if(pbh->pHeadBit[index] & bitflag)
					break;
				bitflag <<= 1;
			}
			addrblock = index*max + i;	//bitflag-> add
			break;
		}
	}
	if(index < pbh->title.headLen)
	{
		u32	uBuff[GFS_BLOCK_SIZE/4];
		BYTE *fBuff,*pTag;
		pTag=pBuff;
		fBuff=(BYTE *)uBuff;
		/*
		//---------------------���ǰ��Flash�Ƿ���Ը�д------------------
		if(addrblock)
		{
			offset=sizeof(DfCounterTitle) + pbh->title.headLen*sizeof(pbh->pHeadBit[0]) + (addrblock-1)*pbh->title.Maxbyte;
			SPI_Flash_Read(pbh->title.Start+offset,fBuff,pbh->title.Maxbyte);
			for(i=0;i<pbh->title.Maxbyte;i++)
			{
				if(pTag[i] != (pTag[i]&fBuff[i]))
				{
					TRACE("!-BalancedCountSave[%d.%d][%d!=%d] BadFlash warning\r\n",offset,i,pTag[b],fBuff[b]);
					break;
				}
			}
			if(i == pbh->title.Maxbyte)
			{//--------���Ը�д----------
				SPI_Flash_Write(pbh->title.Start+offset,pBuff,pbh->title.Maxbyte);
				//-------------���д�������Ƿ�ɹ�-------------------------------------
				SPI_Flash_Read(pbh->title.Start+offset,fBuff,pbh->title.Maxbyte);
				for(i=0;i<pbh->title.Maxbyte;i++)
				{
					if(pTag[i] != fBuff[i])
					{
						TRACE("!-BalancedCountSave[%d.%d][%X!=%X] Repeated fail\r\n",
							offset,i,pTag[i],fBuff[i]);
						break;
					}
				}
				if(i == pbh->title.Maxbyte) return 0;	//OK
			}
		}
		*/
		//-----------------���FLASH�Ƿ�Ƭ--------------------------------
		IndexS=index;
		offset=sizeof(DfCounterTitle) + pbh->title.headLen*sizeof(pbh->pHeadBit[0]) + addrblock*pbh->title.Maxbyte;
		while(offset <= (pbh->title.Size - pbh->title.Maxbyte - sizeof(DfCounterEnd)))
		{
			SPI_Flash_Write(pbh->title.Start+offset,pTag,pbh->title.Maxbyte);
			//-------------���д�������Ƿ�ɹ�-------------------------------------
			SPI_Flash_Read(pbh->title.Start+offset,fBuff,pbh->title.Maxbyte);
			for(i=0;i<pbh->title.Maxbyte;i++)
			{
				if(pTag[i] != fBuff[i])
				{
					TRACE("!-BalancedCountSave[%d.%d][%X!=%X] BadFlash warning\r\n",offset,i,pTag[i],fBuff[i]);
					#ifdef GTREE_FILE_ERR_RECORD
					SaveDataErr++;
					#endif
					break;
				}
			}
			pbh->pHeadBit[IndexS] &= ~bitflag;
			
			if(i == pbh->title.Maxbyte)
			{//--------------д��ǩ----------------------
				u32 hearAddr;
				hearAddr=pbh->title.Start + sizeof(DfCounterTitle) + index*sizeof(pbh->pHeadBit[0]);
				SPI_Flash_Write(hearAddr,&pbh->pHeadBit[index],(IndexS-index+1)*sizeof(pbh->pHeadBit[0]));
				//--------------------У��д��head�Ƿ�ɹ�------------------------------
				SPI_Flash_Read(hearAddr,uBuff,(IndexS-index+1)*sizeof(pbh->pHeadBit[0]));
				if(pbh->pHeadBit[IndexS] == uBuff[IndexS-index])
				{//----д�ɹ�-
					return 0;
				}
				#ifdef GTREE_FILE_ERR_RECORD
				SaveHearErr++;
				#endif
				//------------------------------------------------------
				TRACE("!-BalancedCountSave Head Index[%d,%d][%X!=%X] BadFlash warning\r\n",index,IndexS,pbh->pHeadBit[IndexS],uBuff[IndexS-index]);
			}
			//--------------���ɹ�����д��һ��-------------------
			offset += pbh->title.Maxbyte;
			bitflag <<= 1;
			if(!bitflag) 
			{
				IndexS++;
				bitflag=0x01;
			}
		}
		TRACE("BalancedCountSave Save Full offset[%d]Size[%d]\r\n",offset,pbh->title.Size);
	}

	{//-------------��������������Ӧ����ǩ-----------------
		DfCounterEnd tcounterEnd;
		tcounterEnd.head = GFS_BASIC_DATA;
		tcounterEnd.effset = pbh->title.Size - (offset - pbh->title.Maxbyte);
		SPI_Flash_Write(pbh->title.Start+pbh->title.Size-sizeof(DfCounterEnd),&tcounterEnd,sizeof(tcounterEnd));
	}
	//else //if(index == tCounterHead.title.headLen)
	{//--------�Ѿ�д��һ�����ڣ��������д----------
		char *pSaveData;
		TRACE("BalancedCountSave cycle index[%d] \r\n",index);
		i = pbh->title.headLen;
		while(i--)
			pbh->pHeadBit[i]=0xffffffff;
		pbh->pHeadBit[0] &= ~0x01;
		addrblock=pbh->title.headLen * sizeof(pbh->pHeadBit[0]);

		pSaveData=(char*)malloc(sizeof(pbh->title)+addrblock+pbh->title.Maxbyte);
		
		memcpy_u32((u32*)pSaveData,(u32*)&pbh->title,sizeof(pbh->title)/sizeof(u32));
		memcpy_u32((u32*)(pSaveData+sizeof(pbh->title)),pbh->pHeadBit,addrblock/sizeof(u32));
		addrblock += sizeof(pbh->title);
		memcpy_u8((u8*)pSaveData+addrblock,(u8*)pBuff,pbh->title.Maxbyte);
		addrblock += pbh->title.Maxbyte;

		for(bitflag=0 ;bitflag<addrblock; bitflag+= GFS_BLOCK_SIZE)
			SPI_Flash_OneSector((pbh->title.Start+bitflag)/GFS_BLOCK_SIZE);
		SPI_Flash_Write(pbh->title.Start,pSaveData,addrblock);
		free(pSaveData);
		for(;bitflag<pbh->title.Size; bitflag+= GFS_BLOCK_SIZE)
			SPI_Flash_OneSector((pbh->title.Start+bitflag)/GFS_BLOCK_SIZE);
		#ifdef GTREE_FILE_ERR_RECORD
		EraseTimes++;
		#endif
	}
	return 0;
}

int BalancedRecordRead(DfBalancedHead* pbh,void* pBuff)	//Len== Maxbyte
{
	u16 index,b,max;
	u32	bitflag;
	if(pbh->pHeadBit==NULL) return -1;
	//------------����������ݶ�-------------------
	max=sizeof(pbh->pHeadBit[0])*8;
	index=pbh->title.headLen;
	while(index--)
	{
		bitflag=0x80000000;
		b = max;
		while(b--)
		{
			if(!(pbh->pHeadBit[index] & bitflag))
			{//---------����������---Ҳ����������-----------------------
				u32 dataStart,addrblock;
				addrblock = (index*max + b) * pbh->title.Maxbyte;	//bitflag-> add
				dataStart=sizeof(pbh->title)+pbh->title.headLen*sizeof(pbh->pHeadBit[0]);
				SPI_Flash_Read(pbh->title.Start+dataStart+addrblock,pBuff,pbh->title.Maxbyte);
				return pbh->title.Maxbyte;
			}
			bitflag >>= 1;
		}
	}
	return -5;
}

#endif

#if(0)

#define GFS_COUNTER_ADDR			(4*256*GFS_BLOCK_SIZE)
#define GFS_COUNTER_SIZE			(4*GFS_BLOCK_SIZE)

//---������У��-Return 0,��ȫ,��0Ϊ��Ч�����ݳ���----------------
typedef int (*CHECK_DATA_FULL)(char *,int); 

typedef struct
{
	u32	Start;
	u32	Size;
	u16	Maxbyte;	//ÿ�����ݿ��С
	u16	headLen;	//λͷ������ݳ���
}DfCounterTitle;

typedef struct
{
	DfCounterTitle title;
	u32* 	pHeadBit;
}DfCounterHead;

DfCounterHead tCounterHead={0};

int BalancedRecordInit(u16 BlockStart,u16 BlockNum,u16 Maxbyte,CHECK_DATA_FULL *funCheck)
{
	u16 h,d,t,v;
	u32 AddrStart,AddrSize;
	if(Maxbyte <4) return -1;
	AddrStart = BlockStart*GFS_BLOCK_SIZE;
	AddrSize = BlockNum*GFS_BLOCK_SIZE;
	SPI_Flash_Read(AddrStart,&tCounterHead.title,sizeof(tCounterHead.title));
	//---------��������headLen��Сh----------------
	t=GFS_COUNTER_SIZE-sizeof(DfCounterTitle);
	v=(8*Maxbyte + 1)*sizeof(tCounterHead.pHeadBit[0]);
	d = t%v;
	h = t/v + (d>0);
	//-----------------------------------------------
	v = h * sizeof(tCounterHead.pHeadBit[0]);	//����v������¼pHeadBitʵ��byte��С
	tCounterHead.pHeadBit=(u32 *)malloc(v);
	if(tCounterHead.title.Start == AddrStart \
		&& tCounterHead.title.Size == AddrSize \
	&& tCounterHead.title.Maxbyte == Maxbyte \
		&& tCounterHead.title.headLen == h)
	{
		SPI_Flash_Read(AddrStart+sizeof(tCounterHead.title),tCounterHead.pHeadBit,v);
		if(tCounterHead.pHeadBit[0] == 0xffffffff)
			return 0;
		return Maxbyte;
	}
	else
	{
		char *pSaveData,*pcheckData=NULL;
		u32 dataStart,LastIndex;
		tCounterHead.title.Start = AddrStart;
		tCounterHead.title.Size = AddrSize;
		tCounterHead.title.Maxbyte=Maxbyte;
		tCounterHead.title.headLen=h;
		memset(tCounterHead.pHeadBit,0xff,v);
		dataStart=sizeof(tCounterHead.title) +v;	
		if(funCheck)
		{
			LastIndex =(AddrSize -dataStart)/Maxbyte -1;
			pcheckData =(char *)malloc(Maxbyte);
			SPI_Flash_Read(AddrStart+dataStart+LastIndex*Maxbyte,pcheckData,Maxbyte);
			if(funCheck(pcheckData,Maxbyte) > 0)
			{//������Ч
				tCounterHead.pHeadBit[0] &= (~0x01);
			}
			else
			{
				free(pcheckData);
				pcheckData=NULL;
			}
		}
		if(pcheckData)
		{
			pSaveData=(char *)malloc(dataStart+Maxbyte);
			memcpy(pSaveData,&tCounterHead.title,sizeof(tCounterHead.title));
			memcpy(pSaveData+sizeof(tCounterHead.title),tCounterHead.pHeadBit,v);
			memcpy(pSaveData+dataStart,pcheckData,Maxbyte);
			dataStart += Maxbyte;
		}
		else 
		{
			pSaveData=(char *)malloc(dataStart);
			memcpy(pSaveData,&tCounterHead.title,sizeof(tCounterHead.title));
			memcpy(pSaveData+sizeof(tCounterHead.title),tCounterHead.pHeadBit,v);
		}
		//---------------д��ͷ��----------------------------
		for(LastIndex=0; LastIndex<dataStart ; LastIndex+=GFS_BLOCK_SIZE)
			SPI_Flash_ERASE(AddrStart+LastIndex);
		SPI_Flash_Write(AddrStart,pSaveData,dataStart);
		free(pSaveData);
		for( ; LastIndex<AddrSize ; LastIndex+= GFS_BLOCK_SIZE)
			SPI_Flash_ERASE(AddrStart+LastIndex);
		if(pcheckData)
		{//-----����ȡ������---------
			free(pcheckData);
			return Maxbyte;
		}
		return 0;	//������
	}
}

int BalancedRecordSave(void* pBuff)	//Len== Maxbyte
{
	u16 index,b=0,max,addrblock;
	u32	bitflag=0x01;
	if(tCounterHead.pHeadBit==NULL) return -1;
	//------------����������ݶ�-------------------
	max=sizeof(tCounterHead.pHeadBit[0])*8;
	for(index=0;index<tCounterHead.title.headLen;index++)
	{
		if(tCounterHead.pHeadBit[index])
		{
			bitflag=0x01;
			for(b=0;b<max;b++)
			{
				if(tCounterHead.pHeadBit[index] & bitflag)
					break;
				bitflag <<= 1;
			}
			addrblock = index*max + b;	//bitflag-> add
			break;
		}
	}
	if(index < tCounterHead.title.headLen)
	{
		u32 offset;
		BYTE fBuff[GFS_BLOCK_SIZE],*pTag;
		u16 IndexS;
		IndexS=index;
		//-----------------���FLASH�Ƿ�Ƭ--------------------------------
		while(offset < (tCounterHead.title.Start+tCounterHead.title.Size))
		{
			tCounterHead.pHeadBit[IndexS] &= ~bitflag;
			offset=sizeof(DfCounterTitle) + tCounterHead.title.headLen*sizeof(tCounterHead.pHeadBit[0]) + addrblock*tCounterHead.title.Maxbyte;
			SPI_Flash_Write(tCounterHead.title.Start+offset,pBuff,tCounterHead.title.Maxbyte);
			//-------------���д�������Ƿ�ɹ�-------------------------------------
			SPI_Flash_Read(tCounterHead.title.Start+offset,fBuff,tCounterHead.title.Maxbyte);
			pTag=pBuff;
			for(b=0;b<tCounterHead.title.Maxbyte;b++)
			{
				if(pTag[b] != fBuff[b])
				{
					TRACE("!-BalancedCountSave[%d.%d][%d!=%d] BadFlash warning\r\n",offset,b,pTag[b],fBuff[b]);
					break;
				}
			}
			if(b == tCounterHead.title.Maxbyte) break;	//OK
			//--------------���ɹ�����д��һ��-------------------
			addrblock += tCounterHead.title.Maxbyte;
			bitflag <<= 1;
			if(!bitflag) 
			{
				IndexS++;
				bitflag=0x01;
			}
		}
		if(offset < (tCounterHead.title.Start+tCounterHead.title.Size))
		{
			if(IndexS >=index)
			{
				offset=sizeof(DfCounterTitle) + index*sizeof(tCounterHead.pHeadBit[0]);
				IndexS = (IndexS-index+1)*sizeof(tCounterHead.pHeadBit[0]);	//IndexS -> headBit Len
				SPI_Flash_Write(tCounterHead.title.Start+offset,&tCounterHead.pHeadBit[index],IndexS);
				//--------------------У��д��head�Ƿ�ɹ�------------------------------
				SPI_Flash_Read(tCounterHead.title.Start+offset,fBuff,tCounterHead.title.Maxbyte);
				pTag=&tCounterHead.pHeadBit[index];
				for(b=0;b<IndexS;b++)
				{
					if(pTag[b] != fBuff[b])
						break;
				}
				if(b<IndexS)
				{
					return -1;
				}
				//---------------------------------------------------------------------
			}
			return 0;
		}
		return -1;	//ȫ����Ƭ
	}
	else //if(index == tCounterHead.title.headLen)
	{//--------�Ѿ�д��һ�����ڣ��������д----------
		char *pSaveData;
		u32 dataStart,LastIndex;
		
		b = tCounterHead.title.headLen;
		while(b--)
			tCounterHead.pHeadBit[b]=0xffffffff;
		tCounterHead.pHeadBit[0] &= ~bitflag;
		dataStart=sizeof(tCounterHead.title);
		LastIndex=tCounterHead.title.headLen*sizeof(tCounterHead.pHeadBit[0]);

		pSaveData=(char*)malloc(dataStart+LastIndex+tCounterHead.title.Maxbyte);
		
		memcpy(pSaveData,&tCounterHead.title,dataStart);
		memcpy(pSaveData+dataStart,tCounterHead.pHeadBit,LastIndex);
		dataStart += LastIndex;
		memcpy(pSaveData+dataStart,pBuff,tCounterHead.title.Maxbyte);
		dataStart += tCounterHead.title.Maxbyte;

		for(LastIndex=0 ;LastIndex<dataStart; LastIndex+= GFS_BLOCK_SIZE)
			SPI_Flash_ERASE(tCounterHead.title.Start+LastIndex);
		SPI_Flash_Write(tCounterHead.title.Start,pSaveData,dataStart);
		free(pSaveData);
		for(;LastIndex<tCounterHead.title.Size; LastIndex+= GFS_BLOCK_SIZE)
			SPI_Flash_ERASE(tCounterHead.title.Start+LastIndex);
	}
	return 0;
}

int BalancedRecordRead(void* pBuff)	//Len== Maxbyte
{
	u16 index,b,max;
	u32	bitflag;
	if(tCounterHead.pHeadBit==NULL) return -1;
	//------------����������ݶ�-------------------
	max=sizeof(tCounterHead.pHeadBit[0])*8;
	index=tCounterHead.title.headLen;
	while(index--)
	{
		bitflag=0x80000000;
		b = max;
		while(b--)
		{
			if(!(tCounterHead.pHeadBit[index] & bitflag))
			{//---------����������---Ҳ����������-----------------------
				u32 dataStart,addrblock;
				addrblock = (index*max + b) * tCounterHead.title.Maxbyte;	//bitflag-> add
				dataStart=sizeof(tCounterHead.title)+tCounterHead.title.headLen*sizeof(tCounterHead.pHeadBit[0]);
				SPI_Flash_Read(tCounterHead.title.Start+dataStart+addrblock,pBuff,tCounterHead.title.Maxbyte);
				return tCounterHead.title.Maxbyte;
			}
			bitflag >>= 1;
		}
	}
	return -5;
}


#endif

