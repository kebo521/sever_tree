//#include "includes.h"
#include "ks_type.h"

#include "gMem.h"

#include "hardInfo.h"
#include "app_sdk.h"
#include "voucher.h"
#include "Wallet.h"


static DfHeadInfo *pWallet=NULL;

#ifdef WALLET_TEST_DATA
DfHeadInfo *pRecvWallet=NULL;
DfHeadInfo *pPayWallet=NULL;

DfHeadInfo* GetCurrWallet(void)
{
	return pWallet;
}
void SetCurrWallet(DfHeadInfo* p)
{
	pWallet=p;
}

void SetRecvWallet(void)
{
	pWallet=pRecvWallet;
}

void SetPayWallet(void)
{
	pWallet=pPayWallet;
}


int Test_CreateDc(DfDC_bt* pDc,u8* pOwnerID,u32 amont)
{
	pDc->tag=0x72;
	pDc->Len[0]=0x82;
	pDc->Len[1]=sizeof(DC_bt)/0x100;
	pDc->Len[2]=sizeof(DC_bt)&0xff;

	pDc->dc.veTag= 0x41;
	pDc->dc.veLen= 2;
	pDc->dc.Version[0]= 0x00;
	pDc->dc.Version[0]= 0x01;
	pDc->dc.seTag= 0x42;
	pDc->dc.seLen= 32;
	strcpy(pDc->dc.SerialNumber,"DCEP00000000001");
	
	pDc->dc.trTag= 0x43;
	pDc->dc.trLen= 32;
	strcpy(pDc->dc.TranId,"DC2020082400000000001");

	pDc->dc.deTag= 0x44;
	pDc->dc.deLen= 6;
	Conv_NumToHex(amont,pDc->dc.Denomination,sizeof(pDc->dc.Denomination));

	pDc->dc.inTag= 0x45;
	pDc->dc.inLen= 3;
	strcpy((char*)pDc->dc.InstructingPartyID,"BC");

	pDc->dc.isTag= 0x46;
	pDc->dc.isLen= 6;
	memcpy(pDc->dc.IssuerTime,"\x01\x74\x20\xC6\x74\x98",pDc->dc.isLen);	//2020-08-24 22:01:51

	pDc->dc.psTag= 0x48;
	pDc->dc.psLen= 64;
	memset(pDc->dc.PbcSign,0xff,sizeof(pDc->dc.PbcSign));

	pDc->dc.owTag= 0x49;
	pDc->dc.owLen= 64;
	memcpy(pDc->dc.OwnerID,pOwnerID,sizeof(pDc->dc.OwnerID));

	pDc->dc.ipTag= 0x4b;
	pDc->dc.ipLen= sizeof(pDc->dc.InstructingPartySign);
	memset(pDc->dc.InstructingPartySign,0xff,sizeof(pDc->dc.InstructingPartySign));
	return 0;
}


void Test_CreateWallet(u8* pWalletID,char *pWalletName,char* pNameCert,u32 amont)
{
	uWalletFD* pFd;
	u8 PWCheck[4],EncKey[16];
	uBalVoucher* pBal;
	//-----------------付款钱包----------------------------
	TRACE("CreateWallet[%s]\r\n",pWalletName);
	gkeyAndCheck("12345678",PWCheck, EncKey);
	pFd=gCreateWallet(pWalletID,pWalletName,PWCheck);
	memcpy(pFd->WalletID,pWalletID,sizeof(pFd->WalletID));
	WalletReadData(pFd);

	if((u8)pFd->pWallet->Info.WalletName[0] == 0xff)
	{
		if((pWalletID[7]&0x0f) == 0x01)
			memcpy(pFd->pWallet->Info.FinInstCode,"FinInstCode001",sizeof(pFd->pWallet->Info.FinInstCode));
		else
			memcpy(pFd->pWallet->Info.FinInstCode,"FinInstCode002",sizeof(pFd->pWallet->Info.FinInstCode));
		memcpy(pFd->pWallet->Info.WalletID,pWalletID,sizeof(pFd->pWallet->Info.WalletID));
		strcpy(pFd->pWallet->Info.WalletName,pWalletName);
		APP_FileSaveBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead));
	}
	if(pFd->pKey == NULL)
	{
		u8 certbuff[16];
		pFd->pKey  = (SM2_PRIVKEY *)gMalloc(sizeof(SM2_PRIVKEY));
		sm2_generate_keypair(pFd->pKey->xQ,pFd->pKey->d);
		SavePubKeyCert(pFd->WalletID,PKEY_PERS,(u8*)pFd->pKey,sizeof(SM2_PRIVKEY));
		
		certbuff[0]=0x30;
		certbuff[1]=0x08;
		strcpy((char*)certbuff+2,pNameCert);
		SavePubKeyCert(pFd->WalletID,CERT_PERS,certbuff,10);
		strcpy((char*)certbuff+2,"机构证书");
		SavePubKeyCert(pFd->WalletID,CERT_INS,certbuff,10);
	}
	
	pBal=ReadBalVoucher(GetWalletName(0,pFd->WalletID));
	if(pBal == NULL)
	{
		pBal=(uBalVoucher*)gMalloc(sizeof(uBalVoucher)+4);
		Test_CreateDc(&pBal->tDc,pFd->pKey->xQ,amont);
		pBal->SubChainNun = 0;
		SaveBalVoucher(GetWalletName(0,pFd->WalletID),pBal);
		
		pFd->pRealizeInfo->uBal=amont;
		pFd->pRealizeInfo->uAvailableBal=amont;
		pFd->pRealizeInfo->VoucherBal[0]=amont;
		APP_FileSaveCont(GetWalletName(WAL_REAL,pFd->WalletID),(u32*)pFd->pRealizeInfo,sizeof(uSystemRealizeInfo));
	}
	gFree(pBal);
}


int Test_CreateNewDc(uWalletFD* pFd,u32 Amont)
{
	uBalVoucher* pBal;
	int newIndex;
	if(pFd == NULL)
		return -1;
	if(pFd->pRealizeInfo == NULL)
		return -2;
	newIndex=FindVoucherNull(pFd->pRealizeInfo);
	if(newIndex < 0)
		return -3;
	pBal=(uBalVoucher*)gMalloc(sizeof(uBalVoucher)+4);
	TRACE("Generate new Dc[%d]Amont[%d]\r\n",newIndex,Amont);
	Test_CreateDc(&pBal->tDc,pFd->pKey->xQ,Amont);
	pBal->SubChainNun = 0;
	SaveBalVoucher(GetWalletName(newIndex,pFd->WalletID),pBal);
	gFree(pBal);
	TRACE("SaveBalVoucher[%x] Dc in\r\n",pFd->pRealizeInfo);
	pFd->pRealizeInfo->uBal += Amont;
	pFd->pRealizeInfo->uAvailableBal += Amont;
	pFd->pRealizeInfo->VoucherBal[newIndex]=Amont;
	APP_FileSaveCont(GetWalletName(WAL_REAL,pFd->WalletID),(u32*)pFd->pRealizeInfo,sizeof(uSystemRealizeInfo));
	return 0;
}

#endif

void gkeyAndCheck(char *pPassWord,u8 *pOutChek,u8* pOutKey)
{
	u8 buff[32];
	u8 i,max;
	memset(buff,0xff,sizeof(buff)-9);
	memcpy(buff+sizeof(buff)-9,"g1u2o3z4u",9);
	strcpy((char*)buff,pPassWord);
	api_md_hash(MODE_SM3,buff,32,buff);
	if(pOutChek) memcpy(pOutChek,buff,4);
	max = 16;
	for(i=0;i<max;i++)
	{
		buff[i] = (~buff[i]) ^ buff[i+16];
	}
	if(pOutKey) memcpy(pOutKey,buff,16);	
}



void WalletData_Init(void)
{
	gFILE*	fd;
	int 	ret=0;
	fd = API_fopen("wallet.head",F_RDONLY);
	if(fd == NULL)
	{
		TRACE("A Open[wallet.head]fail, \r\n");
		return;
	}
	pWallet = (DfHeadInfo *)gMalloc(sizeof(DfHeadInfo));	
	ret=API_fread(fd,&pWallet->HeadWallet,sizeof(DfFileHeadInfo));
	API_fclose(fd);
	if(sizeof(DfFileHeadInfo) != ret)
	{
		TRACE("APP FileReadBuff Read[wallet.head] Err nReadSize[%d,%d]\r\n",ret,sizeof(DfFileHeadInfo));
		gFree(pWallet); pWallet=NULL;
	}	
}




uWalletFD* gCreateWallet(u8* pWalletID,char* pWalletName,u8* pPWCheck)
{
	gFILE*	fd;
	int 	ret=0;
	u16 i,total;
	DfFileHeadInfo 		*pHeadWallet;
	if(pWallet == NULL)
		pWallet = (DfHeadInfo *)gMalloc(sizeof(DfHeadInfo)); 
	else if(memcmp(pWallet->HeadWallet.WalletID,pWalletID,8)==0)
	{
		return &pWallet->Wallet;
	}
	memset(&pWallet->Wallet,0x00,sizeof(uWalletFD));
	fd = API_fopen("wallet.head",F_RDWR);
	if(fd)
	{
		ret= gfs_GetSize(fd);
		pHeadWallet= (DfFileHeadInfo *)gMalloc(ret);
		ret=API_fread(fd,pHeadWallet,ret);
		total = ret/sizeof(DfFileHeadInfo);
		for(i=0;i<total;i++)
		{
			if(memcmp(pHeadWallet[i].WalletID,pWalletID,8)==0)
			{
				break;
			}
		}
		if(i<total)
		{
			memcpy(&pWallet->HeadWallet,&pHeadWallet[i],sizeof(DfFileHeadInfo));
		}
		else
		{
			memset(&pWallet->HeadWallet,0xff,sizeof(DfFileHeadInfo));
			memcpy(pWallet->HeadWallet.WalletID,pWalletID,sizeof(pWallet->HeadWallet.WalletID));	
			if(pWalletName)
				memcpy(pWallet->HeadWallet.WalletName,pWalletName,sizeof(pWallet->HeadWallet.WalletName));
			if(pPWCheck)
				memcpy(pWallet->HeadWallet.PWCheck,pPWCheck,sizeof(pWallet->HeadWallet.PWCheck));
			API_fseek(fd,total * sizeof(DfHeadInfo),GFS_SEEK_SET);
			API_fwrite(fd, (u32*)&pWallet->HeadWallet,sizeof(DfFileHeadInfo));
		}
		gFree(pHeadWallet);
		API_fclose(fd);
	}
	else 
	{
		memset(&pWallet->HeadWallet,0xff,sizeof(DfFileHeadInfo));
		memcpy(pWallet->HeadWallet.WalletID,pWalletID,sizeof(pWallet->HeadWallet.WalletID));
		if(pWalletName)
			memcpy(pWallet->HeadWallet.WalletName,pWalletName,sizeof(pWallet->HeadWallet.WalletName));
		if(pPWCheck)
			memcpy(pWallet->HeadWallet.PWCheck,pPWCheck,sizeof(pWallet->HeadWallet.PWCheck));
		APP_FileSaveBuff("wallet.head",(u32*)&pWallet->HeadWallet,sizeof(pWallet->HeadWallet));
	}						
	return &pWallet->Wallet;
}


