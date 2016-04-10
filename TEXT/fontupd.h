#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include "sys.h"												   
 
#define FONTINFOADDR	0	

__packed typedef struct 
{
	const u32 ugbkaddr; 	
	u32 f16addr;			
	u32 f12addr;			
	u32 ugbksize;		
}_font_info;

extern _font_info ftinfo;	


u8 update_font(void);
u8 font_init(void);
#endif

