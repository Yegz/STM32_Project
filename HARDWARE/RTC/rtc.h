#ifndef __RTC_H
#define __RTC_H	    

typedef struct 
{
    u8 hour;
    u8 min;
    u8 sec;			
    u16 w_year;
    u8  w_month;
    u8  w_date;
    u8  week;		 
}tm;	
				 
extern tm timer; 
extern u8 const mon_table[12];

void Disp_Time(u8 x,u8 y,u8 size);
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);
u8   RTC_Init(void);    
u8   Is_Leap_Year(u16 year);
u8   RTC_Get(void);   
u8   RTC_Get_Week(u16 year,u8 month,u8 day);
u8   RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
#endif




























 
