#include "fontupd.h"
#include "ff.h"	  
#include "flash.h"   
#include "lcd.h"  
#include "delay.h"

_font_info ftinfo=
{
	FONTINFOADDR+100000,  					
	0,
	0,
	0,
};

const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";		
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNI2GBK.SYS";	

FIL gbkFile;
u8 temp[1024];

u32 fupd_prog(u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(108,110,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(84,110,t,3,16);
	}
	return 0;					    
}

u8 updata_fontx(u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;
 	u32 tempsys[2]; 
	FIL * fftemp;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	fftemp=&gbkFile;
	if(fftemp==NULL)return 1;			
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)
	{
		f_close(fftemp);
		return 2;
	}
	if(fftemp==NULL)return 2;

	if(fx==0)
	{
 		tempsys[0]=ftinfo.ugbkaddr;							
		tempsys[1]=fftemp->fsize;	 	  					
		SPI_Flash_Write((u8*)tempsys,FONTINFOADDR,8);		
		flashaddr=ftinfo.ugbkaddr;
	}else//GBK16
	{
		SPI_Flash_Read((u8*)tempsys,FONTINFOADDR,8);    
		flashaddr=tempsys[0]+tempsys[1];
		tempsys[0]=flashaddr;								
		tempsys[1]=fftemp->fsize;	 	  					
		SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+8,8);	
	}	   
	while(res==FR_OK)
	{
 	 	res=f_read(fftemp,temp,1024,(UINT *)&bread);	
		if(res!=FR_OK)break;				
		SPI_Flash_Write(temp,offx+flashaddr,1024);
  		offx+=bread;	  
		fupd_prog(fftemp->fsize,offx);			
		if(bread!=1024)break;					
 	} 
	return res;
}
	  
u8 update_font()
{	
	u8 *gbk16_path;
	u8 *unigbk_path;
	u8 res;
	unigbk_path=(u8*)UNIGBK_SDPATH;
	gbk16_path=(u8*)GBK16_SDPATH;

	POINT_COLOR=RED;	 
	LCD_ShowString(60,90,"Updating UNI2GBK.SYS");		
	res=updata_fontx(unigbk_path,0);
	if(res)return 1;
	LCD_ShowString(60,90,"Updating GBK16.FON  ");
	res=updata_fontx(gbk16_path,1);
	if(res)return 2;	   
	
	res=0XAA;
   	SPI_Flash_Write(&res,FONTINFOADDR+24,1);
	return 0;
}

u8 font_init(void)
{			  
	u32 tempsys[2];

	u8 fontok=0;

 	SPI_Flash_Read(&fontok,FONTINFOADDR+24,1);

	if(fontok!=0XAA)return 1;
 	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR,8);

	ftinfo.ugbksize=tempsys[1];

	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+8,8);

	ftinfo.f16addr=tempsys[0];	

	return 0;		    
}

