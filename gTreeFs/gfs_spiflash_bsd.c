
#include "communal.h"

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

#define SPI_SECTOR_SIZE 4096
#define SPI_SECTORS_PER_BLOCK 8   
#define SPI_SECTOR_NUM 1024 
#define SPI_BLOCK_NUM 128
#define SPI_PAGE_SIZE 256
#define SPI_PAGE_SHIFT 8
#define SPI_PAGE_NUM_PER_SECTOR (SPI_SECTOR_SIZE >> SPI_PAGE_SHIFT) //16

#define SPIFLASH_LOG 1
#define TEST_CODE 0
#define READ_REG_TIMEOUT 2000
#define MAX_READREG_TIME 10000   //MS 10 set 60s


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

static u8* pFlashStartAddr = NULL;

int SPI_Flash_init(void)
{
	int ret=-1;
	if (pFlashStartAddr==NULL)
	{
		int OutLen;
		pFlashStartAddr=(u8*)APP_fileRead("gFsData.bin",&OutLen);
		if(pFlashStartAddr==NULL)
		{
			TRACE("Flash_init->pFlashStartAddr Is NULL\r\n");
			pFlashStartAddr = (u8*)malloc(8 * 256 * 4096);
			memset_u32((u32*)pFlashStartAddr,0xffffffff, 8 * 256 * 4096/sizeof(u32));
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
		APP_fileSave("gFsData.bin",pFlashStartAddr,(8 * 256 * 4096));
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

int SPI_Write(int index, BYTE* buff, int len)
{
	u32 addr;
	addr = buff[1] * 0x10000 + buff[2] * 0x100 + buff[3];
	if (buff[0] == CMD_SE)
	{
		addr = addr & (~(SPI_SECTOR_SIZE - 1));
		memset_u32((u32*)(pFlashStartAddr + addr), 0xffffffff, SPI_SECTOR_SIZE/sizeof(u32));
	}
	else if (buff[0] == CMD_CE)
	{
		memset_u32((u32*)pFlashStartAddr, 0xffffffff, 8 * 256 * 4096/sizeof(u32));
	}
	else if(buff[0] == CMD_PP)
	{
		int i;
		//TRACE("SPI_Write->addr[%x],len[%d],max[%d]",addr,len,max);
		//TRACE_HEX("SPI_Write->buff",pBuff,max);
		//TRACE_HEX("SPI_Write->pFlashAddr",pFlashAddr,(len-4));
		for(i=0; i<(len-4); i++)
		{
			//if(i != 0 && ((addr+i)%SPI_PAGE_SIZE)==0) break;
			pFlashStartAddr[addr+i] &= buff[4+i];
		}
		//TRACE_HEX("SPI_Write->iFlashAddr",pFlashAddr,i);
		return i;
	}
	return 0;
}

int SPI_Read(int index, BYTE* buff, int size, char* cmd, int cLen)
{
	if (cmd[0] == CMD_READ)
	{
		u32 addr;
		addr = (u8)cmd[1] * 0x10000 + (u8)cmd[2] * 0x100 + (u8)cmd[3];
	//	TRACE("SPI_Read addr[%X]",addr);
	//	TRACE_HEX("SPI_Read cmd[%X]",cmd,cLen);
	//	TRACE_HEX("SPI_Read pFlashAddr",pFlashAddr,size);
		memcpy_u8(buff, pFlashStartAddr + addr, size);
	//	TRACE_HEX("SPI_Read buff",buff,size);
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
		SPI_Read(SPIFLASH_LOGICID, (BYTE*)&read, 3, cmd, 1);
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
	BYTE data[4];
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
	BYTE data[4];
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
	BYTE data[4];
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

int SPI_Flash_ERASE(u32 addr)
{
	BYTE	data[4];
	data[0] = CMD_SE;
	data[1] = (addr/0x10000) & 0xff;
	data[2] = (addr/0x100) & 0xff;
	data[3] = addr & 0xff;
	return SPI_Write(SPIFLASH_LOGICID, data, 4);
}

int SPI_Flash_OneSector(u32 block)
{
	//TRACE("--SPI Flash EraseOne[0x%X]->\r\n",block);
	if (!SPI_Flash_WriteEnable(5000)) {
		TRACE("**SPI Flash OneSector[%d] Err1\r\n",block);
		return -1;
	}
	
	SPI_Flash_ERASE(block*GFS_BLOCK_SIZE);
	
    if(!SPI_Flash_WriteComplete(5000)){
		TRACE("**SPI Flash OneSector[%d] Err2\r\n",block);
		return -2;
	}
    if(!SPI_Flash_WriteDisable(5000)){
		TRACE("**SPI Flash OneSector[%d] Err3\r\n",block);
		return -3;
	}    
	return 0;
}


int SPI_Flash_Read(u32 addr, void* buff, int size)
{
	BYTE	cmd[4];
	cmd[0] = CMD_READ;
	cmd[1] = (addr/0x10000) & 0xff;
	cmd[2] = (addr/0x100) & 0xff;
	cmd[3] = addr & 0xff;
	return SPI_Read(SPIFLASH_LOGICID, buff, size, (char*)cmd, 4);
}

int SPI_Flash_Write(u32 addr, void* buff, int Len)
{
	BYTE data[4+SPI_PAGE_SIZE];
	u32 pageAddr;
	int offset;
	u16 firstlast,MaxLen;
	if(Len == 0) return 0;
	
	firstlast = addr&(SPI_PAGE_SIZE-1);
	pageAddr = addr&(~(SPI_PAGE_SIZE-1));
	offset=0;
	while(offset < Len)
	{
		data[0] = CMD_PP;
		data[1] = (pageAddr/0x10000) & 0xff;
		data[2] = (pageAddr/0x100) & 0xff;
		data[3] = pageAddr & 0xff;
		if(firstlast)
		{//-------第一次------------
			MaxLen=SPI_PAGE_SIZE-firstlast;
			memset_u8(data+4,0xff,SPI_PAGE_SIZE);
			if(MaxLen > Len) MaxLen = Len;
			memcpy_u8(data+4+firstlast,buff, MaxLen);
			firstlast=0;
		}
		else
		{
			MaxLen=SPI_PAGE_SIZE;
			if(MaxLen > (Len-offset))
			{
				MaxLen = (Len-offset);
				memcpy_u8(data+4, (BYTE*)buff+offset, MaxLen);
				memset_u8(data+4+MaxLen,0xff,SPI_PAGE_SIZE-MaxLen);	//多余部分补0xff
			}
			else
			{
				memcpy_u8(data+4, (BYTE*)buff+offset, MaxLen);
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
		SPI_Write(SPIFLASH_LOGICID, data, 4+SPI_PAGE_SIZE);

		pageAddr += SPI_PAGE_SIZE;
		offset += MaxLen;
	}
	SPI_Flash_WriteComplete(5000);
	SPI_Flash_WriteDisable(3000);
	return offset;
}

int SPI_Flash_ChipERASE(void)
{
	BYTE data[2] = { 0 };
	if (!SPI_Flash_WriteEnable(5000)) {
		TRACE("**SPI Flash ChipERASE Enable Err\r\n");
		return -1;
	}
	data[0] = CMD_CE;
	SPI_Write(SPIFLASH_LOGICID, data, 1);

	if (!SPI_Flash_WriteComplete(30*1000)) {
		TRACE("**SPI Flash ChipERASE Complete No\r\n");
		return -1;
	}

	if (!SPI_Flash_WriteDisable(3000)) {
		TRACE("**SPI Flash ChipERASE Complete No\r\n");
		return -1;
	}
	return 0;
}







#endif

