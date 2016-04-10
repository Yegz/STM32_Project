#include "string.h"
#include "exfuns.h"
#include "fattester.h"	

const u8 *FILE_TYPE_TBL[6][13]=
{
{"BIN"},			
{"LRC"},			
{"NES"},			
{"TXT","C","H"},	
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},
{"BMP","JPG","JPEG","GIF"},
};

FATFS fs[2];  		
FIL file;	  		
FIL ftemp;	  		
UINT br,bw;			
FILINFO fileinfo;	
DIR dir;  			
u8 fatbuf[512];

u8 char_upper(u8 c)
{
	if(c<'A')return c;
	if(c>='a')return c-0x20;
	else return c;
}	      

u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;
		fname++;
	}
	if(i==250)return 0XFF;
 	for(i=0;i<5;i++)
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);
	for(i=0;i<6;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;
}	 

