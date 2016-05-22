#include "24l01.h"
#include "lcd.h"
#include "delay.h"
#include "spi.h"

/**************************************************************************************
* Data
**************************************************************************************/
const u8 TX_ADDRESS[TX_ADR_WIDTH]= {0x34, 0x43, 0x10, 0x10, 0x01};
const u8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34, 0x43, 0x10, 0x10, 0x01};

/**************************************************************************************
* Function Implementation
**************************************************************************************/
void p_dr_NRF24L01Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_4);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    p_dr_SPIInit();

    Clr_NRF24L01_CE;         // 使能24L01  NRF24L01_CE
    NRF24L01_CSN_DISABLE;    // SPI片选取消 NRF24L01_CSN
}

// 检测24L01是否存在
// 返回值:0，成功;1，失败
u8 p_dr_NRF24L01Check(void)
{
    u8 buf[5]= {0XA5,0XA5,0XA5,0XA5,0XA5};
    u8 i;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                 // spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+TX_ADDR, buf, 5); // 写入5个字节的地址.
    p_dr_NRF24L01ReadBuf(TX_ADDR, buf, 5);                     // 读出写入的地址
    for(i=0; i<5; i++)
    {
        if(buf[i] != 0XA5)
            break;
    }
    
    if(i != 5)
        return 1;                                             // 检测24L01错误

    return 0;                                                 // 检测到24L01
}

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 p_dr_NRF24L01WriteReg(u8 reg,u8 value)
{
    u8 status;
    
    NRF24L01_CSN_ENABLE;                 // 使能SPI传输
    status = p_dr_SPIReadWriteByte(reg); // 发送寄存器号
    p_dr_SPIReadWriteByte(value);        // 写入寄存器的值
    NRF24L01_CSN_DISABLE;                // 禁止SPI传输

    return(status);                      // 返回状态值
}

//读取SPI寄存器值
//reg:要读的寄存器
u8 p_dr_NRF24L01ReadReg(u8 reg)
{
    u8 reg_val;
    
    NRF24L01_CSN_ENABLE;                // 使能SPI传输
    p_dr_SPIReadWriteByte(reg);            // 发送寄存器号
    reg_val = p_dr_SPIReadWriteByte(0XFF); // 读取寄存器内容
    NRF24L01_CSN_DISABLE;               // 禁止SPI传输

    return(reg_val);                    // 返回状态值
}

//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 p_dr_NRF24L01ReadBuf(u8 reg,u8 *pBuf,u8 len)
{
    u8 status,u8_ctr;
    
    NRF24L01_CSN_ENABLE;                                                      // 使能SPI传输
    status = p_dr_SPIReadWriteByte(reg);                                      // 发送寄存器值(位置),并读取状态值
    for(u8_ctr=0; u8_ctr<len; u8_ctr++)
        pBuf[u8_ctr] = p_dr_SPIReadWriteByte(0XFF);                           // 读出数据
    NRF24L01_CSN_DISABLE;                                                     // 关闭SPI传输

    return status;                                                            // 返回读到的状态值
}

// 在指定位置写指定长度的数据
// reg:寄存器(位置)
// *pBuf:数据指针
// len:数据长度
// 返回值,此次读到的状态寄存器值
u8 p_dr_NRF24L01WriteBuf(u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;
    
    NRF24L01_CSN_ENABLE;                                               // 使能SPI传输
    status = p_dr_SPIReadWriteByte(reg);                               // 发送寄存器值(位置),并读取状态值
    for(u8_ctr=0; u8_ctr<len; u8_ctr++)
        p_dr_SPIReadWriteByte(*pBuf++);                                // 写入数据
    NRF24L01_CSN_DISABLE;                                              // 关闭SPI传输

    return status;                                                     // 返回读到的状态值
}

// 启动NRF24L01发送一次数据
// txbuf:待发送数据首地址
// 返回值:发送完成状况
u8 p_dr_NRF24L01TxPacket(u8 *txbuf)
{
    u8 sta;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                         // spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);// 写数据到TX BUF  32个字节
    Set_NRF24L01_CE;                                                   // 启动发送
    while(NRF24L01_IRQ!=0);                                            // 等待发送完成
    sta = p_dr_NRF24L01ReadReg(STATUS);                                // 读取状态寄存器的值
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+STATUS, sta);             // 清除TX_DS或MAX_RT中断标志

    if(sta & MAX_TX)                                                   // 达到最大重发次数
    {
        p_dr_NRF24L01WriteReg(NRF24L01_FLUSH_TX, 0xff);                // 清除TX FIFO寄存器
        return MAX_TX;
    }
    
    if(sta & TX_OK)                                                    // 发送完成
    {
        return TX_OK;
    }
    
    return 0xff;                                                       // 其他原因发送失败
}

// 启动NRF24L01发送一次数据
// txbuf:待发送数据首地址
// 返回值:0，接收完成；其他，错误代码
u8 p_dr_NRF24L01RxPacket(u8 *rxbuf)
{
    u8 sta;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                            // spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）
    sta = p_dr_NRF24L01ReadReg(STATUS);                                   // 读取状态寄存器的值
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+STATUS, sta);                // 清除TX_DS或MAX_RT中断标志
    if(sta&RX_OK)                                                         // 接收到数据
    {
        p_dr_NRF24L01ReadBuf(NRF24L01_RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);// 读取数据
        p_dr_NRF24L01WriteReg(NRF24L01_FLUSH_RX, 0xff);                   // 清除RX FIFO寄存器

        return 0;
    }
    
    return 1;                                                             // 没收到任何数据
}

// 该函数初始化NRF24L01到RX模式
// 设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
// 当CE变高后,即进入RX模式,并可以接收数据了
void p_dr_NRF24L01RXMode(void)
{
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // 写RX节点地址

    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_AA, NRF24L01_ENAA_P0_ENABLE);
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_RXADDR, NRF24L01_ENRXADDR_P0_ENABLE);
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_CH, 40);               // 设置RF通信频率
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RX_PW_P0, RX_PLOAD_WIDTH);// 选择通道0的有效数据宽度
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_SETUP, 0x0f);          // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+CONFIG, 0x0f);            // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
    Set_NRF24L01_CE;                                                   // CE为高,进入接收模式
}

// 该函数初始化NRF24L01到TX模式
// 设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
// PWR_UP,CRC使能
// 当CE变高后,即进入RX模式,并可以接收数据了
// CE为高大于10us,则启动发送.
void p_dr_NRF24L01TXMode(void)
{
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);    // 写TX节点地址
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // 设置TX节点地址,主要为了使能ACK

    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_AA, NRF24L01_ENAA_P0_ENABLE);    
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_RXADDR, NRF24L01_ENRXADDR_P0_ENABLE); 
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+SETUP_RETR, 0x1a);// 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_CH, 40);       // 设置RF通道为40
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_SETUP, 0x0f);  // 设置TX发射参数,0db增益,2Mbps,低噪声增益开启
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+CONFIG, 0x0e);    // 配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
    Set_NRF24L01_CE;                                           // CE为高,10us后启动发送
}

