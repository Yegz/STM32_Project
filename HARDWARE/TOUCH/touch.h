#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "sys.h"

#define Key_Down 0x01
#define Key_Up   0x00 

typedef struct 
{
	u16 X0;
	u16 Y0;
	u16 X; 
	u16 Y;						   	    
	u8  Key_Sta;
	float xfac;
	float yfac;
	short xoff;
	short yoff;
	u8 touchtype;
}Pen_Holder;	   
extern Pen_Holder Pen_Point;
#define PEN  PCin(1)   //PC1  INT
#define DOUT PCin(2)   //PC2  MISO
#define TDIN PCout(3)  //PC3  MOSI
#define TCLK PCout(0)  //PC0  SCLK
#define TCS  PCout(13) //PC13 CS    
//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 指令集
//#define CMD_RDX   0X90  //0B10010000即用差分方式读X坐标
//#define CMD_RDY	0XD0  //0B11010000即用差分方式读Y坐标
extern u8 CMD_RDX;
extern u8 CMD_RDY;
   											 
#define TEMP_RD	0XF0
#define ADJ_SAVE_ENABLE	    
			  
void Touch_Init(void);		
u8 Read_ADS(u16 *x,u16 *y);	
u8 Read_ADS2(u16 *x,u16 *y);
u16 ADS_Read_XY(u8 xy);		
u16 ADS_Read_AD(u8 CMD);	
void ADS_Write_Byte(u8 num); 
void Drow_Touch_Point(u8 x,u16 y);
void Draw_Big_Point(u8 x,u16 y);  
void Touch_Adjust(void);          
void Save_Adjdata(void);		
u8 Get_Adjdata(void); 			
void Pen_Int_Set(u8 en); 		
void Convert_Pos(void);           
#endif

