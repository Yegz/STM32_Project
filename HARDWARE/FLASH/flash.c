/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   15-May-2016         Git     Optimize
 *
 **/
 
#include "flash.h" 
#include "spi.h"
#include "delay.h"   

/**************************************************************************************
* Data
**************************************************************************************/
u8 G_u8_SPIFlashBuf[W25X_FLASH_SECTOR_SIZE];

/**************************************************************************************
* Function Implementation
**************************************************************************************/
void p_dr_SPIFlashInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //SPI CS
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);

	p_dr_SPIInit();		  
}  

u8 p_dr_SPIFlashReadSR(void)   
{  
	u8 byte=0;
    
	SPI_FLASH_CS = 0;                         
	p_dr_SPIReadWriteByte(W25X_ReadStatusReg);   
	byte = p_dr_SPIReadWriteByte(W25X_SPI_GET_DATA_CMD);            
	SPI_FLASH_CS = 1; 
    
	return byte;   
} 

void p_dr_SPIFlashWriteSR(u8 sr)   
{   
	SPI_FLASH_CS=0;                           
	p_dr_SPIReadWriteByte(W25X_WriteStatusReg);   
	p_dr_SPIReadWriteByte(sr);              
	SPI_FLASH_CS=1;                           
}   
  
void p_dr_SPIFlashWriteEnable(void)   
{
	SPI_FLASH_CS=0;                            
    p_dr_SPIReadWriteByte(W25X_WriteEnable);     
	SPI_FLASH_CS=1;                           
} 
  
void p_dr_SPIFlashWriteDisable(void)   
{  
	SPI_FLASH_CS=0;                          
    p_dr_SPIReadWriteByte(W25X_WriteDisable);     
	SPI_FLASH_CS=1;                         
} 			    

u16 p_dr_SPIFlashReadID(void)
{
	u16 Temp = 0;
    
	SPI_FLASH_CS=0;				    
	p_dr_SPIReadWriteByte(0x90);
	p_dr_SPIReadWriteByte(0x00); 	    
	p_dr_SPIReadWriteByte(0x00); 	    
	p_dr_SPIReadWriteByte(0x00); 	 			   
	Temp |= p_dr_SPIReadWriteByte(0xFF)<<8;  
	Temp |= p_dr_SPIReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;	
    
	return Temp;
}   		    

void p_dr_SPIFlashRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)   
{ 
 	u16 i;    
    
	SPI_FLASH_CS=0;                            
    p_dr_SPIReadWriteByte(W25X_ReadData);        
    p_dr_SPIReadWriteByte((u8)((ReadAddr)>>16));  
    p_dr_SPIReadWriteByte((u8)((ReadAddr)>>8));   
    p_dr_SPIReadWriteByte((u8)ReadAddr);
    
    for(i=0; i<NumByteToRead; i++)
	{ 
        pBuffer[i] = p_dr_SPIReadWriteByte(0XFF);   
    }
	SPI_FLASH_CS=1;                         
}  

void SPI_Flash_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
 	u16 i;  
    
    p_dr_SPIFlashWriteEnable();  
    
	SPI_FLASH_CS=0;                         
    p_dr_SPIReadWriteByte(W25X_PageProgram);   
    p_dr_SPIReadWriteByte((u8)((WriteAddr)>>16)); 
    p_dr_SPIReadWriteByte((u8)((WriteAddr)>>8));   
    p_dr_SPIReadWriteByte((u8)WriteAddr);
    
    for(i=0;i<NumByteToWrite;i++)
        p_dr_SPIReadWriteByte(pBuffer[i]);
	SPI_FLASH_CS=1;    
    
	p_dr_SPIFlashWaitBusy();					  
} 

void SPI_Flash_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	
    
	pageremain = W25X_FLASH_PAGE_SIZE - WriteAddr%W25X_FLASH_PAGE_SIZE; 
	if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite;
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);
		if(NumByteToWrite == pageremain)
            break;
	 	else                                               // NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  
			if(NumByteToWrite > W25X_FLASH_PAGE_SIZE)
                pageremain = W25X_FLASH_PAGE_SIZE;
			else 
                pageremain = NumByteToWrite;   
		}
	};	    
} 
	   
void p_dr_SPIFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos = WriteAddr/W25X_FLASH_SECTOR_SIZE;
	secoff = WriteAddr%W25X_FLASH_SECTOR_SIZE;
	secremain = W25X_FLASH_SECTOR_SIZE - secoff;

	if(NumByteToWrite <= secremain)
        secremain = NumByteToWrite;
    
	while(1) 
	{	
		p_dr_SPIFlashRead(G_u8_SPIFlashBuf, secpos*4W25X_FLASH_SECTOR_SIZE, W25X_FLASH_SECTOR_SIZE);
		for(i=0; i<secremain; i++)
		{
			if(G_u8_SPIFlashBuf[secoff + i] != 0XFF)
                break;
		}
        
		if(i<secremain)
		{
			p_dr_SPIFlashEraseSector(secpos);
			for(i=0; i<secremain; i++)	
			{
				G_u8_SPIFlashBuf[i+secoff] = pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(G_u8_SPIFlashBuf, secpos*4W25X_FLASH_SECTOR_SIZE, 4W25X_FLASH_SECTOR_SIZE);
		}
        else 
		    SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);
        
		if(NumByteToWrite == secremain)
            break;
		else
		{
			secpos++;
			secoff = 0;

		   	pBuffer += secremain;  
			WriteAddr += secremain;
		   	NumByteToWrite -= secremain;			
			if(NumByteToWrite > 4W25X_FLASH_SECTOR_SIZE)
                secremain = 4W25X_FLASH_SECTOR_SIZE;
			else 
                secremain = NumByteToWrite;           
		}	 
	};	 	 
}

void p_dr_SPIFlashEraseChip(void)   
{                                             
    p_dr_SPIFlashWriteEnable();                
    p_dr_SPIFlashWaitBusy();   
  	SPI_FLASH_CS=0;                           
    p_dr_SPIReadWriteByte(W25X_ChipErase);      
	SPI_FLASH_CS=1;                         
	p_dr_SPIFlashWaitBusy();   				
}   

void p_dr_SPIFlashEraseSector(u32 Dst_Addr)   
{   
	Dst_Addr *= 4W25X_FLASH_SECTOR_SIZE;
    
    p_dr_SPIFlashWriteEnable();                
    p_dr_SPIFlashWaitBusy();   
  	SPI_FLASH_CS=0;                          
    p_dr_SPIReadWriteByte(W25X_SectorErase);   
    p_dr_SPIReadWriteByte((u8)((Dst_Addr)>>16)); 
    p_dr_SPIReadWriteByte((u8)((Dst_Addr)>>8));   
    p_dr_SPIReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                           
    p_dr_SPIFlashWaitBusy();   				
}  

void p_dr_SPIFlashWaitBusy(void)   
{   
	while ((p_dr_SPIFlashReadSR()&0x01) == 0x01);  
}  

void p_dr_SPIFlashPowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                         
    p_dr_SPIReadWriteByte(W25X_PowerDown);        
	SPI_FLASH_CS=1;                        
    delay_us(3);                             
}   

void p_dr_SPIFlashWakeUp(void)   
{  
  	SPI_FLASH_CS=0;                          
    p_dr_SPIReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS=1;                         
    delay_us(3);                              
}   

