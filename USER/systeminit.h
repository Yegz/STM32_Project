#ifndef SYSTEMINIT__H_
#define SYSTEMINIT__H_

/**************************************************************************************
* Includes
**************************************************************************************/
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
#include "24l01.h"

/***************************************************************************************
* Define
****************************************************************************************/

/***************************************************************************************
* Data
***************************************************************************************/

/***************************************************************************************
* Function prototypes
****************************************************************************************/
void p_drUser_SystemInit(void);

#endif
