/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   24-apr-2016                      Git        add  Led status set and get functions
 *
 **/
/**************************************************************************************
* Function Implementation
**************************************************************************************/
#include "stm32f10x.h"
#include "led.h"
#include "sys.h"

/**************************************************************************************
* Function Implementation
**************************************************************************************/
void p_dr_LedInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD,GPIO_Pin_2);
}

void p_dr_SetLedStatus(GpioTypeDef_t GpioType, u8 GpioPinNum, u8 Value)
{
    switch(GpioType)
    {
        case DR_LED_GPIOA:
            BIT_ADDR(GPIOA_ODR_Addr, GpioPinNum) = Value;
            break;
        case DR_LED_GPIOB:
            BIT_ADDR(GPIOB_ODR_Addr, GpioPinNum) = Value;
            break;
        case DR_LED_GPIOC:
            BIT_ADDR(GPIOC_ODR_Addr, GpioPinNum) = Value;
            break;
        case DR_LED_GPIOD:
            BIT_ADDR(GPIOD_ODR_Addr, GpioPinNum) = Value;
            break;
        default:
            break;
    }
}

u8 p_dr_GetLedStatus(GpioTypeDef_t GpioType, u8 GpioPinNum)
{
    u8 u8_LedStatus;

    switch(GpioType)
    {
        case DR_LED_GPIOA:
            u8_LedStatus = BIT_ADDR(GPIOA_IDR_Addr, GpioPinNum);
            break;
        case DR_LED_GPIOB:
            u8_LedStatus = BIT_ADDR(GPIOB_IDR_Addr, GpioPinNum);
            break;
        case DR_LED_GPIOC:
            u8_LedStatus = BIT_ADDR(GPIOC_IDR_Addr, GpioPinNum);
            break;
        case DR_LED_GPIOD:
            u8_LedStatus = BIT_ADDR(GPIOD_IDR_Addr, GpioPinNum);
            break;
        default:
            u8_LedStatus = DR_LED_LEDSTATUSERR;
            break;
    }

    return u8_LedStatus;
}

