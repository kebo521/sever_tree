

#ifndef GFS_SPIFLAG_BSD
#define GFS_SPIFLAG_BSD


#define GFS_BASIC_NULL		0xFFFF	//����
#define GFS_BASIC_GUID		0xEEEE	//����
#define GFS_BASIC_DATA		0xDDDD	//����
#define GFS_BASIC_BAD		0x4884	//����
//----------�������ݿ鶨��-����ÿ���鶨��Ϊ�˱�׼---------
typedef struct
{
	u16	head;	//��ͷ:[FFFF] �Ѿ������[48||84] ���飬[EEEE] �������ݣ�[DDDD] ʹ������
	u16	pageBit;//ҳʹ�ñ�� ÿһҳ256byte
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

