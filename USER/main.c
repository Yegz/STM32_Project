#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "spi.h"
#include "usart.h"
#include "mmc_sd.h"
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"
#include <stdio.h>

u8 Usb_Status_Reg=0;

int main(void)
{	
    u8 offline_cnt=0;
    u8 tct=0;
    u8 USB_STA;
    u8 Divece_STA;
    SystemInit();
    delay_init(72);
    NVIC_Configuration();
    uart_init(9600);
    LED_Init();

    LCD_Init();

    POINT_COLOR=RED; 
    LCD_ShowString(60,50,"Mini STM32");	
    LCD_ShowString(60,70,"USB TEST");	
    LCD_ShowString(60,90,"ATOM@ALIENTEK");
    LCD_ShowString(60,110,"2010/6/19");	

    while(SD_Init())
    {
        LCD_ShowString(60,130,"SD Init ERR!");	
        delay_ms(500);
        LCD_ShowString(60,130,"Please Check");	
        delay_ms(500);
    }					  													 
    LCD_ShowString(60,130,"SD Card Ready");
    Mass_Memory_Size[0]=SD_GetCapacity();
    Mass_Block_Size[0] =512;
    Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
    LCD_ShowString(60,150,"USB Connecting...");
    //USB Configuration
    USB_Interrupts_Config();    
    Set_USBClock();   
    USB_Init();	  	 								   	 
    while(1)
    {	
        delay_ms(1);
    	 if(USB_STA!=Usb_Status_Reg) 
        {	 						   
            LCD_ShowString(60,170,"              ");  
            if(Usb_Status_Reg&0x01)		  
            {
                LCD_ShowString(60,170,"USB Writing...");
            }
            if(Usb_Status_Reg&0x02)
            {
                LCD_ShowString(60,170,"USB Reading...");		 
            }
            
            if(Usb_Status_Reg&0x04)
                LCD_ShowString(60,190,"USB Write Err ");
            else 
                LCD_ShowString(60,190,"              ");
            if(Usb_Status_Reg&0x08)
                LCD_ShowString(60,210,"USB Read  Err ");
            else 
                LCD_ShowString(60,210,"              ");
            
            USB_STA=Usb_Status_Reg;
        }
    	if(Divece_STA!=bDeviceState) 
    	{
            if(bDeviceState==CONFIGURED)
                LCD_ShowString(60,150,"USB Connected    ");
            else 
                LCD_ShowString(60,150,"USB DisConnected ");
            Divece_STA=bDeviceState;
    	}
    	tct++;
    	if(tct==200)
    	{
            tct=0;
            LED0=!LED0;
            if(Usb_Status_Reg&0x10)
            {
                offline_cnt=0;
                bDeviceState=CONFIGURED;
            }else
            {
                offline_cnt++;  
                if(offline_cnt>10)bDeviceState=UNCONNECTED;
            }
            Usb_Status_Reg=0;
        }
    }	   		   
}