DfFileHeadInfo* GetWalletHead(int* pNum)
{
	gFILE*	fd;
	DfFileHeadInfo *pAll;
	int ret;
	fd = API_fopen("wallet.head",F_RDONLY);
	if(fd)
	{
		ret=gfs_GetSize(fd);
		pAll=(DfFileHeadInfo *)gMalloc(ret);
		ret=API_fread(fd,pAll,ret);
		API_fclose(fd);
		if(pNum) *pNum = ret/sizeof(DfFileHeadInfo);
		return pAll;
	}
	return NULL;
}

uWalletFD* gGetWalletPfd(void)
{
	if(pWallet)
		return &pWallet->Wallet;
	return NULL;
}

DfFileHeadInfo* gGetWalletHeadvoid(void)
{
	if(pWallet)
		return &pWallet->HeadWallet;
	return NULL;
}





int ControlInfo_Save_Data(uSystemControlInfo*	pControl,u8 *pS,u8 *pEnd)
{
	u16 tag,Len;
	while(pS < pEnd)
	{
		parse_pboc_tag(tag,pS);
		parse_asn1_len(Len,pS);
		/*
		Len = *pS;
		if(Len&0x80)
		{
			u16 num = Len&0x07;
			Len=0;
			while(num--)
			{
				Len = Len*256 + *pS++;
			}
		}
		*/
		switch(tag)
		{
			case 0x5F1B: //连续离线支付金额[4] TAG=0x5F1B
				pControl->uOffTranLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F1E: //单笔支付金额限制[4] TAG=0x5F1E
				pControl->uSinglePayLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F1F: //日累计支付金额限额[4] TAG=0x5F1F
				pControl->uDayPayLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F20: //月累计支付金额限额[4] TAG=0x5F20
				pControl->uMonthPayLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F21: //年累计支付金额限额[6] TAG=0x5F21
				pControl->uYearPayLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F25: //免密限额[3] TAG=0x5F25
				pControl->uNoPwdLimAmt=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F24: //连续离线连续交易时间限制数[2] TAG=0x5F24
				pControl->uOffTranLimTime=Conv_Hex4ToNum(pS,Len);
				break;
			case 0x5F1A: //连续离线交易参数限制 TAG=0x5F1A
				pControl->OffTranLimTimes=*pS;
				break;
			case 0x5F1C: //转手次数限制 TAG=0x5F1C 
				pControl->HandLimTimes=*pS;
				break;
			case 0x5F1D: //单个币串离线交易时间限制数 TAG=0x5F1D
				pControl->PayLimTime=*pS;
				break;
			case 0x5F22: //钱包生效日期 TAG=0x5F22
				memcpy(pControl->WalletEffectiveData,pS,sizeof(pControl->WalletEffectiveData));
				break;
			case 0x5F23: //上笔交易日期 TAG=0x5F23
				memcpy(pControl->LastTranTime,pS,sizeof(pControl->LastTranTime));
				break;
		}
		pS += Len;
	}
	return 0;
}

//DfFlashHeadInfo tFlashHead;
//void *pfd_Head=NULL;




void WalletReadData(uWalletFD* pFd)
{
	//----------钱包数据----------------
	if(pFd->pWallet == NULL)
	{
		pFd->pWallet=(uWalletHead*)gMalloc(sizeof(uWalletHead));
		if(APP_FileReadBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead)))
		{
			memset(pFd->pWallet,0xff,sizeof(uWalletHead));
			memcpy(pFd->pWallet->Info.WalletID,pFd->WalletID,sizeof(pFd->WalletID));
			//APP_FileSaveBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead));
		}
	}
	//--------------钱包管理数据记录--------------------------
	if(pFd->pRealizeInfo == NULL)
	{
		pFd->pRealizeInfo = (uSystemRealizeInfo*)gMalloc(sizeof(uSystemRealizeInfo));
		memset(pFd->pRealizeInfo,0x00,sizeof(uSystemRealizeInfo));
		memset(pFd->pRealizeInfo->VoucherBal,0xFF,sizeof(pFd->pRealizeInfo->VoucherBal));
		APP_FileReadCont(GetWalletName(WAL_REAL,pFd->WalletID),(u32*)pFd->pRealizeInfo,sizeof(uSystemRealizeInfo));
	}
	//----------调出私钥------------------
	if(pFd->pKey == NULL)
	{
		pFd->pKey = gMalloc(sizeof(SM2_PRIVKEY));
		if(APP_FileReadBuff(GetWalletName(WAL_CERK,pFd->WalletID),(u32*)pFd->pKey,sizeof(SM2_PRIVKEY))==0)
		{//-------------解密私钥-------
			sm4_ecb_encrypt_decrypt(ALG_DECRYPT,pFd->Enckey,(UINT8*)pFd->pKey,(UINT8*)pFd->pKey,sizeof(SM2_PRIVKEY));
		}
		else 
		{
			FreeNull(pFd->pKey);
		}
	}
}

