#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h"

/***************************************************************************************
* Define
****************************************************************************************/
#define W25Q16 	                0XEF14
#define SPI_FLASH_TYPE	        W25Q16
#define FLASH_ID                0XEF14

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F

#define W25X_SPI_GET_DATA_CMD   0xFF

#define W25X_FLASH_PAGE_SIZE    256
#define W25X_FLASH_SECTOR_SIZE  4096
#define W25X_FLASH_BLOCK_SIZE   (W25X_FLASH_SECTOR_SIZE*16)

#define	SPI_FLASH_CS            PAout(2)  

/***************************************************************************************
* Data
***************************************************************************************/
extern u8 G_u8_SPIFlashBuf[W25X_FLASH_SECTOR_SIZE];

/***************************************************************************************
* Function prototypes
****************************************************************************************/
void p_dr_SPIFlashInit(void);
u16  p_dr_SPIFlashReadID(void);  	    
u8	 p_dr_SPIFlashReadSR(void);        
void p_dr_SPIFlashWriteSR(u8 sr);  	
void p_dr_SPIFlashWriteEnable(void);  
void p_dr_SPIFlashWriteDisable(void);	
void p_dr_SPIFlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead); 
void p_dr_SPIFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void p_dr_SPIFlashEraseChip(void);    	 
void p_dr_SPIFlashEraseSector(u32 Dst_Addr);
void p_dr_SPIFlashWaitBusy(void);          
void p_dr_SPIFlashPowerDown(void);       
void p_dr_SPIFlashWakeUp(void);		
#endif

