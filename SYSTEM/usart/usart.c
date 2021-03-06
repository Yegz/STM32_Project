/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   24-apr-2016                      Git        add  uart debug demo
 *
 **/
#include <stdio.h>
#include <rt_misc.h>
#include "sys.h"
#include "usart.h"

/***************************************************************************************
* Data
***************************************************************************************/
static u8 USART_RX_BUF[64];
static u8 USART_RX_STA = 0;

/**************************************************************************************
* Function Implementation
**************************************************************************************/
#if USART_USE_MICRO_LIB

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t) ch);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

    return ch;
}

int GetKey (void)
{
    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & USART_DATA_MASK));
}

#else

int p_dr_UartSendChar(int ch)
{
    while(!(USART1->SR & USART_FLAG_TXE));
    USART1->DR = (ch & USART_DATA_MASK);
    return ch;
}

int p_dr_UartGetKey(void)
{
    while(!(USART1->SR & USART_FLAG_RXNE));
    return ((int)(USART1->DR & USART_DATA_MASK));
}

#pragma import(__use_no_semihosting_swi)

struct __FILE
{
    int handle; // add whatever you need here
};

FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
    return (p_dr_UartSendChar(ch));
}

int fgetc(FILE*f)
{
    return (p_dr_UartSendChar(p_dr_UartGetKey()));
}

void _ttywrch(int ch)
{
    p_dr_UartSendChar(ch);
}

/* You implementation of ferror */
int ferror(FILE *f)
{
    return EOF;
}

/* endless loop */
void _sys_exit(int x)
{
    x = x;    
}

#endif

void p_dr_UartInit(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    //USART1_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart1 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
    u8 Res;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Res = USART_ReceiveData(USART1);//(USART1->DR);

        if((USART_RX_STA&0x80) == 0)
        {
            if(USART_RX_STA&0x40)
            {
                if(Res != 0x0a)
                    USART_RX_STA=0;
                else
                    USART_RX_STA |= 0x80;
            }
            else
            {
                if(Res == 0x0d)
                    USART_RX_STA |= 0x40;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3F] = Res ;
                    USART_RX_STA++;
                    if(USART_RX_STA > 63)
                        USART_RX_STA = 0;
                }
            }
        }
    }
}