#if(0)
int WalletDataParse(u8 *pIn,int inlen,u8 *pOut)
{
	u8* pDataRet=pOut;
	switch(pIn[0])
	{
		case 0x01:	//打开钱包
		case 0x41:	//创建钱包
			{
				u8 buff[16],PWCheck[4];
				u8 new,ufd;
				uWalletFD* pFd;
				uWalletPort *pInfo= (uWalletPort *)(pIn+1);
				gkeyAndCheck(pInfo->PassWord,PWCheck,buff);
				new = 0;
				pFd = gFindWalletFd(pInfo->WalletID,&ufd);
				if(pFd)
				{
					if(memcpy(PWCheck,pFd->pWallet->PWCheck,sizeof(PWCheck)))
					{
						gClearWalletFd(ufd);	//清空数据
						*pDataRet++= 0x80|pIn[0];
						*pDataRet++= 0xff;
						strcpy((char*)pDataRet+1,"密码错");
						*pDataRet++= 6;
						pDataRet += 6;
						break;
					}
				}
				else if(pIn[0]&0x40)
				{
					pFd = gGetNullWalletFd(&ufd);
				}
				
				if(pFd)
				{
					memcpy(pFd->Enckey,buff,sizeof(pFd->Enckey));						
					memcpy(pFd->WalletID,pInfo->WalletID,sizeof(pInfo->WalletID));
					WalletReadData(pFd->WalletID,PWCheck);
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= ufd;
					*pDataRet++= 0x00;
					*pDataRet++= 0x00;
					*pDataRet++= 0x00;
					*pDataRet++= 0x00;	
				}
				else
				{
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 0xff;
					if(pIn[0]&0x40)
						strcpy((char*)pDataRet+1,"创建失败");
					else
						strcpy((char*)pDataRet+1,"无此钱包");
					*pDataRet++ = 8;
					pDataRet += 8;
				}
			}
			break;
		case 0x02:	//关钱包
		case 0x42:	//删钱包
		{
			uWalletFD* pFd;
			pFd=gGetWalletPfd(pIn[1]);
			if(pFd)
			{
				if(pIn[0]&0x40)	//删除钱包
				{
					char ucheck[4];
					//------------检查密码校验值----------------
					gkeyAndCheck(pIn+2,ucheck,NULL);	//检查密码
					if(memcmp(ucheck,pFd->pWallet->PWCheck,4)==0)
					{
						//-------检查是否还有币---------------
						//if(pWallet->SR)
					}
					//----------清除币串信息------------------
					{
						u16 i,max;
						max = sizeof(pFd->pWallet->dc)/sizeof(pFd->pWallet->dc[0]);
						for(i=0;i<max;i++)
						{
							if(pFd->pWallet->dc[i].InvFlag == ~0)	//ff 为NULL
								continue;
							API_fremove(GetWalletName(i,pFd->WalletID));
							pFd->pWallet->dc[i].InvFlag = ~0;
							pFd->pWallet->dc[i].InvVoucherBal = ~0;
						}
					}
					//-----------删除钱包数据----------------------------
					API_fremove(GetWalletName(WAL_HEAD,pFd->WalletID));
					API_fremove(GetWalletName(WAL_REAL,pFd->WalletID));
					API_fremove(GetWalletName(WAL_CERK,pFd->WalletID));
				}
				//----------清除钱包数据-----------------
				FreeNull(pFd->pKey);
				FreeNull(pFd->pNewKey);
				FreeNull(pFd->pRealizeInfo);
				FreeNull(pFd->pWallet);
				memset(pFd->Enckey,0xFF,sizeof(pFd->Enckey));
				
				*pDataRet++= 0x80|pIn[0];
				*pDataRet++= 0;
				*pDataRet++= 0;
			}
		}
			break;
		case 0x03:	//下载参数
			{
				//uEflashSystem *pSystem;
				uWalletFD* pFd;
				u8 uRet;
				//gFILE* pfd;
				pFd=gGetWalletPfd(pIn[1]);
				if(pFd!=NULL && pFd->pWallet != NULL)
				{
					pIn += 2;
					ControlInfo_Save_Data(&(pFd->pWallet->Control),pIn,pIn+(inlen-2));
					//--------保存到文件--------------
					APP_FileSaveBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead));
					uRet = 0;
				}
				else
				{
					uRet = 1;
				}
				//--------Save OK---------------------------------
				*pDataRet++= 0x80|pIn[0];
				*pDataRet++= uRet;
				*pDataRet++= 0;
			}
			break;
		case 0x04:	//申请P10签名
			{
				uWalletFD* pFd;
				u8 p10_buff[128],sign[64],*pHead,*pKey;
				pFd=gGetWalletPfd(pIn[1]);
				pIn += 2;
				inlen -= 2;
				if(pFd)
				{
					if(pFd->pNewKey) gFree(pFd->pNewKey);
					pFd->pNewKey = (SM2_PRIVKEY *)gMalloc(sizeof(SM2_PRIVKEY));
					sm2_generate_keypair(pFd->pNewKey->xQ,pFd->pNewKey->d);
					//------------组建立P10信息-------------------------
					pHead=p10_buff+2;
					
					pKey=pHead+2;
					//------Info---------------------------
					memcpy(pKey,pIn,inlen);
					pKey += inlen;
					//-------PuKey----------------------
					memcpy(pKey,"\x30\x59",2);
					pKey += 2;
					memcpy(pKey,"\x30\x13\x06\x07\x2a\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x81\x1c\xcf\x55\x01\x82\x2d",21);
					pKey += 21;
					memcpy(pKey,"\xA1\x44\x03\x42\x00\x04",6);
					pKey += 6;
					memcpy(pKey,pFd->pNewKey->xQ,32);
					pKey += 32;
					memcpy(pKey+32,pFd->pNewKey->yQ,32);
					pKey += 32;
					//--------------------------------------------
					pHead[0]=0x30;
					pHead[1]=(pKey-pHead)-2;
					//----------------sign-------------------
					api_sm_sign2(pFd->pNewKey->d,pHead,pKey-pHead,sign);
					//---------------------------------------------------------------
					memcpy(pKey,"\x30\x0a\x06\x08\x2a\x81\x1c\xcf\x55\x01\x83\x75",12);
					pKey += 12;
					memcpy(pKey,"\x03\x47\x00\x30\x44\x02\x20\x31\xa1",9);
					pKey += 9;
					memcpy(pKey,sign,64);
					pKey += 64;
					//----------------------------------------------
					p10_buff[0]=0x30;
					p10_buff[1]=(pKey-p10_buff)-2;

					//--------------ReturnData---------------------------------------
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 0;
					//-----P10_Len[2]-----	
					*pDataRet++= 0;
					*pDataRet++= (pKey-p10_buff);
					memcpy(pDataRet,p10_buff,(pKey-p10_buff));
					pDataRet += (pKey-p10_buff);
					//-----sign--oldKey-----------------
					*pDataRet++= 0;
					*pDataRet++= 64;
					api_sm_sign2(pFd->pNewKey->d,p10_buff,pKey-p10_buff,sign);
					pDataRet += 64;
				}
				else 
				{
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 1;
				}
			}
			break;
		case 0x05://下载证书
			{
				uWalletFD* pFd;
				u8 cerType,fd=pIn[1];
				pFd=gGetWalletPfd(fd);
				if(pFd)
				{
					//char fName[32];
					//int ret;
					cerType=pIn[2];
					pIn += 3;
					inlen -= 3;
					SavePubKeyCert(pFd->WalletID,cerType,pIn,inlen);
					/*
					CertKey *pCer;
					sprintf(fName,"%s.cer",pFd->pWallet->Info.WalletID);
					pCer = (CertKey *)gMalloc(sizeof(CertKey));
					memset(pCer,0xff,sizeof(CertKey));
					APP_FileReadBuff(fName,0,pCer,sizeof(CertKey));
					if(cerType==0x01)	//人行公钥证书
					{
						memcpy(pCer->PbcPubKeyCert,pRecvData,ReLen);
					}
					else if(cerType==0x02)// 运营机构证书
					{
						memcpy(pCer->InstructingPatryPubKeyCert,pRecvData,ReLen);
					}
					else if(cerType==0x03)//个人公钥证书
					{
						memcpy(pCer->PersPubKeyCert,pRecvData,ReLen);
						//--------------个人私钥需加密存储-----------------
						memcpy(pCer->PersPriKeyCert,pFd->pNewKey,sizeof(SM2_PRIVKEY));
						sm4_ecb_encrypt_decrypt(ALG_ENCRYPT,pFd->Enckey,(UINT8*)pCer->PersPriKeyCert,(UINT8*)pCer->PersPriKeyCert,sizeof(SM2_PRIVKEY));
						if(pFd->pNewKey)
						{//------替换密钥----开始使用新密钥----------
							gFree(pFd->pKey);
							pFd->pKey = pFd->pNewKey;
							pFd->pNewKey = NULL;
						}
					}
					else if(cerType==0x04)//初始个人私钥参数
					{
						memcpy(pCer->PersPriKeyCert,pRecvData,ReLen);
						//--------------个人私钥需加密存储-----------------
						sm4_ecb_encrypt_decrypt(ALG_ENCRYPT,pFd->Enckey,(UINT8*)pCer->PersPriKeyCert,(UINT8*)pCer->PersPriKeyCert,sizeof(SM2_PRIVKEY));
					}
					APP_FileSaveBuff(fName,0,pCer,sizeof(CertKey));
					gFree(pCer);
					*/

					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 0;
					*pDataRet++= 0;

				}
				else
				{
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 1;
					*pDataRet++= 0;
				}
			}
			break;
		case 0x06://交易付款
			{
				u32 Amount;
				int index;
				//u8 RandB[8];				
				uPaymentApp *pInBack;
				uWalletFD* pFd;
				pFd=gGetWalletPfd(pIn[1]);
				if(pFd)
				{
					pInBack = (uPaymentApp *)pIn+2;
					Amount=Conv_Hex8ToNum(pInBack->amount,6);
					if(Amount <= pFd->pRealizeInfo->uAvailableBal)
					{
						index=CheckVoucherAmount(pFd->pWallet,Amount);
						if(index >= 0)
						{
							int outLen;
							TransSubChain tTransSub;
							uBalVoucher *pBal;
							

							tTransSub.aTag =0x51;
							tTransSub.aLen =sizeof(tTransSub.Amount);
							Conv_NumToDecimal(Amount,tTransSub.Amount,sizeof(tTransSub.Amount));
							
							tTransSub.tTag =0x52;
							tTransSub.tLen =sizeof(tTransSub.Amount);
							//Conv_NumToDecimal(Amount,tTransSub.TransactionIndex,sizeof(tTransSub.Amount));

							tTransSub.cTag =0x53;
							tTransSub.cLen =sizeof(tTransSub.CreditorPubKey);
							memcpy(tTransSub.CreditorPubKey,pInBack->PubKey,tTransSub.cLen);
							
							tTransSub.dTag =0x54;
							tTransSub.dLen =sizeof(tTransSub.DebtorSign);
							pBal=ReadBalVoucher(GetWalletName(index,pFd->WalletID));
							if(TransSubChainSign(pFd->pKey,pBal,&tTransSub))
							{
								gFree(pBal);
								break;
							}
							pFd->pWallet->dc[index].InvVoucherBal -= ~((~pFd->pWallet->dc[index].InvVoucherBal) -Amount);
							//-------------Return-------------------------------------
							*pDataRet++= 0x80|pIn[0];
							*pDataRet++= 0;
							outLen=ReadPaymentVoucher(pBal,&tTransSub,pDataRet);
							gFree(pBal);
							pDataRet += outLen;
							outLen=ReadPubKeyCert(pFd->WalletID,CERT_INS,pDataRet,256);
							pDataRet += outLen;
						}
						//TransSubChainSign
					}
					else	//-------余额不够-----------
					{

					}
				}					
				*pDataRet++= 0x80|pIn[0];
				*pDataRet++= 0;
				*pDataRet++= 0;
			}

			break;
		case 0x07://交易收款请求
		{
				int 	ret;
				UINT32	outLen;
				DfCommOutPack *pRetPack;
				DfRecviveInit *pInBack;
				DfRecviveResI *pOutResI;
				u8				*p;
				uWalletFD* pFd;
				pFd=gGetWalletPfd(pIn[1]);
				if(pFd)
				{
					SM2_PUBKEY *pPuKey=NULL;
					//----------检查钱包空间----------------
					pFd->index=FindVoucherNull(pFd->pWallet);
					if(pFd->index < 0)
					{
						*pDataRet++= 0x80|pIn[0];
						*pDataRet++= 5;
						strcpy(pDataRet+1,"没有储存空间");
						*pDataRet++= 12;
						pDataRet += 12;
						break;
					}
					pInBack = (DfRecviveInit *)pIn+2;
					//------------解析验证证书-----------------
					{
						u16 len;
						u8 *pCert =pInBack->CertChainB;
						//-------运营机构交易证书-----------
						if(*pCert == 0x30) pCert++;
						parse_asn1_len(len,pCert);

						pCert += len;
						//-------硬件钱包交易证书----------
						if(*pCert == 0x30) pCert++;
						parse_asn1_len(len,pCert);
						//06 08 2a 81 1c cf 55 01 82 2d
						pCert=eMemseek(pCert,len,"\x06\x08\x2a\x81\x1c\xcf\x55\x01\x82\x2d",10);
						if(pCert)
						{
							pCert++ ;
							parse_asn1_len(len,pCert);
							if(len > 64)
							{
								pCert += len-64;
								pPuKey = (SM2_PUBKEY *)pCert;
							}
						}
					}
					if(pPuKey == NULL) break;
					memcpy(pFd->amount,pInBack->amount,sizeof(pFd->amount));
					memcpy(pFd->TimeStamp,pInBack->TimeStamp,sizeof(pFd->TimeStamp));
					memcpy(pFd->RandB,pInBack->RandB,sizeof(pFd->RandB));

					pRetPack = (DfCommOutPack *)pIn;
					pRetPack->RetTag = 0x87;
					pRetPack->errcode = 0;
					pOutResI =(DfRecviveResI *)pRetPack->data;
					api_rand(pFd->RandA,sizeof(pFd->RandA)/4); //1*4 =4
					api_rand(pFd->FactA,sizeof(pFd->RandA)/4);	//4*4 =16
					memcpy(pOutResI->RandA,pFd->RandA,sizeof(pOutResI->RandA));
					memcpy(pOutResI->FactA,pFd->FactA,16);
					sm2_encrypt(pPuKey,(u8*)pFd->FactA,16,pOutResI->FactA,&outLen);
					{//TransType,TransIndex,Amount,TimeStamp
						u8 buff[512];
						p=buff;
						memcpy(p,pFd->TransType,sizeof(pFd->TransType));
						p += sizeof(pFd->TransType);
						//---------生成交易索引----------------
						memcpy(pFd->TransIndex,pFd->RandA,sizeof(pFd->RandA));
						memcpy(&pFd->TransIndex[sizeof(pFd->RandA)],pFd->RandB,sizeof(pFd->RandB));
						memcpy(p,pFd->TransIndex,sizeof(pFd->TransIndex));
						p += sizeof(pFd->TransIndex);
						memcpy(p,pFd->amount,sizeof(pFd->amount));
						p += sizeof(pFd->amount);
						memcpy(p,pFd->TimeStamp,sizeof(pFd->TimeStamp));
						p += sizeof(pFd->TimeStamp);
						api_sm_sign2(pFd->pKey->d,buff,p-buff,pOutResI->SigA);
					}
					p = pOutResI->CertChainA;
					//------------------打包证书-----------------
					ret=ReadPubKeyCert(pFd->WalletID,CERT_INS,p,256);
					p += ret;
					ret=ReadPubKeyCert(pFd->WalletID,CERT_PERS,p,256);
					p += ret;
					pDataRet = p;
				}
				else
				{
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 1;
					*pDataRet++= 0;
				}
			};
			break;
		case 0x08://交易收款确定	
			{
				int 	ret;
				UINT32	outLen;
				//DfCommOutPack *pRetPack;
				DfRecviveOperation	*pInBack;
				//DfRecviveResOpe	*pOutResI;
				u8				*pPaymentVoucher,*p;
				uWalletFD* pFd;
				pFd=gGetWalletPfd(pIn[1]);
				if(pFd)
				{
					u8 skeyBuff[40];
					pInBack = (DfRecviveOperation *)pIn+2;
					inlen -= 2;
					//------------解析验证证书-----------------
					sm2_decrypt(pFd->pKey->d,pInBack->FactB,sizeof(pInBack->FactB),(u8*)pFd->FactB,&outLen);
					p = skeyBuff;
					Conv_buffXor(p,(u8*)pFd->FactA,(u8*)pFd->FactB,sizeof(pFd->FactB));
					p += sizeof(pFd->FactB);
					strcpy(p,"ENC&MAC"); p += 7;
					Conv_buffXor(p,(u8*)pFd->RandA,(u8*)pFd->RandB,sizeof(pFd->RandB));
					p += sizeof(pFd->RandB);
					api_md_hash(MODE_SM3,skeyBuff,p-skeyBuff,(u8*)&pFd->sesKey);	//得到会话密钥
					inlen -= sizeof(DfRecviveOperation);
					if(inlen > 0)
					{
						PaymentVoucher *pPay;
						pPaymentVoucher = gMalloc(inlen);
						memcpy(pPaymentVoucher,pInBack->PaymentVoucher,inlen);
						sm4_en_de_crypt(ALG_DECRYPT,ALG_CBC,ALG_COFBIT_DUMMY,
											(UINT32 *)pPaymentVoucher,
											(UINT32 *)pPaymentVoucher,
											inlen,
											pFd->sesKey.Enc,
											NULL,
											0,	 //false
											0,	 //false
											1	 //true
											);
						
						outLen=pPaymentVoucher[0]*256 + pPaymentVoucher[1];
						pPay = (PaymentVoucher *)(pPaymentVoucher+2);
						ret = PaymentVoucherVerify(pPay,outLen);
						if(ret >= 0)
						{
							u32 Amount;
							uBalVoucher *pBal;
							pBal=PaymentToBalVouche(pPay,NULL,NULL);
							SaveBalVoucher(GetWalletName(pFd->index,pFd->WalletID),pBal);
							gFree(pBal);
							//-------记录金额---------------------
							Amount=Conv_Hex8ToNum(pFd->amount,sizeof(pFd->amount));
							pFd->pWallet->dc[ret].InvVoucherBal = ~Amount;
							pFd->pRealizeInfo->uBal += Amount;
							pFd->pRealizeInfo->uAvailableBal += Amount;
							pFd->pRealizeInfo->uOffTranAmt += Amount;

							pFd->pRealizeInfo->uDayPayAmt += Amount;
							pFd->pRealizeInfo->uMonthPayAmt += Amount;
							pFd->pRealizeInfo->uYearPayAmt += Amount;
							
							pFd->pRealizeInfo->uATC++;
							//----存交易流水------------
							

							DfTransactionBill* pbill;
							pbill = (DfTransactionBill*)gMalloc(sizeof(DfTransactionBill)+256);
							Conv_NumToDecimal(Amount,pbill->tBill.Amount,sizeof(pbill->tBill.Amount));
							memcpy(pbill->tBill.TransactionType,pFd->TransType,sizeof(pbill->tBill.TransactionType));
							memcpy(pbill->tBill.TransactionType,pFd->TransType,sizeof(pbill->tBill.TransactionType));
							//SaveTransactionBill(GetWalletName(  ,pFd->WalletID),DfTransactionBill *pBill,int blen)
							gFree(pbill);
						}
						gFree(pPaymentVoucher);
					}

				}
				else
				{
					*pDataRet++= 0x80|pIn[0];
					*pDataRet++= 1;
					*pDataRet++= 0;
				}
			}
			break;
		case 0x09://同步请求
			{
				UINT32	wLen,amount;
				DfCommOutPack *pRetPack;
				DfRecviveResSyn *pOutRes;
				u8				*pOut;
				uWalletFD* pFd;
				pFd=gGetWalletPfd(pIn[1]);

				pRetPack =(DfCommOutPack *)pIn;
				pOutRes = (DfRecviveResSyn *)pRetPack->data;
				if(pFd)
				{
					u16 i,max;
					uBalVoucher* pBal;
					
					max = sizeof(pFd->pWallet->dc)/sizeof(pFd->pWallet->dc[0]);;
					amount = 0;
					pOut = pOutRes->Voucher;
					for(i=0;i<max;i++)
					{
						pBal=ReadBalVoucher(GetWalletName(pFd->index,pFd->WalletID));
						if(pBal)
						{
							u16 num,maxNum;
							amount += ~pFd->pWallet->dc[i].InvVoucherBal;
							wLen = sizeof(DfDC_bt) + (pBal->SubChainNun *sizeof(DfTranSubChain));
						
							*pOut++ = 0x61; //DC余额凭证 TAG=0x61
							if(wLen >= 0x100) 
							{
								*pOut++ = 0x82;
								*pOut++ = wLen/0x100;
								*pOut++ = wLen&0xff;
							}
							else
							{
								*pOut++ = 0x81;
								*pOut++ = wLen;
							}
							memcpy(pOut,&pBal->tDc,sizeof(DfDC_bt));
							pOut += sizeof(DfDC_bt);
							maxNum = pBal->SubChainNun;
							for(num = 0; num < maxNum;num++)
							{
								memcpy(pOut,&pBal->tranChain[num],sizeof(DfTranSubChain));
								pOut += sizeof(DfTranSubChain);
							}
							gFree(pBal);
						}
					}
					pDataRet = pOut;
					Conv_NumToHex(amount,pOutRes->amount,sizeof(pOutRes->amount));
				}
				pRetPack->RetTag = 0x89;
				pRetPack->errcode = 0;
			}
			break;
		case 0x0a://同步确认
			{
				DfCommInPack *pInPack;
				uWalletFD* pFd;
				pInPack =(DfCommInPack *)pIn;
				pFd=gGetWalletPfd(pInPack->fd);
				if(pFd)
				{
					u16 i,max;
					DfRecviveDcSyn *pRecvDc;
					uBalVoucher *pBal;
					pRecvDc =(DfRecviveDcSyn*)pInPack->data;
					pBal =(uBalVoucher*)pRecvDc->pDc;
					if(InDcVerify(&pBal->tDc) == 0)
					{
						max = sizeof(pFd->pWallet->dc)/sizeof(pFd->pWallet->dc[0]);
						for(i=0;i<max;i++)
						{
							if(pFd->pWallet->dc[i].InvFlag == ~0)
								continue;
							API_fremove(GetWalletName(i,pFd->WalletID));
							pFd->pWallet->dc[i].InvFlag= ~0;
							pFd->pWallet->dc[i].InvVoucherBal= ~0;
						}
						pFd->pWallet->dc[0].InvVoucherBal= ~ Conv_Hex4ToNum(pBal->tDc.dc.Denomination,sizeof(pBal->tDc.dc.Denomination));
						pBal->SubChainNun = 0;	//没有子链
						SaveBalVoucher(GetWalletName(0,pFd->WalletID),pBal);
						pFd->pWallet->dc[0].InvFlag ='D';

						pFd->pRealizeInfo->uBal = ~pFd->pWallet->dc[0].InvVoucherBal;
						pFd->pRealizeInfo->uAvailableBal = pFd->pRealizeInfo->uBal;
						pFd->pRealizeInfo->uInTransmitBal = 0;
						pFd->pRealizeInfo->uFreezeBal = 0;
						pFd->pRealizeInfo->OffTranTimes = 0;
						pFd->pRealizeInfo->uOffTranAmt = 0;
						memcpy(pFd->pRealizeInfo->LastUpdateTime,pRecvDc->DateTime,sizeof(pFd->pRealizeInfo->LastUpdateTime));
					}
					*pDataRet++ = 0x8a;
					*pDataRet++ = 0;
					*pDataRet++ = 0;
				}
				else
				{
					*pDataRet++ = 0x8a;
					*pDataRet++ = 1;
					*pDataRet++ = 0;
				}
			}
			break;
		case 0x0b://查看金额
			{
				uWalletFD* pFd;
				DfCommOutPack *pRet;
				pRet = (DfCommOutPack *)pDataRet;
				pFd=gGetWalletPfd(pIn[1]);
				if(pFd)
				{
					pRet->errcode = 0;
					pDataRet = pRet->data;
					Conv_NumToDecimal(pFd->pRealizeInfo->uBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uAvailableBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uInTransmitBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uFreezeBal,pDataRet,9);
					pDataRet += 9;
				}
				else
				{
					pRet->errcode = 1;
					pDataRet = pRet->data;
					strcpy((char*)pDataRet,"钱包句柄不存在");
					pDataRet += 15;
				}
				pRet->RetTag = 0x8b;
			}
			break;
		}
	return pDataRet-pOut;
}
#endif
int WalletDataParseApdu(DC_APDU *pAPDU,u8 *pOut)
{
	u8* pDataRet=pOut;
	int inlen=pAPDU->lc[0]*256 + pAPDU->lc[1];
	if(pAPDU->cla != 0x80 || inlen > 1024)
	{
		*pOut++ = 0x82;
		*pOut++ = 0x05;
		return 2;
	}
	switch(pAPDU->ins)
	{
		case 0x41:	//创建钱包
			{
				//u8 buff[16],PWCheck[4];
				uWalletFD* pFd;
				WalletInfo	*pSave;
				uWalletPort *pInfo= (uWalletPort *)pAPDU->data;
				if(pAPDU->p1 == 0x00)
				{
					pFd = gCreateWallet(pInfo->WalletID,pInfo->WalletName,NULL);
					if(pFd)
					{
						//memcpy(pFd->Enckey,buff,sizeof(pFd->Enckey)); 					
						pSave = &pFd->pWallet->Info;
						memcpy(pSave->WalletID,pInfo->WalletID,sizeof(pSave->WalletID));
						memcpy(pSave->WalletLevel,pInfo->WalletLevel,sizeof(pSave->WalletLevel));
						memcpy(pSave->WalletType,pInfo->WalletType,sizeof(pSave->WalletType));
						memcpy(pSave->WalletName,pInfo->WalletName,sizeof(pSave->WalletName));
						memcpy(pSave->CustTelePhone,pInfo->CustTelePhone,sizeof(pSave->CustTelePhone));
						memcpy(pSave->CustEmail,pInfo->CustEmail,sizeof(pSave->CustEmail));
						memcpy(pSave->FinInstCode,pInfo->FinInstCode,sizeof(pSave->FinInstCode));
						memcpy(pSave->CustName,pInfo->CustName,sizeof(pSave->CustName));
						memcpy(pSave->WalletSetTime,pInfo->WalletSetTime,sizeof(pSave->WalletSetTime));
						memcpy(pSave->IDType,pInfo->IDType,sizeof(pSave->IDType));
						memcpy(pSave->IDNo,pInfo->IDNo,sizeof(pSave->IDNo));
						memcpy(pSave->WalletStatus,"WS05",sizeof(pSave->WalletStatus));
						
						APP_FileSaveBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead));
						//WalletReadData(pFd->WalletID,PWCheck);
	
						memcpy(pFd->WalletID,pSave->WalletID,sizeof(pInfo->WalletID));
						*pDataRet++= 0x90;
						*pDataRet++= 0x00;
					}
					else
					{
						strcpy((char*)pDataRet,"空间不足,请检查存状态");
						pDataRet += 64;
						*pDataRet++= 0x82;
						*pDataRet++= 0x02;
					}

				}
				else if(pAPDU->p1 == 0x02)
				{
					DfFileHeadInfo *pAll;
					int max,i;
					pAll=GetWalletHead(&max);
					for(i=0;i<max;i++)
					{
						memcpy(pDataRet,pAll[i].WalletID,sizeof(pAll[i].WalletID));
						pDataRet += sizeof(pAll[i].WalletID);
						memcpy(pDataRet,pAll[i].WalletName,sizeof(pAll[i].WalletName));
						pDataRet += sizeof(pAll[i].WalletName);
					}
					gFree(pAll);
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else if(pAPDU->p1 == 0x03)
				{
					//----测试专用------
					#ifdef WALLET_TEST_DATA
					if(pAPDU->data[1]==0x01)
						SetCurrWallet(pRecvWallet);
					else
						SetCurrWallet(pPayWallet);
					#endif
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
			}
			break;
		case 0x47:	//删钱包
			{
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					//if(pIn[0]&0x40)	//删除钱包
					{
						//char ucheck[4];
						//----------清除币串信息------------------
						{
							u16 i,max;
							max = sizeof(pFd->pRealizeInfo->VoucherBal)/sizeof(pFd->pRealizeInfo->VoucherBal[0]);
							for(i=0;i<max;i++)
							{
								if(pFd->pRealizeInfo->VoucherBal[i] == ~0)	//ff 为NULL
									continue;
								API_fremove(GetWalletName(i,pFd->WalletID));
								pFd->pRealizeInfo->VoucherBal[i]= ~0;
							}
						}
						//-----------删除钱包数据----------------------------
						API_fremove(GetWalletName(WAL_HEAD,pFd->WalletID));
						API_fremove(GetWalletName(WAL_REAL,pFd->WalletID));
						API_fremove(GetWalletName(WAL_CERK,pFd->WalletID));
					}
					//----------清除钱包数据-----------------
					FreeNull(pFd->pKey);
					FreeNull(pFd->pNewKey);
					FreeNull(pFd->pRealizeInfo);
					FreeNull(pFd->pWallet);
					memset(pFd->Enckey,0xFF,sizeof(pFd->Enckey));
					
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
			}
			break;
		case 0x42:	//更新控制参数
			{
				uWalletFD* pFd;
				//u8 uRet;
				//gFILE* pfd;
				pFd=gGetWalletPfd();
				if(pFd!=NULL && pFd->pWallet != NULL)
				{
					ControlInfo_Save_Data(&(pFd->pWallet->Control),pAPDU->data,pAPDU->data+inlen);
					//--------保存到文件--------------
					APP_FileSaveBuff(GetWalletName(WAL_HEAD,pFd->WalletID),(u32*)pFd->pWallet,sizeof(uWalletHead));

					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else
				{
					strcpy((char*)pDataRet,"没有钱包,请创立钱包");
					pDataRet += 64;
					*pDataRet++= 0x82;
					*pDataRet++= 0x01;
				}
			}
			break;
		case 0x43:	//申请P10签名
			{
				uWalletFD* pFd;
				u8 p10_buff[128],sign[64],*pHead,*pKey;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					if(pFd->pNewKey) gFree(pFd->pNewKey);
					pFd->pNewKey = (SM2_PRIVKEY *)gMalloc(sizeof(SM2_PRIVKEY));
					sm2_generate_keypair(pFd->pNewKey->xQ,pFd->pNewKey->d);
					//------------组建立P10信息-------------------------
					pHead=p10_buff+2;
					
					pKey=pHead+2;
					//------Info---------------------------
					memcpy(pKey,pAPDU->data,inlen);
					pKey += inlen;
					//-------PuKey----------------------
					memcpy(pKey,"\x30\x59",2);
					pKey += 2;
					memcpy(pKey,"\x30\x13\x06\x07\x2a\x86\x48\xce\x3d\x02\x01\x06\x08\x2a\x81\x1c\xcf\x55\x01\x82\x2d",21);
					pKey += 21;
					memcpy(pKey,"\xA1\x44\x03\x42\x00\x04",6);
					pKey += 6;
					memcpy(pKey,pFd->pNewKey->xQ,32);
					pKey += 32;
					memcpy(pKey+32,pFd->pNewKey->yQ,32);
					pKey += 32;
					//--------------------------------------------
					pHead[0]=0x30;
					pHead[1]=(pKey-pHead)-2;
					//----------------sign-------------------
					api_sm_sign2(pFd->pNewKey->d,pHead,pKey-pHead,sign);
					//---------------------------------------------------------------
					memcpy(pKey,"\x30\x0a\x06\x08\x2a\x81\x1c\xcf\x55\x01\x83\x75",12);
					pKey += 12;
					memcpy(pKey,"\x03\x47\x00\x30\x44\x02\x20\x31\xa1",9);
					pKey += 9;
					memcpy(pKey,sign,64);
					pKey += 64;
					//----------------------------------------------
					p10_buff[0]=0x30;
					p10_buff[1]=(pKey-p10_buff)-2;

					//--------------ReturnData---------------------------------------
					*pDataRet++= 0;
					//-----P10_Len[2]-----	
					*pDataRet++= 0;
					*pDataRet++= (pKey-p10_buff);
					memcpy(pDataRet,p10_buff,(pKey-p10_buff));
					pDataRet += (pKey-p10_buff);
					//-----sign--oldKey-----------------
					*pDataRet++= 0;
					*pDataRet++= 64;
					api_sm_sign2(pFd->pNewKey->d,p10_buff,pKey-p10_buff,sign);
					pDataRet += 64;
					
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else 
				{
					strcpy((char*)pDataRet,"没有钱包,请创立钱包");
					pDataRet += 64;
					*pDataRet++= 0x82;
					*pDataRet++= 0x01;
				}
			}
			break;
		case 0x44://下载证书
			{
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					SavePubKeyCert(pFd->WalletID,pAPDU->p1,pAPDU->data,inlen);
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else
				{
					strcpy((char*)pDataRet,"没有钱包,请创立钱包");
					pDataRet += 64;
					*pDataRet++= 0x82;
					*pDataRet++= 0x01;
				}
			}
			break;
		case 0x50:	//初始化
			{
				int 	ret;
				//UINT32	outLen;
				u8				*p;
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					//SM2_PUBKEY *pPuKey=NULL;
					if(pAPDU->p1 == 1) //初始化收款（后续接收支付凭证）
					{
						DfOnLineRecvInit *pInBack;
						DfOnLineRecvRet *pOutResI;
						//----------检查钱包空间----------------
						pFd->index=FindVoucherNull(pFd->pRealizeInfo);
						if(pFd->index < 0)
						{//--------SW---------------------
							strcpy((char*)pDataRet,"已经到达最大上限,请同步");
							pDataRet += 64;
							*pDataRet++= 0x91;
							*pDataRet++= 0x03;
							break;
						}
						pInBack = (DfOnLineRecvInit *)pAPDU->data;
						memcpy(pFd->CreditorInsPartyID,pInBack->CreditorInsPartyID,sizeof(pFd->CreditorInsPartyID));
						memcpy(pFd->CreditorWalletID,pInBack->CreditorWalletID,sizeof(pFd->CreditorWalletID));
						memcpy(pFd->RandB,pInBack->RandB,sizeof(pFd->RandB));
						pFd->Amount=Conv_Hex4ToNum(pInBack->amount,sizeof(pInBack->amount));
						memcpy(pFd->TimeStamp,pInBack->TimeStamp,sizeof(pFd->TimeStamp));
						memcpy(pFd->RandB,pInBack->RandB,sizeof(pFd->RandB));

						//pInBack->keyflag	//密钥标志符
						
						pOutResI =(DfOnLineRecvRet *)pDataRet;
						api_rand(pFd->RandA,sizeof(pFd->RandA)/4); //1*4 =4
						api_rand(pFd->FactA,sizeof(pFd->FactA)/4);	//4*4 =16
						
						Conv_NumToHex(pFd->pRealizeInfo->uATC,pOutResI->ATC,sizeof(pOutResI->ATC));
						Conv_NumToHex(pFd->pRealizeInfo->uAvailableBal,pOutResI->WallAmount,sizeof(pOutResI->WallAmount));

						memcpy(pOutResI->InsPartyID,pFd->pWallet->Info.FinInstCode,sizeof(pOutResI->InsPartyID));
						memcpy(pOutResI->WalletID,pFd->WalletID,sizeof(pOutResI->WalletID));
						//memcpy(pOutResI->Random,pFd->RandA,sizeof(pOutResI->Random));
						memcpy(pOutResI->TimeStamp,pFd->TimeStamp,sizeof(pOutResI->TimeStamp));
						p = pOutResI->Certification;
						//------------------打包证书-----------------
						ret=ReadPubKeyCert(pFd->WalletID,CERT_INS,p,256);
						p += ret;
						ret=ReadPubKeyCert(pFd->WalletID,CERT_PERS,p,256);
						p += ret;
						//-------------------签名[64]---------------------
					}
					else if(pAPDU->p1 == 2) //初始化支付（后续生成支付凭证）
					{
						//DfPaymentInit *pInBack;
						//DfPaymentResI *pOutResI;
						/*
						//pInBack = (DfPaymentInit *)pAPDU->data;
						memcpy(pFd->amount,pAPDU->data,sizeof(pFd->amount));
						memcpy(pFd->TimeStamp,pAPDU->data[sizeof(pFd->amount)],sizeof(pFd->TimeStamp));
						api_rand(pFd->RandB,sizeof(pFd->RandB)/4); //1*4 =4
						memcpy(pDataRet,pFd->RandB,sizeof(pFd->RandB));
						pDataRet += sizeof(pFd->RandB);
						
						ret=ReadPubKeyCert(pFd->WalletID,CERT_INS,pDataRet,256);
						if(ret > 0) pDataRet+= ret;
						ret=ReadPubKeyCert(pFd->WalletID,CERT_PERS,pDataRet,256);
						if(ret > 0) pDataRet+= ret;
						*/
					}
					else if(pAPDU->p1 == 3) //初始化同步更新
					{
						DfOnLineRecvRet *pOutResI;
						u8 buffdata[128];
						if(inlen >= 6)
						{
							memcpy(pFd->TimeStamp,pAPDU->data,inlen);
						}
						pOutResI =(DfOnLineRecvRet *)pDataRet;
						Conv_NumToHex(pFd->pRealizeInfo->uATC,pOutResI->ATC,sizeof(pOutResI->ATC));
						pOutResI->keyVersion = 0x01;
						pOutResI->AlgoFlag = 0x01;
						memcpy(pOutResI->InsPartyID,pFd->pWallet->Info.FinInstCode,sizeof(pOutResI->InsPartyID));
						memcpy(pOutResI->WalletID,pFd->WalletID,sizeof(pOutResI->WalletID));
						Conv_NumToHex(pFd->pRealizeInfo->uBal,pOutResI->WallAmount,sizeof(pOutResI->WallAmount));
						//api_rand(pFd->RandA,sizeof(pFd->RandA)/4);	//4*4 =16
						memcpy(pOutResI->TimeStamp,pFd->TimeStamp,sizeof(pOutResI->TimeStamp));
						memcpy(pOutResI->LastTradeIndex,pFd->pRealizeInfo->LastTradeIndex,sizeof(pOutResI->LastTradeIndex));
						
						//----------------GetMac1----------------------------------
						p = buffdata;
						memcpy(p,pOutResI->InsPartyID,sizeof(pOutResI->InsPartyID));
						p += sizeof(pOutResI->InsPartyID);
						memcpy(p,pOutResI->WalletID,sizeof(pOutResI->WalletID));
						p += sizeof(pOutResI->WalletID);
						*p++ = 0x03;	//TransType;
						//memcpy(p,pOutResI->Random,sizeof(pOutResI->Random));
						//p += sizeof(pOutResI->Random);
						memcpy(p,pFd->TimeStamp,sizeof(pFd->TimeStamp));
						p += sizeof(pOutResI->TimeStamp);
						GetMAC_ATC((u8*)"test_MacKey",pOutResI->ATC,buffdata,p-buffdata,pOutResI->MAC1);

						pDataRet = pOutResI->Certification;
					}
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else
				{
					*pDataRet++= 0x51;
					*pDataRet++= 0x02;
				}
			}
			break;
		case 0x52://在线收款（ONLINE RECEIVING）
			{
				uWalletFD* pFd=gGetWalletPfd();
				if(pFd)
				{
					u16 i,max;
					uBalVoucher *pBal;
					pBal =(uBalVoucher*)pAPDU->data;
					if(InDcVerify(&pBal->tDc) == 0)
					{
						max = sizeof(pFd->pRealizeInfo->VoucherBal)/sizeof(pFd->pRealizeInfo->VoucherBal[0]);
						for(i=0;i<max;i++)
						{
							if(pFd->pRealizeInfo->VoucherBal[i] == ~0)
								continue;
							API_fremove(GetWalletName(i,pFd->WalletID));
							pFd->pRealizeInfo->VoucherBal[i]= ~0;
						}
						pFd->pRealizeInfo->VoucherBal[0]= Conv_Hex4ToNum(pBal->tDc.dc.Denomination,sizeof(pBal->tDc.dc.Denomination));
						pBal->SubChainNun = 0;	//没有子链
						SaveBalVoucher(GetWalletName(0,pFd->WalletID),pBal);

						pFd->pRealizeInfo->uBal = pFd->pRealizeInfo->VoucherBal[0];
						pFd->pRealizeInfo->uAvailableBal = pFd->pRealizeInfo->uBal;
						pFd->pRealizeInfo->uInTransmitBal = 0;
						pFd->pRealizeInfo->uFreezeBal = 0;
						pFd->pRealizeInfo->OffTranTimes = 0;
						pFd->pRealizeInfo->uOffTranAmt = 0;
						//memcpy(pFd->pRealizeInfo->LastUpdateTime,pRecvDc->DateTime,sizeof(pFd->pRealizeInfo->LastUpdateTime));
						APP_FileSaveCont(GetWalletName(WAL_REAL,pFd->WalletID),(u32*)pFd->pRealizeInfo,sizeof(uSystemRealizeInfo));
					}
					//----------Mac5------------
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					//----------SW---------------
					*pDataRet++ = 0x90;
					*pDataRet++ = 0x00;
				}
				else
				{
					strcpy((char*)pDataRet,"没有钱包,请创立钱包");
					pDataRet += 64;
					*pDataRet++= 0x82;
					*pDataRet++= 0x01;
				}
			}
			break;
		
		case 0x54://在线支付（ONLINE PAYMENT）
			{
				uWalletFD* pFd=gGetWalletPfd();
				if(pFd)
				{
					int outLen;
					TransSubChain tTransSub;
					uBalVoucher *pBal;
					pBal=ReadBalVoucher(GetWalletName(pFd->index,pFd->WalletID));
					if(pBal == NULL)
					{
						strcpy((char*)pDataRet,"没有币串,请兑出数字币");
						pDataRet += 64;
						*pDataRet++= 0x82;
						*pDataRet++= 0x02;
						break;
					}
					//-----------tTransSub-----------------------------
					tTransSub.aTag =0x51;
					tTransSub.aLen =sizeof(tTransSub.Amount);
					Conv_NumToDecimal(pFd->Amount,tTransSub.Amount,sizeof(tTransSub.Amount));
					
					tTransSub.tTag =0x52;
					tTransSub.tLen =sizeof(tTransSub.TransactionIndex);
					memcpy(tTransSub.TransactionIndex,pFd->TransIndex,sizeof(tTransSub.TransactionIndex));

					tTransSub.cTag =0x53;
					tTransSub.cLen =sizeof(tTransSub.CreditorPubKey);
					memcpy(tTransSub.CreditorPubKey,pAPDU->data,tTransSub.cLen);
					
					tTransSub.dTag =0x54;
					tTransSub.dLen =sizeof(tTransSub.DebtorSign);
					if(TransSubChainSign(pFd->pKey,pBal,&tTransSub))
					{
						gFree(pBal);
						break;
					}
					pFd->pRealizeInfo->uBal -= pFd->Amount;
					pFd->pRealizeInfo->uAvailableBal -= pFd->Amount;
					pFd->pRealizeInfo->VoucherBal[pFd->index] -= pFd->Amount;
					//-------------Return-------------------------------------
					outLen=ReadPaymentVoucher(pBal,&tTransSub,(PaymentVoucher *)pDataRet);
					gFree(pBal);
					pDataRet += outLen;
					//----------MAC3------------
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					//----------SW---------------
					*pDataRet++ = 0x90;
					*pDataRet++ = 0x00;
				}
				else
				{
					strcpy((char*)pDataRet,"没有钱包,请创立钱包");
					pDataRet += 64;
					*pDataRet++= 0x82;
					*pDataRet++= 0x01;
				}
			}
			break;
		case 0x60://初始化离线收款
		{
				int 	ret;
				UINT32	outLen;
				u8				*p;
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					SM2_PUBKEY *pPuKey=NULL;
					if(pAPDU->p1 == 1) //初始化收款（后续接收支付凭证）
					{
						DfRecviveInit *pInBack;
						DfRecviveResI *pOutResI;
						//----------检查钱包空间----------------
						pFd->index=FindVoucherNull(pFd->pRealizeInfo);
						if(pFd->index < 0)
						{//--------SW---------------------
							strcpy((char*)pDataRet,"已经到达最大上限,请同步");
							pDataRet += 64;
							*pDataRet++= 0x91;
							*pDataRet++= 0x03;
							break;
						}
						pInBack = (DfRecviveInit *)pAPDU->data;
						//------------解析验证证书-----------------
						{
							//-------运营机构交易证书-----------
							#ifdef WALLET_TEST_DATA
							pPuKey=(SM2_PUBKEY *)WALLET_TEST_DATA;
							#else
							u16 len;
							u8 *pCert =pInBack->CertChainB;
							if(*pCert == 0x30) pCert++;
							parse_asn1_len(len,pCert);
							if(len > 256)
							{
								*pDataRet++= 0x51;
								*pDataRet++= 0x01;
								break;
							}
							pCert += len;
							//-------硬件钱包交易证书----------
							if(*pCert == 0x30) pCert++;
							parse_asn1_len(len,pCert);
							if(len > 256)
							{
								*pDataRet++= 0x51;
								*pDataRet++= 0x02;
								break;
							}
							//06 08 2a 81 1c cf 55 01 82 2d
							pCert=eMemseek(pCert,len,(u8*)"\x06\x08\x2a\x81\x1c\xcf\x55\x01\x82\x2d",10);
							if(pCert)
							{
								pCert++ ;
								parse_asn1_len(len,pCert);
								if(len > 64)
								{
									pCert += len-64;
									pPuKey = (SM2_PUBKEY *)pCert;
								}
							}
							#endif
						}
						
						if(pPuKey == NULL) break;
						pFd->Amount=Conv_Hex4ToNum(pInBack->amount,sizeof(pInBack->amount));
						memcpy(pFd->TimeStamp,pInBack->TimeStamp,sizeof(pFd->TimeStamp));
						memcpy(pFd->RandB,pInBack->RandB,sizeof(pFd->RandB));
						
						pOutResI =(DfRecviveResI *)pDataRet;
						api_rand(pFd->RandA,sizeof(pFd->RandA)/4); //1*4 =4
						api_rand(pFd->FactA,sizeof(pFd->FactA)/4);	//4*4 =16
						
						Conv_NumToHex(pFd->pRealizeInfo->uATC,pOutResI->ATC,sizeof(pOutResI->ATC));
						Conv_NumToHex(pFd->pRealizeInfo->VoucherBal[0],pOutResI->wallAmount,sizeof(pOutResI->wallAmount));
						
						memcpy(pOutResI->RandA,pFd->RandA,sizeof(pOutResI->RandA));
						p = pOutResI->CertChainA;
						//------------------打包证书-----------------
						ret=ReadPubKeyCert(pFd->WalletID,CERT_INS,p,256);
						p += ret;
						ret=ReadPubKeyCert(pFd->WalletID,CERT_PERS,p,256);
						p += ret;
						//-------------FactA------------------------------------
						memcpy(p,pFd->FactA,sizeof(pFd->FactA));
						#ifdef WALLET_TEST_DATA
						outLen = sizeof(pFd->FactA);
						#else
						sm2_encrypt((u8*)pPuKey,(u8*)pFd->FactA,sizeof(pFd->FactA),p,&outLen);
						#endif
						p += outLen;
						//--------------Signature---------------------
						api_sm_sign2(pFd->pKey->d,pDataRet,p-pDataRet,p);
						p += 64;
						pDataRet = p;
					}
					else if(pAPDU->p1 == 2) //初始化支付（后续生成支付凭证）
					{
						DfPaymentInit *pInBack;
						DfPaymentResI *pOutResI;
						pInBack = (DfPaymentInit *)pAPDU->data;
						pFd->Amount=Conv_Hex4ToNum(pInBack->amount,sizeof(pInBack->amount));
						if(pFd->Amount > pFd->pRealizeInfo->uAvailableBal)
						{
							strcpy((char*)pDataRet,"没有足够的余额用于本次支付");
							pDataRet += 64;
							*pDataRet++= 0x81;
							*pDataRet++= 0x01;
							break;
						}
						pFd->index = CheckVoucherAmount(pFd->pRealizeInfo,pFd->Amount);
						if(pFd->index < 0)
						{
							strcpy((char*)pDataRet,"单个币不足以支付本次交易");
							pDataRet += 64;
							*pDataRet++= 0x81;
							*pDataRet++= 0x02;
							break;
						}
						memcpy(pFd->TimeStamp,pInBack->TimeStamp,sizeof(pInBack->TimeStamp));
						pOutResI =(DfPaymentResI *)pDataRet;
						api_rand(pFd->RandA,sizeof(pFd->RandA)/4); //1*4 =4
						memcpy(pOutResI->Random,pFd->RandA,sizeof(pOutResI->Random));
						pDataRet = pOutResI->CertChainB;
						ret=ReadPubKeyCert(pFd->WalletID,CERT_INS,pDataRet,256);
						if(ret > 0) pDataRet+= ret;
						ret=ReadPubKeyCert(pFd->WalletID,CERT_PERS,pDataRet,256);
						if(ret > 0) pDataRet+= ret;
					}
					*pDataRet++= 0x90;
					*pDataRet++= 0x00;
				}
				else
				{
					*pDataRet++= 0x51;
					*pDataRet++= 0x02;
				}
			};
			break;
		case 0x62://离线收款（OFFLINE RECEIVING）
			{
				u32		outLen;
				DfRecviveOperation	*pInBack;
				u8				*p;	//*pPaymentVoucher,
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					u8 skeyBuff[40];
					pInBack = (DfRecviveOperation *)pAPDU->data;
					//------------解析验证证书-----------------
					#if(FATAB_LEN == 16)
					memcpy(pFd->FactB,pInBack->FactB,sizeof(pFd->FactB));
					#else
					sm2_decrypt(pFd->pKey->d,pInBack->FactB,sizeof(pInBack->FactB),(u8*)pFd->FactB,&outLen);
					#endif
					p = skeyBuff;
					Conv_buffXor(p,(u8*)pFd->FactA,(u8*)pFd->FactB,sizeof(pFd->FactB));
					p += sizeof(pFd->FactB);
					strcpy((char*)p,"ENC&MAC"); p += 7;
					Conv_buffXor(p,(u8*)pFd->RandA,(u8*)pFd->RandB,sizeof(pFd->RandB));
					p += sizeof(pFd->RandB);
					api_md_hash(MODE_SM3,skeyBuff,p-skeyBuff,(u8*)&pFd->sesKey);	//得到会话密钥
					inlen -= sizeof(pInBack->FactB);
					if(inlen > 0)
					{
						u32 Amount;
						uBalVoucher *pBal;
						pBal=PaymentToBalVouche((PaymentVoucher *)&pInBack->PayVoucher,&outLen,&Amount);
						if(pBal == NULL)
						{
							*pDataRet++= 0x51;
							*pDataRet++= 0x02;
							break;
						}
						TRACE("outLen=%d,inlen=%d\n",outLen,inlen);
						//-------检查金额---------------------
						if(Amount != pFd->Amount)
						{
							*pDataRet++= 0x51;
							*pDataRet++= 0x03;
							break;
						}
						if((outLen+4) > inlen)
						{
							*pDataRet++= 0x51;
							*pDataRet++= 0x04;
							break;
						}
						p = (u8*)&pInBack->PayVoucher + outLen;
						//------------MAC1------------------------
						SaveBalVoucher(GetWalletName(pFd->index,pFd->WalletID),pBal);
						gFree(pBal);
						pFd->pRealizeInfo->VoucherBal[pFd->index] = Amount;
						pFd->pRealizeInfo->uBal += Amount;
						pFd->pRealizeInfo->uAvailableBal += Amount;
						pFd->pRealizeInfo->uOffTranAmt += Amount;

						pFd->pRealizeInfo->uDayPayAmt += Amount;
						pFd->pRealizeInfo->uMonthPayAmt += Amount;
						pFd->pRealizeInfo->uYearPayAmt += Amount;
						pFd->pRealizeInfo->uATC++;
						//----存交易流水------------
						{
							DfTransactionBill* pbill;
							pbill = (DfTransactionBill*)gMalloc(sizeof(DfTransactionBill)+256);
							Conv_NumToDecimal(Amount,pbill->tBill.Amount,sizeof(pbill->tBill.Amount));
							memcpy(pbill->tBill.TransactionType,pFd->TransType,sizeof(pbill->tBill.TransactionType));
							memcpy(pbill->tBill.TransactionType,pFd->TransType,sizeof(pbill->tBill.TransactionType));
							//SaveTransactionBill(GetWalletName(  ,pFd->WalletID),DfTransactionBill *pBill,int blen)
							gFree(pbill);
						}
						//------MAC2---------------------
						*pDataRet++= 0;
						*pDataRet++= 0;
						*pDataRet++= 0;
						*pDataRet++= 0;
						*pDataRet++= 0x90;	//SW1
						*pDataRet++= 0x00;  //SW2
						//gFree(pPaymentVoucher);
					}

				}
				else
				{
					*pDataRet++= 0x82;	//SW1
					*pDataRet++= 0x01;  //SW2
				}
			}
			break;
		case 0x64://离线支付
			{
				//int 	ret;
				DfPaymentCerI *pInBack;
				DfPaymentResO *pOutResI;
				u8				*p;
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					u16 len;
					u8* pCert;
					SM2_PUBKEY *pPuKey=NULL;
					
					pInBack = (DfPaymentCerI *)pAPDU->data;
					//------------解析验证证书-----------------
					pCert =pInBack->CertChainB;
					//-------运营机构交易证书-----------
					if(*pCert == 0x30) pCert++;
					parse_asn1_len(len,pCert);
				
					pCert += len;
					//-------硬件钱包交易证书----------
					if(*pCert == 0x30) pCert++;
					parse_asn1_len(len,pCert);
					//06 08 2a 81 1c cf 55 01 82 2d
					p=eMemseek(pCert,len,(u8*)"\x06\x08\x2a\x81\x1c\xcf\x55\x01\x82\x2d",10);
					if(p)
					{
						p++ ;
						parse_asn1_len(len,p);
						if(len > 64)
						{
							p += len-64;
							pPuKey = (SM2_PUBKEY *)p;
						}
					}
					pCert += len;
					#ifdef WALLET_TEST_DATA
					if(pPuKey==NULL)
						pPuKey=(SM2_PUBKEY *)WALLET_TEST_DATA;
					#endif
					//-------------FAT----------------------
					memcpy((u8*)pFd->FactB,pCert,sizeof(pFd->FactB));
					pCert += sizeof(pFd->FactB);
					//-----------sign[64]---------------------------
					TRACE("pPuKey[%x]\n",pPuKey);

					//-----------------------------------------------
					if(pPuKey)
					{
						int outLen;
						TransSubChain tTransSub;
						uBalVoucher *pBal;
						pBal=ReadBalVoucher(GetWalletName(pFd->index,pFd->WalletID));
						if(pBal == NULL)
						{
							strcpy((char*)pDataRet,"没有币串,请兑出数字币");
							pDataRet += 64;
							*pDataRet++= 0x82;
							*pDataRet++= 0x02;
							break;
						}
						pOutResI =(DfPaymentResO *)pDataRet;
						api_rand(pFd->FactA,sizeof(pFd->FactA)/4); //4*4 =16
						memcpy(pOutResI->FactA,pFd->FactA,sizeof(pOutResI->FactA));
						//-----------tTransSub-----------------------------
						tTransSub.aTag =0x51;
						tTransSub.aLen =sizeof(tTransSub.Amount);
						Conv_NumToDecimal(pFd->Amount,tTransSub.Amount,sizeof(tTransSub.Amount));
						
						tTransSub.tTag =0x52;
						tTransSub.tLen =sizeof(tTransSub.TransactionIndex);
						memcpy(tTransSub.TransactionIndex,pFd->TransIndex,sizeof(tTransSub.TransactionIndex));

						tTransSub.cTag =0x53;
						tTransSub.cLen =sizeof(tTransSub.CreditorPubKey);
						memcpy(tTransSub.CreditorPubKey,pPuKey,tTransSub.cLen);
						
						tTransSub.dTag =0x54;
						tTransSub.dLen =sizeof(tTransSub.DebtorSign);
						
						TransSubChainSign(pFd->pKey,pBal,&tTransSub);
						pFd->pRealizeInfo->uBal -= pFd->Amount;
						pFd->pRealizeInfo->uAvailableBal -= pFd->Amount;
						pFd->pRealizeInfo->VoucherBal[pFd->index] -= pFd->Amount;
						//-------------Return-------------------------------------
						outLen=ReadPaymentVoucher(pBal,&tTransSub,&pOutResI->payVou);
						gFree(pBal);
						pDataRet = (u8*)&pOutResI->payVou + outLen;
						//----------MAC1------------
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						//----------SW---------------
						*pDataRet++ = 0x90;
						*pDataRet++ = 0x00;
					}
					else
					{
						*pDataRet++= 0x90;
						*pDataRet++= 0x01;
					}
				}
				else
				{
					*pDataRet++= 0x51;
					*pDataRet++= 0x02;
				}
			}
			break;
		case 0x72://同步更新（DC SYNCHRONIZATION）
			{
				UINT32	Amount;
				DfRecviveResSyn *pOutRes;
				u8				*pOut;//,*pIndata;
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				pOutRes = (DfRecviveResSyn *)pDataRet;
				if(pFd)
				{
					u16 i,max;
					uBalVoucher* pBal;
					max = sizeof(pFd->pRealizeInfo->VoucherBal)/sizeof(pFd->pRealizeInfo->VoucherBal[0]);;
					Amount = 0;
					pOut = pOutRes->Voucher;
					
					for(i=0;i<max;i++)
					{
						if(pFd->pRealizeInfo->VoucherBal[i] == ~0) 
							continue;
						pBal=ReadBalVoucher(GetWalletName(i,pFd->WalletID));
						if(pBal)
						{
							u16 wLen,num;
							Amount += pFd->pRealizeInfo->VoucherBal[i];
							pOut = &pOutRes->Voucher[4];
							memcpy(pOut,&pBal->tDc,sizeof(DfDC_bt));
							pOut += sizeof(DfDC_bt);
							wLen = (pBal->SubChainNun *sizeof(DfTranSubChain));
							if(wLen > 0)
							{
								*pOut++ = 0x61; //DC交易链 TAG=0x60
								if(wLen > 255)
								{
									*pOut++ = 0x82;
									*pOut++ = wLen/0x100;
									*pOut++ = wLen&0xff;
								}
								else
								{
									*pOut++ = 0x81;
									*pOut++ = wLen;
								}
								for(num = 0; num < pBal->SubChainNun;num++)
								{
									memcpy(pOut,&pBal->tranChain[num],sizeof(DfTranSubChain));
									pOut += sizeof(DfTranSubChain);
								}
							}
							gFree(pBal);
							*pOut++ = 0x5A;	//凭证余额
							*pOut++ = 6;	//len
							Conv_NumToHex(pFd->pRealizeInfo->VoucherBal[i],pOut,6);
							pOut += 6;
							//----------------------------------------------------
							wLen = pOut- &pOutRes->Voucher[4];
							pOutRes->Voucher[0] = 0x61; //DC余额凭证 TAG=0x61
							pOutRes->Voucher[1] = 0x82;
							pOutRes->Voucher[2] = wLen/0x100;
							pOutRes->Voucher[3] = wLen&0xff;
						}
					}
					if(pOut > pOutRes->Voucher)
					{
						pDataRet = pOut;
						//Conv_NumToHex(Amount,pOutRes->amount,sizeof(pOutRes->amount));
						//----------Mac3------------
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						*pDataRet++ = 0x00;
						//----------SW---------------
						*pDataRet++ = 0x90;
						*pDataRet++ = 0x00;
					}
					else
					{
						strcpy((char*)pDataRet,"没有币串,请兑出数字币");
						pDataRet += 64;
						*pDataRet++= 0x82;
						*pDataRet++= 0x02;
						break;
					}
				}
				else
				{
					//----------SW---------------
					*pDataRet++ = 0x82;
					*pDataRet++ = 0x01;
				}
			}
			break;
		case 0x74://同步更新完成（FINISH DC SYNCHRONIZATION）
			{
				uWalletFD* pFd;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					u16 i,max;
					uBalVoucher *pBal;
					pBal =(uBalVoucher*)pAPDU->data;
					if(InDcVerify(&pBal->tDc) == 0)
					{
						max = sizeof(pFd->pRealizeInfo->VoucherBal)/sizeof(pFd->pRealizeInfo->VoucherBal[0]);
						for(i=0;i<max;i++)
						{
							if(pFd->pRealizeInfo->VoucherBal[i] == ~0)
								continue;
							API_fremove(GetWalletName(i,pFd->WalletID));
							pFd->pRealizeInfo->VoucherBal[i]= ~0;
						}
						pFd->pRealizeInfo->VoucherBal[0]= Conv_Hex4ToNum(pBal->tDc.dc.Denomination,sizeof(pBal->tDc.dc.Denomination));
						pBal->SubChainNun = 0;	//没有子链
						SaveBalVoucher(GetWalletName(0,pFd->WalletID),pBal);

						pFd->pRealizeInfo->uBal = pFd->pRealizeInfo->VoucherBal[0];
						pFd->pRealizeInfo->uAvailableBal = pFd->pRealizeInfo->uBal;
						pFd->pRealizeInfo->uInTransmitBal = 0;
						pFd->pRealizeInfo->uFreezeBal = 0;
						pFd->pRealizeInfo->OffTranTimes = 0;
						pFd->pRealizeInfo->uOffTranAmt = 0;
						APP_FileSaveCont(GetWalletName(WAL_REAL,pFd->WalletID),(u32*)pFd->pRealizeInfo,sizeof(uSystemRealizeInfo));
						//memcpy(pFd->pRealizeInfo->LastUpdateTime,pRecvDc->DateTime,sizeof(pFd->pRealizeInfo->LastUpdateTime));
					}
					else
					{
						//---------Transaction Result-----------
						strcpy((char*)pDataRet,"币串验证出错");
						pDataRet += 64;
						//----------SW---------------
						*pDataRet++ = 0x80;
						*pDataRet++ = 0x05;
						break;
					}
					//---------Transaction Result-----------
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					//----------Mac5------------
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					*pDataRet++ = 0x00;
					//----------SW---------------
					*pDataRet++ = 0x90;
					*pDataRet++ = 0x00;
				}
				else
				{
					//----------SW---------------
					*pDataRet++ = 0x82;
					*pDataRet++ = 0x01;
				}
			}
			break;
		case 0x40://查看金额
			{
				uWalletFD* pFd;
				//DfCommOutPack *pRet;
				pFd=gGetWalletPfd();
				if(pFd)
				{
					//TRACE("pFd->pRealizeInfo->uBal=%d\r\n",pFd->pRealizeInfo->uBal);
					Conv_NumToDecimal(pFd->pRealizeInfo->uBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uAvailableBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uInTransmitBal,pDataRet,9);
					pDataRet += 9;
					Conv_NumToDecimal(pFd->pRealizeInfo->uFreezeBal,pDataRet,9);
					pDataRet += 9;
					//---------------钱包名称----------------
					memcpy(pDataRet,pFd->pWallet->Info.WalletName,sizeof(pFd->pWallet->Info.WalletName));
					pDataRet += sizeof(pFd->pWallet->Info.WalletName);
					//----------SW---------------
					*pDataRet++ = 0x90;
					*pDataRet++ = 0x00;
				}
				else
				{
					//----------SW---------------
					*pDataRet++ = 0x82;
					*pDataRet++ = 0x01;
				}
			}
			break;
		#ifdef WALLET_TEST_DATA
		case 0x30:
			{
				//uWalletFD* pFd;
				u32 Amont;
				int ret;
				Amont=Conv_Hex4ToNum(pAPDU->data,inlen);
				ret=Test_CreateNewDc(gGetWalletPfd(),Amont);
				if(ret == 0)
				{
					*pDataRet++ = 0x90;
					*pDataRet++ = 0x00;
				}
				else
				{
					//----------SW---------------
					*pDataRet++ = 0x82;
					*pDataRet++ = 0x01;
				}
			}
			break;
		#endif
		}
	return pDataRet-pOut;
}

void HardWalletPross(void)
{
	u8 *pUartData,*pDataRet,*pRecvData;
	u32 ReLen;
	pUartData=gMalloc(2048);
	while(1)
	{
		ReLen = 2048;
		pDataRet=pUartData+4;
		pRecvData=(u8*)API_Uart_PackRecv(pUartData,&ReLen);
		if(pRecvData != NULL)	//CLA0x80
		{
			ReLen=WalletDataParseApdu((DC_APDU *)pRecvData,pDataRet);
			API_Uart_PackSend(pDataRet,ReLen);
		}
		else
		{
			*pDataRet++ = 'E';
			*pDataRet++ = 'R';
			*pDataRet++ = 'R';
			API_Uart_PackSend(pUartData+4,pDataRet-(pUartData+4));
		}
	}
}



