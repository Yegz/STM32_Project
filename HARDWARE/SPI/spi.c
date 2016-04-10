#include "spi.h"

void SPIx_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC->APB2RSTR|=1<<12;      
    RCC->APB2RSTR&=~(1<<12);   

    //	RCC->APB2ENR|=1<<2;       
    //	RCC->APB2ENR|=1<<12;      //SPI1
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	

    //	GPIOA->CRL&=0X000FFFFF; 
    //	GPIOA->CRL|=0XBBB00000;
    //	GPIOA->ODR|=0X7<<5;  
    	/* Configure SPI1 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //	SPI1->CR1|=0<<10;
    //	SPI1->CR1|=1<<9; 
    //	SPI1->CR1|=1<<8;  
    //	
    //	SPI1->CR1|=1<<2; 
    //	SPI1->CR1|=0<<11;//8bit
    //  CPHA=0;CPOL=0;
    //	SPI1->CR1|=1<<1;  
    //	SPI1->CR1|=1<<0; 
    //	
    //	SPI1->CR1|=7<<3; //Fsck=Fcpu/256
    //	SPI1->CR1|=0<<7; //MSBfirst   

    /* SPI1 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//SPI_NSS_Soft;		
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure); 


    //SPI1->CR1|=1<<6;
    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE); 

    SPIx_ReadWriteByte(0xff);	 
} 
  

void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	switch(SpeedSet)
		{
		case SPI_SPEED_2:
			SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
			break;
		case SPI_SPEED_4:
			SPI1->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
			break;
		case SPI_SPEED_8:
			SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
			break;
		case SPI_SPEED_16:
			SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
			break;
		case SPI_SPEED_256:
			SPI1->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
			break;
		}		 
	SPI1->CR1|=1<<6;  
} 

u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	//while((SPI1->SR&(1<<1))==0)
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) 
		{
		retry++;
		if(retry>200)return 0;
		}			  
	//SPI1->DR=TxData;	 	  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, TxData); 
	retry=0;
	//while((SPI1->SR&(1<<0))==0) 
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	//return SPI1->DR;        
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1); 
}

