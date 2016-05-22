/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   15-May-2016         Git           
 *
 **/

#include "spi.h"

/**************************************************************************************
* Data
**************************************************************************************/

/**************************************************************************************
* Function Implementation
**************************************************************************************/
void p_dr_SPIInit(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC->APB2RSTR |= 1<<12;   // Reset SPI1
    RCC->APB2RSTR &= ~(1<<12);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );

    /* Configure SPI1 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI1 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = 0; //SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = 0; //SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;   //SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);

    p_dr_SPIReadWriteByte(0xff);
}


void p_dr_SPISetSpeed(u8 SpeedSet)
{
    SPI1->CR1 &= 0XFFC7;          // Fsck: Fcpu/256
    switch(SpeedSet)
    {
        case SPI_SPEED_2:
            SPI1->CR1 |= 0<<3;    // Fsck: Fpclk/2=36Mhz
            break;
        case SPI_SPEED_4:
            SPI1->CR1 |= 1<<3;    // Fsck: Fpclk/4=18Mhz
            break;
        case SPI_SPEED_8:
            SPI1->CR1 |= 2<<3;    // Fsck: Fpclk/8=9Mhz
            break;
        case SPI_SPEED_16:
            SPI1->CR1 |= 3<<3;    // Fsck: Fpclk/16=4.5Mhz
            break;
        case SPI_SPEED_32:
            SPI1->CR1 |= 4<<3;    // Fsck: Fpclk/32=2.25MHz
            break;
        case SPI_SPEED_64:
            SPI1->CR1 |= 5<<3;    // Fsck: Fpclk/64=1.125MHz
            break;
        case SPI_SPEED_128:
            SPI1->CR1 |= 6<<3;    // Fsck: Fpclk/128=562.5KHz
            break;
        case SPI_SPEED_256:
            SPI1->CR1 |= 7<<3;    // Fsck: Fpclk/16=281.25Khz
            break;
    }

    SPI1->CR1 |= 1<<6;
}

u8 p_dr_SPIReadWriteByte(u8 TxData)
{
    u8 retry = 0;

    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        if(retry>200)return 0;
    }

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, TxData);
    retry=0;

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if(retry>200)return 0;
    }

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

