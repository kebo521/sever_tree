

#ifndef GFS_SPIFLAG_BSD
#define GFS_SPIFLAG_BSD


#define     SPI_PAGE_SIZE           4	//256
#define 	SPI_SECTOR_SIZE 		512	//4096
#define 	SPI_BLOCK_SIZE 			(SPI_SECTOR_SIZE*16)

//----------------------------------------------------------------
#define TEST_FLASH_START 	(0x00400000)
#define TEST_FLASH_END 		(TEST_FLASH_START + 0x3F000)

#define TEST_FLASH_SIZE 	(TEST_FLASH_END - TEST_FLASH_START)

extern int SPI_Flash_init(void);
extern int SPI_Flash_Deinit(void);
extern int SPI_Flash_Exist(void);

extern int SPI_Flash_EraseOne(u32 addr);

extern int SPI_Flash_Read(u32 addr, void* pbuff, int size);
extern int SPI_Flash_Write(u32 addr,void* pbuff, int Len);




#endif

