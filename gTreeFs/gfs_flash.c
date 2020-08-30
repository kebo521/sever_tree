
#include "gfs_api.h"

#ifdef GTREE_FILE_COURCE

//#include "MercurySpi.h"
//#include "MercuryThread.h"
//#include "MercuryTimer.h"
//#include "MercuryDef.h"

#ifdef   __cplusplus
extern   "C"
{
#endif

#ifndef LOCAL
#define LOCAL static
#endif

//typedef uint32 BSD_STATUS;


#define SPIFLASH_LOG 1
#define TEST_CODE 0


//common command
#define CMD_READ            0x03
#define CMD_FAST_READ       0x0B    //QPI 4READ
#define CMD_4READ           0xEB    //SPI 4READ/QPI 4READ
#define CMD_WREN            0x06
#define CMD_WRDI            0x04
#define CMD_RDSR1           0x05
#define CMD_RDSR2           0x35
#define CMD_WRSR            0x01
#define CMD_4PP             0x32
#define CMD_SE              0x20
#define CMD_32KBE           0x52
#define CMD_64KBE           0xD8
#define CMD_CE              0xC7    //or 0x60 C7
#define CMD_PP              0x02
#define CMD_RDID            0x9F
#define CMD_RSTEN           0x66
#define CMD_RST             0x99
#define CMD_SETBURST        0xC0
#define CMD_DP              0xB9
#define CMD_RDP             0xAB
#define CMD_WBPR            0x42
#define CMD_RDVECR          0x65
#define CMD_WRVECR          0x61
#define CMD_RFSR            0x70
#define CMD_CLFSR           0x50
#define CMD_RDVCR           0x85
#define CMD_WRVCR           0x81
#define CMD_RDCR            0x15
#define CMD_4BYTEADDREN     0xB7
#define CMD_4BYTEADDRDI     0xE9
#define CMD_QUAD_FAST_READ  0x6B

#define WEL (0x1<<1)
#define WIP 0x01

#define SPIFLASH_LOGICID 1

#ifdef WINDOW_FLASH_SOFT


static u32* pFlashStartAddr = NULL;

int SPI_Flash_init(void)
{
	int ret=-1;
	if (pFlashStartAddr==NULL)
	{
		int OutLen;
		pFlashStartAddr=(u32*)APP_fileRead("gFsData.bin",&OutLen);
		if(pFlashStartAddr==NULL)
		{
			TRACE("Flash_init->pFlashStartAddr Is NULL\r\n");
			pFlashStartAddr = (u32*)malloc(TEST_FLASH_SIZE);
			memset_u32(pFlashStartAddr,0xffffffff,TEST_FLASH_SIZE/sizeof(u32));
			TRACE("Flash_init->set FF[] Ok\r\n",TEST_FLASH_SIZE);
			ret= 1;
		}
		else 
		{
			ret= 0;
		}
	}
	//TRACE_HEX("FLASH_4M",pFlashStartAddr+(4*256)*4096,128);
	return ret;
}

int SPI_Flash_Deinit(void)
{
	if (pFlashStartAddr)
	{
		APP_fileSave("gFsData.bin",pFlashStartAddr,TEST_FLASH_SIZE);
		free(pFlashStartAddr);
		pFlashStartAddr=NULL;
	}
	TRACE("Flash_Deinit ->gFsData.bin");
	return 0;
}


int SPI_Flash_Exist(void)
{
	if (pFlashStartAddr == NULL) return -1;
	return 0;
}

int SPI_Flash_WriteEnable(int timeOutMs)
{
	return TRUE;
}

int SPI_Flash_WriteComplete(int timeOutMs)
{
	return TRUE;
}

int SPI_Flash_WriteDisable(int timeOutMs)
{
	return TRUE;
}

int SPI_Write(int index, u32* buff, int len)
{
	u32 addr;
	u8* pbuff=(u8*)buff;
	addr = pbuff[1] * 0x10000 + pbuff[2] * 0x100 + pbuff[3];
	if(addr & 0x03)
	{
		TRACE("##SPI Write->addr[%X] ERR",pbuff);
		return -6;
	}
	if (pbuff[0] == CMD_SE)
	{
		addr -= TEST_FLASH_START;
		addr = addr & (~(SPI_SECTOR_SIZE - 1));
		addr >>= 2;
		memset_u32((pFlashStartAddr + addr), 0xffffffff, SPI_SECTOR_SIZE/sizeof(u32));
	}
	else if (pbuff[0] == CMD_CE)
	{
		memset_u32(pFlashStartAddr, 0xffffffff, TEST_FLASH_SIZE/sizeof(u32));
	}
	else if(pbuff[0] == CMD_PP)
	{
		int i;
		if((u32)buff & 0x03)
		{
			TRACE("##SPI Write->buff[%X] ERR",buff);
			return -6;
		}
		if(len & 0x03)
		{
			TRACE("##SPI Write->len[%X] ERR",len);
			return -6;
		}
		len = (len-4)>>2;
		addr -= TEST_FLASH_START;
		addr >>= 2;
		buff++;
		for(i=0; i<len; i++)
		{
			pFlashStartAddr[addr+i] &= buff[i];
		}
		return (i<<2);
	}
	return 0;
}

int SPI_Read(int index, u32* buff, int size, char* cmd, int cLen)
{
	if (cmd[0] == CMD_READ)
	{
		int i;
		u32 addr;
		addr = (u8)cmd[1] * 0x10000 + (u8)cmd[2] * 0x100 + (u8)cmd[3];
		if(addr & 0x03)
		{
			TRACE("##SPI Read->addr[%X] ERR",buff);
			return -6;
		}
		if((u32)buff & 0x03)
		{
			TRACE("##SPI Read->buff[%X] ERR",buff);
			return -6;
		}
		if(size & 0x03)
		{
			TRACE("##SPI Read->size[%X] ERR",size);
			return -6;
		}
		addr -= TEST_FLASH_START;
		addr >>= 2;
		size >>= 2;
		for(i=0; i<size; i++)
		{
			buff[i] = pFlashStartAddr[addr + i];
		}
		return (i<<2);
	}
	return size;
}

#else

int SPI_Flash_init(void)
{
	SPI_CONFIG_S cfg;
	int i;

	cfg.freq = 20000000;
	cfg.mode = CPOL0_CPHA0;
	cfg.tx_bit_length = 8;

	for (i = 0; i<3; i++)
	{
		if (0 == SPI_Init(SPIFLASH_LOGICID, &cfg))
		{
			return 0;
		}
	}
	return -1;
}

int SPI_Flash_Exist(void)
{
	char cmd[4];
	u32  read = 0xffffffff;
	int i;
	cmd[0] = 0x9f;
	for (i = 0; i<5; i++)
	{
		SPI_Read(SPIFLASH_LOGICID,(u8*)&read, 3, cmd, 1);
		if (read == 0xffffffff)
		{
			Sleep(50);
			continue;
		}
		else
		{
			return 0;
		}
	}
	return -1;
}

int SPI_Flash_WriteEnable(int timeOutMs)
{
	u8 data[4];
	data[0] = CMD_WREN;
	data[1] = CMD_RDSR1;
	data[2] = 0;
	while (1)
	{
		SPI_Write(SPIFLASH_LOGICID, data, 1);
		SPI_Read(SPIFLASH_LOGICID, data + 2, 1, (char*)data + 1, 1);
		if (data[2] & (WEL))
			return TRUE;

		Sleep(40);
		timeOutMs -= 50;	//执行一次大概50s
		if (timeOutMs < 0)
		{
			TRACE("\r\n****Error:spiflash WriteEnable Timeout*****\r\n");
			return FALSE;
		}
	}
}


int SPI_Flash_WriteComplete(int timeOutMs)
{
	u8 data[4];
	data[0] = CMD_RDSR1;
	data[1] = 0;
	while (1)
	{
		SPI_Read(SPIFLASH_LOGICID, data + 1, 1, (char*)data, 1);
		if (!(data[1] & (WIP)))
			return TRUE;

		Sleep(40);
		timeOutMs -= 50;	//执行一次大概50s
		if (timeOutMs < 0)
		{
			TRACE("\r\n****Error:spiflash WriteComplete Timeout*****\r\n");
			return FALSE;
		}
	}
}

int SPI_Flash_WriteDisable(int timeOutMs)
{
	u8 data[4];
	data[0] = CMD_WRDI;
	data[1] = CMD_RDSR1;
	data[2] = 0;
	while (1)
	{
		SPI_Write(SPIFLASH_LOGICID, data, 1);
		SPI_Read(SPIFLASH_LOGICID, data + 2, 1, (char*)data + 1, 1);
		if (!(data[2] & (WEL)))
			return TRUE;

		Sleep(40);
		timeOutMs -= 50;	//执行一次大概50s
		if (timeOutMs < 0)
		{
			TRACE("\r\n****Error:spiflash WriteDisable Timeout*****\r\n");
			return FALSE;
		}
	}
}

#endif

int SPI_Flash_ERASE_One(u32 addr)
{
	u8	data[4];
	data[0] = CMD_SE;
	data[1] = (addr/0x10000) & 0xff;
	data[2] = (addr/0x100) & 0xff;
	data[3] = addr & 0xff;
	return SPI_Write(SPIFLASH_LOGICID, (u32*)data, 4);
}

int SPI_Flash_EraseOne(u32 addr)
{
	if(addr < TEST_FLASH_START || (addr >= TEST_FLASH_END))
    {
    	TRACE("###Flash_ERASE_SECTOR[%X] Err\r\n",addr);
        return -1;
    }	
	//TRACE("--SPI Flash EraseOne[0x%X]->\r\n",block);
	if (!SPI_Flash_WriteEnable(5000)) {
		TRACE("**SPI Flash OneSector[%x] Err1\r\n", addr);
		return -1;
	}
	
	SPI_Flash_ERASE_One(addr);
	
    if(!SPI_Flash_WriteComplete(5000)){
		TRACE("**SPI Flash OneSector[%x] Err2\r\n", addr);
		return -2;
	}
    if(!SPI_Flash_WriteDisable(5000)){
		TRACE("**SPI Flash OneSector[%x] Err3\r\n", addr);
		return -3;
	}    
	return 0;
}


int SPI_Flash_Read(u32 addr, void* buff, int size)
{
	u8	cmd[4];
	if(addr & 0x03)
	{
		TRACE("##SPI_Flash_Read->addr[%X] ERR",buff);
		return -6;
	}
	if((u32)buff & 0x03)
	{
		TRACE("##SPI_Flash_Read->buff[%X] ERR",buff);
		return -6;
	}
	if(size & 0x03)
	{
		TRACE("##SPI_Flash_Read->size[%X] ERR",size);
		return -6;
	}
		
	if(addr < TEST_FLASH_START || (addr >= TEST_FLASH_END) || (NULL == buff) || (size <=0))
    {
    	TRACE("###Flash_Read[%X][%d] Err\r\n",addr,size);
        return -1;
    }	
	cmd[0] = CMD_READ;
	cmd[1] = (addr/0x10000) & 0xff;
	cmd[2] = (addr/0x100) & 0xff;
	cmd[3] = addr & 0xff;
	return SPI_Read(SPIFLASH_LOGICID, buff, size, (char*)cmd, 4);
}

int SPI_Flash_Write(u32 addr, void* buff, int Len)
{
	u32 pageAddr;
	u8 data[4];
	u32 cmd[(4+SPI_PAGE_SIZE)/4],*pSave;
	int offset;
	u16 firstlast,MaxLen;
	if(addr & 0x03)
	{
		TRACE("##SPI_Flash_Write->addr[%X] ERR",buff);
		return -6;
	}
	if((u32)buff & 0x03)
	{
		TRACE("##SPI_Flash_Write->buff[%X] ERR",buff);
		return -6;
	}
	if(Len & 0x03)
	{
		TRACE("##SPI_Flash_Write->Len[%X] ERR",Len);
		return -6;
	}
	if (addr < TEST_FLASH_START || (addr >= TEST_FLASH_END) || (Len == 0))
    {
    	TRACE("###Flash_Write[%X] [%d]Err\r\n",addr,Len);
        return -1;
    }	
	pSave = (u32*)buff;
	firstlast = addr&(SPI_PAGE_SIZE-1);
	pageAddr = addr&(~(SPI_PAGE_SIZE-1));
	offset=0;
	while(offset < Len)
	{
		data[0] = CMD_PP;
		data[1] = (pageAddr/0x10000) & 0xff;
		data[2] = (pageAddr/0x100) & 0xff;
		data[3] = pageAddr & 0xff;
		memcpy(cmd,data,4);
		if(firstlast)
		{//-------第一次------------
			MaxLen=SPI_PAGE_SIZE-firstlast;
			memset_u32(cmd+1,0xffffffff,SPI_PAGE_SIZE/sizeof(u32));
			if(MaxLen > Len) MaxLen = Len;
			memcpy_u32(cmd+1+(firstlast>>2),pSave, MaxLen>>2);
			firstlast=0;
		}
		else
		{
			MaxLen=SPI_PAGE_SIZE;
			if(MaxLen > (Len-offset))
			{
				MaxLen = (Len-offset);
				memcpy_u32(cmd+1,pSave+(offset>>2),MaxLen>>2);
				memset_u32(cmd+1+(MaxLen>>2),0xffffffff,SPI_PAGE_SIZE-(MaxLen>>2));
			}
			else
			{
				memcpy_u32(cmd+1, pSave+(offset>>2),MaxLen>>2);
			}
		}
		//---------------------------------------------------------------
		if(offset)	//第一次不用判断
		{//-----放在write前面有利于节约主程序时间-------
			if (!SPI_Flash_WriteComplete(5000)) {
				TRACE("**SPI Flash Write[%X] Complete Err\r\n",addr);
				return -1;
			}
		}
		if (!SPI_Flash_WriteEnable(5000)) {
			TRACE("**SPI Flash Write[%X] Enable Err\r\n",addr);
			return -1;
		}
		SPI_Write(SPIFLASH_LOGICID, (u32*)cmd, 4+SPI_PAGE_SIZE);

		pageAddr += SPI_PAGE_SIZE;
		offset += MaxLen;
	}
	SPI_Flash_WriteComplete(5000);
	SPI_Flash_WriteDisable(3000);
	return offset;
}




#endif


