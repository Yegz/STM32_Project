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

    Clr_NRF24L01_CE;         // ʹ��24L01  NRF24L01_CE
    NRF24L01_CSN_DISABLE;    // SPIƬѡȡ�� NRF24L01_CSN
}

// ���24L01�Ƿ����
// ����ֵ:0���ɹ�;1��ʧ��
u8 p_dr_NRF24L01Check(void)
{
    u8 buf[5]= {0XA5,0XA5,0XA5,0XA5,0XA5};
    u8 i;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                 // spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+TX_ADDR, buf, 5); // д��5���ֽڵĵ�ַ.
    p_dr_NRF24L01ReadBuf(TX_ADDR, buf, 5);                     // ����д��ĵ�ַ
    for(i=0; i<5; i++)
    {
        if(buf[i] != 0XA5)
            break;
    }
    
    if(i != 5)
        return 1;                                             // ���24L01����

    return 0;                                                 // ��⵽24L01
}

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 p_dr_NRF24L01WriteReg(u8 reg,u8 value)
{
    u8 status;
    
    NRF24L01_CSN_ENABLE;                 // ʹ��SPI����
    status = p_dr_SPIReadWriteByte(reg); // ���ͼĴ�����
    p_dr_SPIReadWriteByte(value);        // д��Ĵ�����ֵ
    NRF24L01_CSN_DISABLE;                // ��ֹSPI����

    return(status);                      // ����״ֵ̬
}

//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 p_dr_NRF24L01ReadReg(u8 reg)
{
    u8 reg_val;
    
    NRF24L01_CSN_ENABLE;                // ʹ��SPI����
    p_dr_SPIReadWriteByte(reg);            // ���ͼĴ�����
    reg_val = p_dr_SPIReadWriteByte(0XFF); // ��ȡ�Ĵ�������
    NRF24L01_CSN_DISABLE;               // ��ֹSPI����

    return(reg_val);                    // ����״ֵ̬
}

//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 p_dr_NRF24L01ReadBuf(u8 reg,u8 *pBuf,u8 len)
{
    u8 status,u8_ctr;
    
    NRF24L01_CSN_ENABLE;                                                      // ʹ��SPI����
    status = p_dr_SPIReadWriteByte(reg);                                      // ���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for(u8_ctr=0; u8_ctr<len; u8_ctr++)
        pBuf[u8_ctr] = p_dr_SPIReadWriteByte(0XFF);                           // ��������
    NRF24L01_CSN_DISABLE;                                                     // �ر�SPI����

    return status;                                                            // ���ض�����״ֵ̬
}

// ��ָ��λ��дָ�����ȵ�����
// reg:�Ĵ���(λ��)
// *pBuf:����ָ��
// len:���ݳ���
// ����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 p_dr_NRF24L01WriteBuf(u8 reg, u8 *pBuf, u8 len)
{
    u8 status,u8_ctr;
    
    NRF24L01_CSN_ENABLE;                                               // ʹ��SPI����
    status = p_dr_SPIReadWriteByte(reg);                               // ���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for(u8_ctr=0; u8_ctr<len; u8_ctr++)
        p_dr_SPIReadWriteByte(*pBuf++);                                // д������
    NRF24L01_CSN_DISABLE;                                              // �ر�SPI����

    return status;                                                     // ���ض�����״ֵ̬
}

// ����NRF24L01����һ������
// txbuf:�����������׵�ַ
// ����ֵ:�������״��
u8 p_dr_NRF24L01TxPacket(u8 *txbuf)
{
    u8 sta;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                         // spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);// д���ݵ�TX BUF  32���ֽ�
    Set_NRF24L01_CE;                                                   // ��������
    while(NRF24L01_IRQ!=0);                                            // �ȴ��������
    sta = p_dr_NRF24L01ReadReg(STATUS);                                // ��ȡ״̬�Ĵ�����ֵ
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+STATUS, sta);             // ���TX_DS��MAX_RT�жϱ�־

    if(sta & MAX_TX)                                                   // �ﵽ����ط�����
    {
        p_dr_NRF24L01WriteReg(NRF24L01_FLUSH_TX, 0xff);                // ���TX FIFO�Ĵ���
        return MAX_TX;
    }
    
    if(sta & TX_OK)                                                    // �������
    {
        return TX_OK;
    }
    
    return 0xff;                                                       // ����ԭ����ʧ��
}

// ����NRF24L01����һ������
// txbuf:�����������׵�ַ
// ����ֵ:0��������ɣ��������������
u8 p_dr_NRF24L01RxPacket(u8 *rxbuf)
{
    u8 sta;
    
    p_dr_SPISetSpeed(SPI_BaudRatePrescaler_8);                            // spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��
    sta = p_dr_NRF24L01ReadReg(STATUS);                                   // ��ȡ״̬�Ĵ�����ֵ
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+STATUS, sta);                // ���TX_DS��MAX_RT�жϱ�־
    if(sta&RX_OK)                                                         // ���յ�����
    {
        p_dr_NRF24L01ReadBuf(NRF24L01_RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);// ��ȡ����
        p_dr_NRF24L01WriteReg(NRF24L01_FLUSH_RX, 0xff);                   // ���RX FIFO�Ĵ���

        return 0;
    }
    
    return 1;                                                             // û�յ��κ�����
}

// �ú�����ʼ��NRF24L01��RXģʽ
// ����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
// ��CE��ߺ�,������RXģʽ,�����Խ���������
void p_dr_NRF24L01RXMode(void)
{
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // дRX�ڵ��ַ

    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_AA, NRF24L01_ENAA_P0_ENABLE);
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_RXADDR, NRF24L01_ENRXADDR_P0_ENABLE);
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_CH, 40);               // ����RFͨ��Ƶ��
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RX_PW_P0, RX_PLOAD_WIDTH);// ѡ��ͨ��0����Ч���ݿ��
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_SETUP, 0x0f);          // ����TX�������,0db����,2Mbps,���������濪��
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+CONFIG, 0x0f);            // ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
    Set_NRF24L01_CE;                                                   // CEΪ��,�������ģʽ
}

// �ú�����ʼ��NRF24L01��TXģʽ
// ����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
// PWR_UP,CRCʹ��
// ��CE��ߺ�,������RXģʽ,�����Խ���������
// CEΪ�ߴ���10us,����������.
void p_dr_NRF24L01TXMode(void)
{
    Clr_NRF24L01_CE;
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);    // дTX�ڵ��ַ
    p_dr_NRF24L01WriteBuf(NRF24L01_WRITE_REG+RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); // ����TX�ڵ��ַ,��ҪΪ��ʹ��ACK

    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_AA, NRF24L01_ENAA_P0_ENABLE);    
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+EN_RXADDR, NRF24L01_ENRXADDR_P0_ENABLE); 
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+SETUP_RETR, 0x1a);// �����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_CH, 40);       // ����RFͨ��Ϊ40
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+RF_SETUP, 0x0f);  // ����TX�������,0db����,2Mbps,���������濪��
    p_dr_NRF24L01WriteReg(NRF24L01_WRITE_REG+CONFIG, 0x0e);    // ���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
    Set_NRF24L01_CE;                                           // CEΪ��,10us����������
}

