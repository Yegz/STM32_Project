#include <string.h>
#include "sys.h"
#include "rtc.h"
#include "delay.h"
#include "usart.h"

tm G_st_Timer;//ʱ�ӽṹ��
//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������
void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;      //RTCȫ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //��ռ���ȼ�1λ,�����ȼ�3λ
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //��ռ���ȼ�0λ,�����ȼ�4λ
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //ʹ�ܸ�ͨ���ж�
    NVIC_Init(&NVIC_InitStructure);     //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

u8 p_dr_RtcInit(void)
{
    //����ǲ��ǵ�һ������ʱ��
    u8 temp=0;
    RTC_NVIC_Config();
    //if(BKP->DR1!=0X5050)//��һ������
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)      //��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
    {
        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);    //ʹ��PWR��BKP����ʱ��

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);    //ʹ��RTC�ͺ󱸼Ĵ�������

        /* Reset Backup Domain */
        BKP_DeInit();   //������BKP��ȫ���Ĵ�������Ϊȱʡֵ


        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);  //�����ⲿ���پ���(LSE),ʹ��������پ���
        /* Wait till LSE is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) //���ָ����RCC��־λ�������,�ȴ����پ������
        {
            temp++;
            delay_ms(10);
        }
        if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������
        /* Select LSE as RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);  //ʹ��RTCʱ��
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();       //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);       //ʹ��RTC���ж�
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        /* Set RTC prescaler: set RTC period to 1sec */
        /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
        RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        p_dr_RtcSet(2009,12,2,10,0,55);  //����ʱ��
        BKP_WriteBackupRegister(BKP_DR1, 0X5050);   //��ָ���ĺ󱸼Ĵ�����д���û���������
    }
    else//ϵͳ������ʱ
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)    //���ָ����RCC��־λ�������:POR/PDR��λ
        {
            //printf("\rPower On Reset occurred....");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)   //���ָ����RCC��־λ�������:�ܽŸ�λ
        {
            //printf("\rExternal Reset occurred....");
        }

        //printf("\rNo need to configure RTC....");
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();   //�ȴ����һ�ζ�RTC�Ĵ�����д�������

        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //ʹ��RTC���ж�
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    }
    p_dr_RtcGet();//����ʱ��

    /* Clear reset flags */
    RCC_ClearFlag();    //���RCC�ĸ�λ��־λ

    return 0; //ok
}


//RTC�жϷ�����
void RTC_IRQHandler(void)
{
    if(RTC->CRL&0x0001)//�����ж�
    {
        p_dr_RtcGet();//����ʱ��
        //printf("CRL:%d\n",RTC->CRL);
    }
    if(RTC->CRL&0x0002)//�����ж�
    {
        //printf("Alarm!\n");
        RTC->CRL&=~(0x0002);//�������ж�
        //���Ӵ���
    }
    RTC->CRL&=0X0FFA;         //�������������жϱ�־
    while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������
}


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 p_dr_RtcIsLeapYear(u16 year)
{
    if(year%4==0) //�����ܱ�4����
    {
        if(year%100==0)
        {
            if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400����
            else return 0;
        }
        else return 1;
    }
    else return 0;
}

//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�
u8 const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//ƽ����·����ڱ�
const u8 mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};


u8 p_dr_RtcSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<1970||syear>2099)return 1;
    for(t=1970; t<syear; t++) //��������ݵ��������
    {
        if(p_dr_RtcIsLeapYear(t))seccount+=31622400;//�����������
        else seccount+=31536000;              //ƽ���������
    }
    smon-=1;
    for(t=0; t<smon; t++)  //��ǰ���·ݵ����������
    {
        seccount+=(u32)mon_table[t]*86400;//�·����������
        if(p_dr_RtcIsLeapYear(syear)&&t==1)seccount+=86400;//����2�·�����һ���������
    }
    seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ����������
    seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;   //����������
    seccount+=sec;//�������Ӽ���ȥ

    //����ʱ��
    //RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    //RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
    //PWR->CR|=1<<8;    //ȡ��������д����
    //���������Ǳ����!
    //RTC->CRL|=1<<4;   //��������
    //RTC->CNTL=seccount&0xffff;
    //RTC->CNTH=seccount>>16;
    //RTC->CRL&=~(1<<4);//���ø���
    //while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    /* Change the current time */
    RTC_SetCounter(seccount);   //����RTC��������ֵ
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    return 0;
}

