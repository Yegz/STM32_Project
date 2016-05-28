/* == HISTORY =========================================================
 *
 * Name     Date                Ver     Action
 * --------------------------------------------------------------------
 * Carols   24-apr-2016         Git     add  user systeminit functions
 *
 **/

#include "systeminit.h"

/**************************************************************************************
* Function Implementation
**************************************************************************************/
void p_drUser_SystemInit(void)
{
//    u8 key,mode;
//	u16 t=0;			 
//	u8 tmp_buf[33];
    
    SystemInit();
    delay_init(72);
    NVIC_Configuration();
    p_dr_UartInit(9600);
    p_dr_LedInit();
    //LCD_Init();
    //p_dr_NRF24L01Init();
    p_dr_KeyInit();
    p_dr_MatrixKeyBoardInit();

    //POINT_COLOR=RED;
    //LCD_ShowString(60,50,"Mini STM32");
    //LCD_ShowString(60,70,"USART TEST");
    //LCD_ShowString(60,90,"2016/4/24");
    while(1)
    {
        p_dr_KeyScan();
        p_dr_KeyDebug();
        delay_ms(500);
        DR_LED_LED0 = !DR_LED_LED0;
    }
#if 0
	mode = 0;
	LCD_Fill(10,150,240,166,WHITE);
 	POINT_COLOR=BLUE;   
	if(mode==0)
	{
		LCD_ShowString(60,150,"NRF24L01 RX_Mode");	
		LCD_ShowString(60,170,"Received DATA:");	
		p_dr_NRF24L01RXMode();		  
		while(1)
		{	  		    		    				 
			if(p_dr_NRF24L01TxPacket(tmp_buf)==0)
			{
				tmp_buf[32]=0;
				LCD_ShowString(0,190,tmp_buf); 
			}else delay_us(100);	   
			t++;
			if(t==10000)
			{
				t=0;
				DR_LED_LED0=!DR_LED_LED0;
			} 				    
		};	
	}else
	{							    
		LCD_ShowString(60,150,"NRF24L01 TX_Mode");	
		p_dr_NRF24L01TXMode();
		mode=' ';
		while(1)
		{	  		   				 
			if(p_dr_NRF24L01TxPacket(tmp_buf)==TX_OK)
			{
				LCD_ShowString(60,170,"Sended DATA:");	
				LCD_ShowString(0,190,tmp_buf); 
				key=mode;
				for(t=0;t<32;t++)
				{
					key++;
					if(key>('~'))key=' ';
					tmp_buf[t]=key;	
				}
				mode++; 
				if(mode>'~')mode=' ';  	  
				tmp_buf[32]=0;	   
			}else
			{										   	
 				LCD_ShowString(60,170,"Send Failed "); 
				LCD_Fill(0,188,240,218,WHITE);		   
			};
			DR_LED_LED0=!DR_LED_LED0;
			delay_ms(1500);				    
		};
	}
#endif
}

