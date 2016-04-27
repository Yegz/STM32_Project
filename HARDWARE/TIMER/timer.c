#include "timer.h"
#include "led.h" 

void Timerx_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 5000;
    TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
    // TIM IT enable
    TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_Trigger, ENABLE);
    // Enable the TIM3 global Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);  
    TIM_Cmd(TIM3, ENABLE);						 
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  
        /* Pin PD.02 toggling with frequency = 10KHz */
        //GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)));
        DR_LED_LED1 =!DR_LED_LED1;
    }
}