//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 p_dr_RtcGet(void)
{
    static u16 daycnt=0;
    u32 timecount=0;
    u32 temp=0;
    u16 temp1=0;

    timecount=RTC->CNTH;//�õ��������е�ֵ(������)
    timecount<<=16;
    timecount+=RTC->CNTL;

    temp=timecount/86400;   //�õ�����(��������Ӧ��)
    if(daycnt!=temp)//����һ����
    {
        daycnt=temp;
        temp1=1970; //��1970�꿪ʼ
        while(temp>=365)
        {
            if(p_dr_RtcIsLeapYear(temp1))//������
            {
                if(temp>=366)temp-=366;//�����������
                else
                {
                    temp1++;
                    break;
                }
            }
            else temp-=365;   //ƽ��
            temp1++;
        }
        G_st_Timer.w_year=temp1;//�õ����
        temp1=0;
        while(temp>=28)//������һ����
        {
            if(p_dr_RtcIsLeapYear(G_st_Timer.w_year)&&temp1==1)//�����ǲ�������/2�·�
            {
                if(temp>=29)temp-=29;//�����������
                else break;
            }
            else
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
                else break;
            }
            temp1++;
        }
        G_st_Timer.w_month=temp1+1;//�õ��·�
        G_st_Timer.w_date=temp+1;  //�õ�����
    }
    temp=timecount%86400;     //�õ�������
    G_st_Timer.hour=temp/3600;     //Сʱ
    G_st_Timer.min=(temp%3600)/60; //����
    G_st_Timer.sec=(temp%3600)%60; //����
    G_st_Timer.week=p_dr_RtcGetWeek(G_st_Timer.w_year,G_st_Timer.w_month,G_st_Timer.w_date);//��ȡ����
    return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
u8 p_dr_RtcGetWeek(u16 year,u8 month,u8 day)
{
    u16 temp2;
    u8 yearH,yearL;

    yearH=year/100;
    yearL=year%100;
    // ���Ϊ21����,�������100
    if (yearH>19)yearL+=100;
    // ����������ֻ��1900��֮���
    temp2=yearL+yearL/4;
    temp2=temp2%7;
    temp2=temp2+day+table_week[month-1];
    if (yearL%4==0&&month<3)temp2--;
    return(temp2%7);
}
//�Ƚ������ַ���ָ�����ȵ������Ƿ����
//����:s1,s2Ҫ�Ƚϵ������ַ���;len,�Ƚϳ���
//����ֵ:1,���;0,�����

u8 p_dr_RtcStrCmpx(u8*s1,u8*s2,u8 len)
{
    u8 i;
    for(i=0; i<len; i++)if((*s1++)!=*s2++)return 0;
    return 1;
}

#if 0
extern const u8 *COMPILED_DATE;//��ñ�������
extern const u8 *COMPILED_TIME;//��ñ���ʱ��
const u8 Month_Tab[12][3]= {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//�Զ�����ʱ��Ϊ������ʱ��
void p_dr_RtcAutoTimeSet(void)
{
    u8 temp[3];
    u8 i;
    u8 mon,date;
    u16 year;
    u8 sec,min,hour;
    for(i=0; i<3; i++)temp[i]=COMPILED_DATE[i];
    for(i=0; i<12; i++)if(str_cmpx((u8*)Month_Tab[i],temp,3))break;
    mon=i+1;//�õ��·�
    if(COMPILED_DATE[4]==' ')date=COMPILED_DATE[5]-'0';
    else date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';
    year=1000*(COMPILED_DATE[7]-'0')+100*(COMPILED_DATE[8]-'0')+10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';
    hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';
    min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';
    sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';
    p_dr_RtcSet(year,mon,date,hour,min,sec) ;
    //printf("%d-%d-%d  %d:%d:%d\n",year,mon,date,hour,min,sec);
}
#endif

