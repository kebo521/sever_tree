

#ifndef GFS_SPIFLAG_BSD
#define GFS_SPIFLAG_BSD


#define GFS_BASIC_NULL		0xFFFF	//无数
#define GFS_BASIC_GUID		0xEEEE	//引导
#define GFS_BASIC_DATA		0xDDDD	//数据
#define GFS_BASIC_BAD		0x4884	//坏块
//----------基础数据块定义-对于每个块定义为此标准---------
typedef struct
{
	u16	head;	//块头:[FFFF] 已经察除，[48||84] 坏块，[EEEE] 引导数据，[DDDD] 使用数据
	u16	pageBit;//页使用标记 每一页256byte
}DfBasicBlockHead;



extern int SPI_Flash_init(void);
extern int SPI_Flash_Deinit(void);

extern int SPI_Flash_Exist(void);
extern int SPI_Flash_WriteEnable(int timeOutMs);
extern int SPI_Flash_WriteComplete(int timeOutMs);
extern int SPI_Flash_WriteDisable(int timeOutMs);
extern int SPI_Flash_ERASE(u32 addr);
extern int SPI_Flash_OneSector(u32 block);
extern int SPI_Flash_Read(u32 addr, void* buff, int size);
extern int SPI_Flash_Write(u32 addr, void* buff, int Len);
extern int SPI_Flash_ChipERASE(void);




#endif

