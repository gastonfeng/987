/*==============================*/
/*	数据采集器程序		*/
/*	99.9第二版		*/
/*	用户:西藏		*/
/*	文件名:tibet.C		*/
/*==============================*/
#include <reg552.h>
#include <stdio.h>
#include <stdarg.h>
#include <absacc.h>
#include <math.h>
#include "ASCII.H"
#include "Ha.h"

typedef struct{
    char type;              /*  型式标志                */
                            /*  0=数值                  */
                            /*  1=字符串选项            */
                            /*  2=时钟                  */
                            /*  3=日期                  */
    char title[32];         /*  提示信息字符串        */
    int max,min;            /*  最大最小值                  */
    char **option;          /*  字符串选项的显示指针        */
    unsigned char *value;
    unsigned char *value1;
    unsigned char *value2;
}setup_index;               /*  设置所需要的信息结构        */
#define CLOCK 2
#define DATE    3

/*----------------------------------------------*/
/*每天纪录数据,占用322字节X98天=31566字节.	*/
/*----------------------------------------------*/
typedef struct{
	unsigned int year;
	unsigned char month;
	unsigned char date;
	unsigned int BatteryVoltageAverage[24],RadioAverage[24],ChargeCurrentAverage[24],DischargeCurrentAverage[24];
	int TemperatureMaximum,TemperatureMinimum,TemperatureAverage[24];
	unsigned int BatteryVoltageMaximum,BatteryVoltageMinimum,RadioMaximum,RadioMinimum,ChargeCurrentMaximum,ChargeCurrentMinimum,DischargeCurrentMaximum,DischargeCurrentMinimum;
	unsigned int RadioTotal,ChargeTotal,DischargeTotal;
}recorddate;

/*----------------------------------------------*/
/*	月数据,占用9字节*12月*10年=1080字节.	*/
/*----------------------------------------------*/
typedef struct{
	unsigned char month;
	unsigned int year,RadioTotal,ChargeTotal,DischargeTotal;
}recordmonth;

/*--------------------------------------*/
/*	年数据,占用8字节*10年=80字节.	*/
/*--------------------------------------*/
typedef struct{
	unsigned int year;
	unsigned int RadioTotal,ChargeTotal,DischargeTotal;
}recordyear;

/*==============*/
/*  系统宏定义  */
/*==============*/
#define MODEL 10            /*机器代码  */
#define DELAY_SCALE 115     /*  延时子程序系数  */
#define WAITTIME 10*60       /*  不按键等待时间  */
#define COMP_ZERO   200
#define FLASH_TIME  50     /*  闪烁时间    */
#define INPUT_DELAY 10      /*  设置连加等待时间    */
#define LCD		1
#define PRINTER		0

/*------------------------------*/
/*  keyboard value table        */
/*------------------------------*/
#define NO  0xf
#define OK  0xd
#define ESC 0xe
#define INC 0xb
#define DEC 7

/*======================*/
/*	硬件接口定义	*/
/*======================*/

/*  LCD */
#define base            ((unsigned char *)0x29000)
#define WriteCommand    ((unsigned char *)0x29000)
#define ReadCommand ((unsigned char *)0x29008)
#define WriteData               ((unsigned char *)0x29004)
#define ReadData                ((unsigned char *)0x2900c)
sbit backlight=P4^4;
sbit LED1=P1^4;
sbit LED2=P1^5;

/*  DS12887A    */
#define DS12887     ((unsigned char *)0x28800)
#define Secondss     DS12887[0]
#define SecondsAlarm    DS12887[1]
#define Minutess     DS12887[2]
#define MinutesAlarm    DS12887[3]
#define Hourss       DS12887[4]
#define HoursAlarm  DS12887[5]
#define Weeks    DS12887[6]
#define Dates    DS12887[7]
#define Months   DS12887[8]
#define Years    DS12887[9]

/*		打印口定义		*/
#define PrnPort			(*(unsigned char *)0x02b000)
sbit 	Stb=P4^0;
sbit	Ack=P4^1;
sbit	Busy=P4^3;
sbit	Pgout=P4^2;

#define Used		(*(unsigned char *)0x020000)
#define PointDate 	(*(unsigned char *)0x020001)
#define PointMonth	(*(unsigned char *)0x020002)
#define PointYear	(*(unsigned char *)0x020003)
#define RecordDates	(*(unsigned char *)0x020004)
#define RecordMonths	(*(unsigned char *)0x020005)
#define RecordYears	(*(unsigned char *)0x020006)
#define Contrast	(*(unsigned char *)0x020007)
#define Prtbytimer	(*(unsigned char *)0x020008)
#define Prthours	(*(unsigned char *)0x020009)
#define Prtminutes	(*(unsigned char *)0x02000a)
#define Scale		((float *)0x02000b)
#define RecordDate	((recorddate *)0x020030)
#define RecordMonth	((recordmonth *)0x027000)
#define RecordYear	((recordyear *)0x027e00)




unsigned char x;				/*显示坐标的垂直坐标*/
unsigned char y;                 /*显示坐标的水平坐标*/

static bit first;
bit flag,ff;
bit pflag,round;
	code unsigned char *sprompt[]={"蓄电池电压系数：","辐射量系数：","温度计算系数：","风机电流系数","放电电流系数：","充电电流系数："};
	code float ScaleDefault[6]={3.9063,1.6540,1,0.4883,2.9297,1.9531};
	bit sflag;

bit intflag,mflag,mdisplay,Asciibold;    /*  中断标志    */
bit Prn,Device;
unsigned int time;
unsigned char Month,Date,Week,Hours,Minutes,Seconds;
unsigned char char_point=10; /*  设置变量指针    */
unsigned int Year;

code char *pmode[]={"否","是"};

    code setup_index prompt[]={
        {DATE,"当前日期:\n\n %b-%b-%d",0,0,NULL,&Months,&Dates,&Years},
        {CLOCK,"当前时间:\n\n %b:%b:%b",0,0,NULL,&Hourss,&Minutess,&Secondss},
        {0,"浏览历史数据",0,0,NULL,NULL,NULL,NULL},
        {0,"打印历史数据",0,0,NULL,NULL,NULL,NULL}
    };

unsigned int loop[6],Maximum[6],Minimum[6],Current[6],Average[6];
long memory[6];
void pputchar(unsigned char);

lcd_int(unsigned chip)
{
    while(ReadCommand[chip]>=0x80);
}

void clrscr(void)
{
    unsigned char chip,xx,yy;
    for(chip=0;chip<3;chip++){
        for(xx=0;xx<8;xx++){
            lcd_int(chip);
            WriteCommand[chip]=xx|0xb8;
            lcd_int(chip);
            WriteCommand[chip]=0x40;
            for(yy=0;yy<64;yy++){
                lcd_int(chip);
                WriteData[chip]=0;
            }
        }
        lcd_int(chip);
        WriteCommand[chip]=0xc0;
    }
    x=y=0;
}

/*液晶显示器单个汉字字符显示*/
void  lcd_char(unsigned char dot)  /*页地址,Y地址,字符点阵数组*/
{
    unsigned char i,chip,*p,buf[8],lf,j,l;
    static union{
    	unsigned char c[2];
    	unsigned int d;
    }c;
    if(dot=='\n'){
    	x=(x>=7)?0:x+1;
    	y=0;
    	return;
    }
    if(dot<=160){
    	first=1;
    	if(y>=128)chip=1;
		else{
			if(y>=64)chip=2;
			else chip=0;
		}
		p=ASCII+dot*8;
    	lcd_int(chip);
    	if(!Asciibold){
    		WriteCommand[chip]=x&7|0xb8;
    		lcd_int(chip);
    		WriteCommand[chip]=y&0x3f|0x40;
    		for(i=0;i<8;i++){
    			lcd_int(chip);
    			WriteData[chip]=*(p+i);
    		}
    		lcd_int(chip);
    		WriteCommand[chip]=0xc0;
    		y+=8;
    		if(y>=192){
    			y=0;
    			x=(x>=7)?0:x+1;
    		}
    		return;
    	}
    	WriteCommand[chip]=((x+1)&0x7)|0xb8;     /*页地址设置*/
    	lcd_int(chip);
    	WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=0;i<8;i++){
        lcd_int(chip);
        buf[i]=0;
        l=*p++;
        for(j=0;j<4;j++){
        	buf[i]<<=2;
			if(l>=0x80)buf[i]+=3;
			l<<=1;
        }
        WriteData[chip]=buf[i];
        for(j=0;j<4;j++){
        	buf[i]<<=2;
			if(l>=0x80)buf[i]+=3;
			l<<=1;
        }
    }
    lcd_int(chip);
    WriteCommand[chip]=(x&0x7)|0xb8;     /*页地址设置*/
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=0;i<8;i++){
        lcd_int(chip);
        WriteData[chip]=buf[i];
    }
    lcd_int(chip);
    WriteCommand[chip]=0xc0;
    	y=y+8;
    	if(y>=192){
        	y=0;
        	x=(x>=7)?0:x+2;
    	}
    	return;
    }
    if(first){
    	c.c[0]=dot;
    	first=0;
    	return;
    }
    first=1;
    c.c[1]=dot;
    for(i=0;i<sizeof(CHINESEINDEX);i++){
    	if(CHINESEINDEX[i]==c.d){
    		c.c[0]=i;
    		break;
    	}
    }
    lf=16;
    if(y>=128){
    	chip=1;
    	if(y>176){
    		y=0;
    		x=x>=7?0:x+2;
    		chip=0;
    	}
    }
	else{
        if(y>=64){
        	chip=2;
        	if(y>112)lf=128-y;
        }
			else{
				chip=0;
				if(y>48)lf=64-y;
			}
	}
    lcd_int(chip);
    WriteCommand[chip]=(x&7)|0xb8;     /*页地址设置*/
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=0;i<lf;i++){
    	lcd_int(chip);
        WriteData[chip]=CHINESEDOT[c.c[0]][i];
    }
    lcd_int(chip);
    WriteCommand[chip]=((x+1)&7)|0xb8;
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=16;i<16+lf;i++){
        lcd_int(chip);
        WriteData[chip]=CHINESEDOT[c.c[0]][i];
    }
    if(lf!=16){
    	if(y+16>=128)chip=1;
    	else{
    		if(y+16>=64)chip=2;
    		else chip=0;
    	}
	    lcd_int(chip);
	    WriteCommand[chip]=(x&7)|0xb8;     /*页地址设置*/
	    lcd_int(chip);
	    WriteCommand[chip]=0x40;
	    for(i=lf;i<16;i++){
	    	lcd_int(chip);
	        WriteData[chip]=CHINESEDOT[c.c[0]][i];
	    }
	    lcd_int(chip);
	    WriteCommand[chip]=((x+1)&7)|0xb8;
	    lcd_int(chip);
	    WriteCommand[chip]=0x40;
	    for(i=16+lf;i<32;i++){
	        lcd_int(chip);
	        WriteData[chip]=CHINESEDOT[c.c[0]][i];
	    }    	
    }	  		
    lcd_int(chip);
    WriteCommand[chip]=0xc0;
    y=y+16;
    if(y>=192){
        y=0;
        x=(x>=7)?0:x+2;
    }
}

gotoxy(char x1,char y1)
{
    x=x1;
    y=y1;
}

void LcdOpen()
{
	unsigned char chip;
    for(chip=0;chip<3;chip++){
    	lcd_int(chip);
    	WriteCommand[chip]=0x3f;
    	lcd_int(chip);
    	WriteCommand[chip]=0xc0;
    }
    clrscr();
    backlight=1;
}

void LcdClose()
{
	unsigned char chip;
    clrscr();
    for(chip=0;chip<3;chip++){
    	lcd_int(chip);
    	WriteCommand[chip]=0x3e;
    	lcd_int(chip);
    	WriteCommand[chip]=0xc0;
    }
    backlight=0;
}

void pprintf (unsigned char *format,...)
{
    unsigned char format_flag,*ptr,cc;
	unsigned int div_val,  basenumber,uu_val;
	int u_val;
    va_list ap;
    va_start(ap,format);
    cc=0;
    for (;;){
        while ((format_flag = *format++) != '%'){      /* Until '%' or '\0' */
            if (!format_flag){
                return;
            }
            else pputchar(format_flag);
        }
        switch (format_flag = *format++){
            case 'c':
                format_flag = va_arg(ap,int);
            default:
                pputchar(format_flag);
            continue;
            case 's':
                ptr = va_arg(ap,char *);
            while (format_flag = *ptr++){
                if(cc==char_point)pputchar(' ');
                else pputchar(format_flag);
            }
            continue;
            case 'b':
            	div_val=100;
            	u_val=va_arg(ap,unsigned char);
            	goto l1;
            case 'f':
            	uu_val=va_arg(ap,float)*10000;
				basenumber=10;
				pputchar(uu_val/10000+48);
				pputchar('.');         
				uu_val%=10000;
				div_val=1000;
				do{
					pputchar(uu_val/div_val+48);
					uu_val%=div_val;
					div_val/=basenumber;
				}while(div_val);
				continue;  	
            case 'o':
            case 'd':
                div_val = 10000;
            u_val = va_arg(ap,int);
l1:			basenumber = 10;
		if(u_val<0){
			pputchar('-');
			u_val=-u_val;
		}
		while (div_val > 1 && div_val > u_val){
                    div_val /= 10;
                }
                if(format_flag=='o' && div_val==1){
                    if(cc==char_point)pputchar(' ');
                    else pputchar('0');
                }
            do{
                if(format_flag=='o' && div_val==1){
                    if(cc==char_point)pputchar(' ');
                    else pputchar('.');
                }
                if(cc==char_point)pputchar(' ');
                else pputchar(u_val / div_val+48);
                u_val %= div_val;
                div_val /= basenumber;
            }
            while (div_val);
        }
        cc++;
    }
}

void delay(unsigned int time)
{
    unsigned int j;
    for(;time>0;time--){
        for(j=0;j<10;j++);
        PCON|=0x10;
        T3=0;
    }
}

unsigned char kbhit(void)
{
	PCON|=0x10;
	T3=0;
    if((P1&0x0f)==0xf)return 0;else return 1;
}

unsigned char getch()
{
    unsigned char j;
    PCON|=0x10;
    T3=0;
    j=P1&0x0f;
    if(j==0xf)return NO;
    delay(10);
    if(j==(P1&0xf)){
        time=0;
        return j;
    }
    return NO;
}

unsigned char wait_key()
{
    unsigned char k;
    while(getch()!=NO&&time<WAITTIME);
    while((k=getch())==NO && time<WAITTIME);
	return k;
}	

unsigned int ad(unsigned char ch)
{
	unsigned int rr;
	ADCON=ch;
	ADCON|=8;
	while(!(ADCON&0x10));
	rr=(unsigned int)(((float)(ADCH*256+ADCON)/64)*Scale[ch]);
	ADCON&=0xef;
	return rr;
}

void pputchar(unsigned char cc)
{
	unsigned char k;
	PCON|=0x10;
	T3=0;
	if(Device==LCD){
		lcd_char(cc);
		return;
	}
	if(Prn==0)return;
	while(Busy){   
		for(k=0;k<200;k++){
			if(!Busy)goto next;
			delay(10);
		}
		clrscr();
		Device=LCD;
		pprintf("打印机未准备好...\n\n按确定键继续.\n\n按设置键退出打印.");
		if((k=wait_key())==ESC||k==NO){Prn=0;Device=PRINTER;return;}
		clrscr();                  
		pprintf("正在打印...");
		Device=PRINTER;
	}    
next:	
	PrnPort=cc;
	Stb=0;               
	Stb=1;
}

void int0() interrupt 0			/*	硬时钟每秒中断一次	*/
{
	unsigned char ch;
	DS12887[0xc]=DS12887[0xc];
	LED1=~LED1;
	LED2=~LED2;
	time++;
	if(Secondss==0){	/*	整秒,每分钟操作一次,A/D采样并比较最大最小值	*/
		for(ch=0;ch<6;ch++){
			memory[ch]+=(Current[ch]=ad(ch));
			loop[ch]++;
			if(Maximum[ch]<Current[ch])Maximum[ch]=Current[ch];
			if(Minimum[ch]>Current[ch])Minimum[ch]=Current[ch];
		}
		RecordDate[PointDate].BatteryVoltageMaximum=Maximum[0];
		RecordDate[PointDate].BatteryVoltageMinimum=Minimum[0];
		RecordDate[PointDate].RadioMaximum=Maximum[1];
		RecordDate[PointDate].RadioMinimum=Minimum[1];
		RecordDate[PointDate].ChargeCurrentMaximum=Maximum[5];
		RecordDate[PointDate].ChargeCurrentMinimum=Minimum[5];
		RecordDate[PointDate].DischargeCurrentMaximum=Maximum[4];
		RecordDate[PointDate].DischargeCurrentMinimum=Minimum[4];
		RecordDate[PointDate].TemperatureMaximum=Maximum[2];
		RecordDate[PointDate].TemperatureMinimum=Minimum[2];
		if(Minutess==0){	/*	整分,每小时一次,计算记录每小时平均值	*/
			Average[0]=memory[0]/loop[0];
			Average[1]=memory[1]/loop[1];
			Average[2]=memory[2]/loop[2];
/*			Average[3]=memory[3]/loop[3];*/
			Average[4]=memory[4]/loop[4];
			Average[5]=memory[5]/loop[5];
			RecordDate[PointDate].BatteryVoltageAverage[Hours]=Average[0];
			RecordDate[PointDate].RadioAverage[Hours]=Average[1];
			RecordDate[PointDate].ChargeCurrentAverage[Hours]=Average[5];
			RecordDate[PointDate].DischargeCurrentAverage[Hours]=Average[4];
			RecordDate[PointDate].TemperatureAverage[Hours]=Average[2];
/*			RecordDate[PointDate].WindCurrentAverage[Hours]=Average[3];*/
			RecordDate[PointDate].ChargeTotal+=Average[5];
			RecordDate[PointDate].DischargeTotal+=Average[4];
			RecordDate[PointDate].RadioTotal+=Average[1];
			loop[0]=loop[1]=loop[2]=loop[3]=loop[4]=loop[5]=0;
			memory[0]=memory[1]=memory[2]=memory[3]=memory[4]=memory[5]=0;
			if(Hourss==0){	/*	整小时,每天一次,记录当天最大最小值	*/
/*				RecordDate[PointDate].WindCurrentMaximum=Maximum[3];
				RecordDate[PointDate].WindCurrentMinimum=Minimum[3];*/
				RecordMonth[PointMonth].RadioTotal+=RecordDate[PointDate].RadioTotal;	/*	月累计	*/
				RecordMonth[PointMonth].ChargeTotal+=(RecordDate[PointDate].ChargeTotal/10);
				RecordMonth[PointMonth].DischargeTotal+=(RecordDate[PointDate].DischargeTotal/10);
				PointDate++;
				RecordDate[PointDate].year=Years+1980;
				RecordDate[PointDate].month=Months;
				RecordDate[PointDate].date=Dates;
				for(ch=0;ch<6;ch++){
    					Current[ch]=0;
					loop[ch]=0;
					memory[ch]=0;
					Maximum[ch]=0;
					Minimum[ch]=20000;
					Average[ch]=0;
				}
				if(Dates==1){	/*	每月一日执行年数据累计	*/
					RecordYear[PointYear].RadioTotal+=(RecordMonth[PointMonth].RadioTotal/1000);
					RecordYear[PointYear].ChargeTotal+=(RecordMonth[PointMonth].ChargeTotal/1000);
					RecordYear[PointYear].DischargeTotal+=(RecordMonth[PointMonth].DischargeTotal/1000);
					PointMonth++;
					RecordMonth[PointMonth].year=Years;
					RecordMonth[PointMonth].month=Months;
				
				if(Months==1){ 	/*	一月一日	*/
						PointYear++;
						RecordYear[PointYear].year=Years+1980;
					}
				}
			}	
		}	
	}
	Seconds=Secondss;
	Minutes=Minutess;
	Hours=Hourss;
	Date=Dates;
	Month=Months;
	Year=Years+1980;
	Week=Weeks;
	intflag=1;
}

void setup(setup_index x)
{
    unsigned char flash=0,key_time;     /*flash:闪烁时间计时,key_time:按键时间计时*/
    unsigned char last_key;       /*last_key:记忆上次键值,char_point:时钟日期设置指针*/
    unsigned char p[3],i;
    unsigned int year,max,min;
    flag=1;
    p[0]=*x.value;
    p[1]=*x.value1;
    p[2]=*x.value2;
    if(x.type<2){
        max=x.max;
        min=x.min;
    }
    else{
        if(x.type==CLOCK){
            max=23;
            min=0;
        }
        else{
            max=12;
            min=1;
        }
    }
/*    pprintf(x.title,p[0],p[1],p[2]);*/
    while(kbhit());
    char_point=0;
    while(1){
    	year=p[2]+1980;
        if(flag){
            if(ff){
                i=char_point;
                char_point=3;
                clrscr();
                switch(x.type){
                    case 0:pprintf(x.title,p[0]);break;
                    case 1:pprintf(x.title,x.option[p[0]]);break;
                    case 2:pprintf(x.title,p[0],p[1]);break;
                    case 3:pprintf(x.title,p[0],p[1],year);break;
                }
                flag=0;
                ff=0;
                char_point=i;
            }
            else{
            	clrscr();
                switch(x.type){
                    case 0:pprintf(x.title,p[0]);break;
                    case 1:pprintf(x.title,x.option[0]);break;
                    case 2:pprintf(x.title,p[0],p[1]);break;
                    case 3:pprintf(x.title,p[0],p[1],year);break;
                }
                flag=0;
                ff=1;
            }
        }
        delay(50);
        if(time>WAITTIME)return;
        if(kbhit()){
            switch(getch()){
                case ESC:char_point=10;return;
                case OK:{
                    char_point=10;
                    *x.value=p[0];
                    *x.value1=p[1];
                    *x.value2=p[2];
                    return;
                }
                case INC:{
                    flag=ff=1;
                    if(last_key!=INC){
                        p[char_point]>=max?p[char_point]=min:p[char_point]++;
                        last_key=INC;
                        key_time=0;
                    }
                    else{
                        if(key_time>INPUT_DELAY){
                            p[char_point]>=max?p[char_point]=min:p[char_point]++;
                        }
                        else key_time++;
                    }
                    flash=0;
                    break;
                }
                case DEC:{
                    flag=ff=1;
                    if(x.type==CLOCK){
                        if(char_point==1){
                            char_point=0;
                            max=23;
                        }
                        else{
                            char_point=1;
                            max=59;
                        }
                        while(kbhit());
                        break;
                    }
                    if(x.type==DATE){
                        if(char_point>=2)char_point=0;else char_point++;
                        if(char_point==0){max=12;min=1;}
                        else{
                            if(char_point==1)max=31;
                            else {max=99;min=0;}
                        }
                        while(kbhit());
                        break;
                    }
                    if(last_key!=DEC){
                        p[char_point]<=min?p[char_point]=max:p[char_point]--;
                        last_key=DEC;
                        key_time=0;
                    }
                    else{
                        if(key_time>INPUT_DELAY){
                            p[char_point]<=min?p[char_point]=max:p[char_point]--;
                        }
                        else key_time++;
                    }
                    flash=0;
                    break;
                }
                default:last_key=ESC;key_time=0;
            }
        }
        else{
            last_key=ESC;key_time=0;
        }
        if(flash>FLASH_TIME){flash=0;flag=1;}
        else flash++;
    }
}

void viewhistory(unsigned char item)
{
	unsigned char month,i,date,start,frame;
	unsigned int year;
	switch(item){
		case 0:{
			year=PointYear;
			/*year=0;
			while(1){
				clrscr();
				pprintf("请选择年度:");
				pprintf("%d年",RecordYear[year]);
				switch(wait_key()){
					case INC:year=(year>=PointYear)?0:year+1;break;
					case DEC:year=(year<=0)?PointYear:year-1;break;
					case OK:{*/
						while(1){
							clrscr();
							pprintf("%d年",RecordYear[year].year);
							pprintf("\n\n辐射量:%dkW",RecordYear[year].RadioTotal);
							pprintf("\n\n充电电量:%dkAH",RecordYear[year].ChargeTotal);
							pprintf("\n\n放电电量:%dkAH",RecordYear[year].DischargeTotal);
							switch(wait_key()){
								case INC:year=(year>=PointYear)?0:year+1;break;
								case DEC:year=(year<=0)?PointYear:year-1;break;
								default:return;
							}
						}/*
						break;
					}
					default:return;
				}		
			}*/
		}
		case 1:{
			year=RecordMonth[0].year;
			month=RecordMonth[0].month;
			/*while(1){
				clrscr();
				pprintf("请选择年度:");
				pprintf("%d",year);
				switch(wait_key()){
					case INC:year=(year>=RecordMonth[PointMonth].year)?RecordMonth[0].year:year+1;break;
					case DEC:year=(year<=RecordMonth[0].year)?RecordMonth[PointMonth].year:year-1;break;
					case OK:{
						while(1){
							clrscr();
							pprintf("%d年",year);
							pprintf("\n\n请选择月份:%b",month);
							switch(wait_key()){
								case INC:month=(month>=12)?1:month+1;break;
								case DEC:month=(month<=1)?12:month-1;break;
								case OK:{*/
									while(1){
										clrscr();
										for(i=0;i<=PointMonth;i++){
											if((RecordMonth[i].year==year) &&(RecordMonth[i].month==month))break;
										}
										if(i>PointMonth){
											pprintf("无指定时段数据!");
											wait_key();
											return;
										}
										pprintf("%d年%b月",RecordMonth[i].year,RecordMonth[i].month);
										pprintf("\n\n辐射量:%dW",RecordMonth[i].RadioTotal);
										pprintf("\n\n充电电量:%dAH",RecordMonth[i].ChargeTotal);
										pprintf("\n\n放电电量:%dAH",RecordMonth[i].DischargeTotal);
										switch(wait_key()){
											case INC:i=(i>=PointMonth)?0:i+1;break;
											case DEC:i=(i<=0)?PointMonth:i-1;break;
											default:return;
										}
									}/*
									break;
								}
								default:return;
							}
						}
					}
					default:return;
				}		
			}*/
		}
		case 2:{
			if(RecordDate[PointDate+1].year==0)start=0;
			else start=PointDate+1;
			year=RecordDate[PointDate].year;
			month=RecordDate[PointDate].month;
			date=RecordDate[PointDate].date;
			/*while(1){
				clrscr();
				pprintf("请选择年度:%d",year);
				switch(wait_key()){
					case INC:year=(year>=RecordDate[PointDate].year)?RecordDate[start].year:year+1;break;
					case DEC:year=(year<=RecordDate[start].year)?RecordDate[PointDate].year:year-1;break;
					case OK:{
						while(1){
							clrscr();
							pprintf("请选择年度:%d",year);
							pprintf("\n\n月份:%b",month);
							switch(wait_key()){
								case INC:month=(month>=12)?1:month+1;break;
								case DEC:month=(month<=1)?12:month-1;break;
								case OK:{
									while(1){
										clrscr();
										pprintf("%d年",year);
										pprintf("\n\n%b月",month);
										pprintf("\n\n%b日",date);
										switch(wait_key()){
											case INC:date=(date>=31)?1:date+1;break;
											case DEC:date=(date<=1)?31:date-1;break;
											case OK:{*/
												while(1){
													clrscr();
													for(i=0;i<=99;i++){
														if((RecordDate[i].year==year)&&(RecordDate[i].month==month)&&(RecordDate[i].date==date))break;
													}
													if(i>99){
														pprintf("没有指定数据!");
														wait_key();
														return;
													}
													frame=0;
													while(1){
														clrscr();
														Asciibold=0;
														if(frame<8){
															pprintf("%b-%b-%d Average",RecordDate[i].month,RecordDate[i].date,RecordDate[i].year);
															gotoxy(1,0);
															pprintf("Time: %b:00 %b:00 %b:00",frame*3+1,frame*3+2,frame*3+3);
															gotoxy(2,0);
															pprintf("Batt: %o %o %oV",RecordDate[i].BatteryVoltageAverage[frame*3],RecordDate[i].BatteryVoltageAverage[frame*3+1],RecordDate[i].BatteryVoltageAverage[frame*3+2]);
															gotoxy(3,0);
															pprintf("Radi: %d %d %dW",RecordDate[i].RadioAverage[frame*3],RecordDate[i].RadioAverage[frame*3+1],RecordDate[i].RadioAverage[frame*3+2]);
															gotoxy(4,0);
															pprintf("Char: %o %o %oA",RecordDate[i].ChargeCurrentAverage[frame*3],RecordDate[i].ChargeCurrentAverage[frame*3+1],RecordDate[i].ChargeCurrentAverage[frame*3+2]);
															gotoxy(5,0);
															pprintf("Disc: %o %o %oA",RecordDate[i].DischargeCurrentAverage[frame*3],RecordDate[i].DischargeCurrentAverage[frame*3+1],RecordDate[i].DischargeCurrentAverage[frame*3+2]);
															gotoxy(6,0);
															pprintf("Temp: %o %o %o",RecordDate[i].TemperatureAverage[frame*3]-200,RecordDate[i].TemperatureAverage[frame*3+1]-200,RecordDate[i].TemperatureAverage[frame*3+2]-200);
															/*gotoxy(7,0);
															pprintf("Wind: %oA %oA %oA",RecordDate[i].WindCurrentAverage[frame*3],RecordDate[i].WindCurrentAverage[frame*3+1],RecordDate[i].WindCurrentAverage[frame*3+2]);*/
														}
														else{
															pprintf("%b-%b-%d Maximum&Minimum",RecordDate[i].month,RecordDate[i].date,RecordDate[i].year);
															gotoxy(1,0);
															pprintf("Batt: %o %oV",RecordDate[i].BatteryVoltageMaximum,RecordDate[i].BatteryVoltageMinimum);
															gotoxy(2,0);
															pprintf("Radi: %d %dW",RecordDate[i].RadioMaximum,RecordDate[i].RadioMinimum);
															gotoxy(3,0);
															pprintf("Char: %o %oA",RecordDate[i].ChargeCurrentMaximum,RecordDate[i].ChargeCurrentMinimum);
															gotoxy(4,0);
															pprintf("Disc: %o %oA",RecordDate[i].DischargeCurrentMaximum,RecordDate[i].DischargeCurrentMinimum);
															gotoxy(5,0);
															pprintf("Temp: %o %o",RecordDate[i].TemperatureMaximum-200,RecordDate[i].TemperatureMinimum-200);
															/*gotoxy(6,0);
															pprintf("Wind: %oA %oA",RecordDate[i].WindCurrentMaximum,RecordDate[i].WindCurrentMinimum);*/
														}
														Asciibold=1;
														switch(wait_key()){
															case INC:i=(i>=((start==0)?PointDate:99))?0:i+1;break;
															case DEC:i=(i<=0)?((start==0)?PointDate:99):i-1;break;
															case OK:frame=(frame>=8)?0:frame+1;break;
															default:return;
														}
													}
												}/*
											}
											default:return;
										}
									}
									default:return;
								}
							}
						}
					}
				}	
			}*/
		}
	}
}

void view(void)
{
	unsigned char j,item=0,*prompt[]={"浏览年度数据","浏览月份数据","浏览每天数据"};
	while(1){
		clrscr();
		pprintf("历史数据浏览");
		for(j=0;j<3;j++){
			pprintf("\n\n");
			if(j==item)pprintf(">>");
				else pprintf("  ");
			pprintf("%s",prompt[j]);
		}
		switch(wait_key()){
			case INC:item=(item>=2)?0:item+1;
				break;
			case DEC:item=(item<=0)?2:item-1;
				break;
			case OK:viewhistory(item);
				return;
			default:return;
		}
	}				
}


void printhis(void)
{
	unsigned char i,start,end,h;
	Prn=1;
	pflag=1;
	if(RecordDate[99].year==0)round=0;else round=1;
	start=end=(PointDate==0)?0:PointDate-1;
	while(pflag){
		clrscr();
		pprintf("历史数据打印\n\n起始日期:%d年%b月%b日",RecordDate[start].year,RecordDate[start].month,RecordDate[start].date);
		switch(wait_key()){
			case INC:{
				start++;
				if(round){
					if(start==100)start=0;
				}
				else{
					if(start>PointDate)start=0;
				}
				break;
			}
			case DEC:{
				if(round){
					start=(start>0)?start-1:99;
				}
				else{
					start=(start>0)?start-1:PointDate;
				}
				break;
			}
			case OK:pflag=0;break;
			default:return;
		}
	}
	pflag=1;
	end=start;
	while(pflag){
		clrscr();
		pprintf("历史数据打印\n\n起始日期:%d年%b月%b日\n\n结束日期:%d年%b月%b日",RecordDate[start].year,RecordDate[start].month,RecordDate[start].date,RecordDate[end].year,RecordDate[end].month,RecordDate[end].date);
		switch(wait_key()){
			case INC:{
				if(round){
					if(start>PointDate){
						if(end==99)end=0;
						else{
							if(end==PointDate)end=start;
							else end++;
						}
					}
					else end=(end==PointDate)?start:end+1;
				}
				else end=(end==PointDate)?start:end+1;
				break;
			}
			case DEC:{
				if(round){
					if(start>PointDate){
						if(end==0)end=99;
						else{
							if(end==start)end=PointDate;
							else end--;
						}
					}
					else end=(end==start)?PointDate:end-1;
				}
				else end=(end==start)?PointDate:end-1;
				break;
			}
			case OK:pflag=0;break;
			default:return;
		}
	}           
	clrscr();
	pprintf("正在打印...");
	Device=PRINTER;
	pprintf("\t\t历史数据打印\n\r");
	i=start;
	while(1){
		pprintf("\n\r%d年%b月%b日\n\r",RecordDate[i].year,RecordDate[i].month,RecordDate[i].date);
		pprintf("时间\t辐射量\t蓄电池电压\t充电电流\t放电电流\t温度\n\r");
		for(h=0;h<24;h++)pprintf("%b:00\t%dW\t%oV\t%oA\t%oA\t%o℃\n\r",h+1,RecordDate[i].RadioAverage[h],RecordDate[i].BatteryVoltageAverage[h],RecordDate[i].ChargeCurrentAverage[h],RecordDate[i].DischargeCurrentAverage[h],RecordDate[i].TemperatureAverage[h]-200);
		pprintf("当天最大值\t%dW\t%oV\t%oA\t%oA\t%o℃\n\r",RecordDate[i].RadioMaximum,RecordDate[i].BatteryVoltageMaximum,RecordDate[i].ChargeCurrentMaximum,RecordDate[i].DischargeCurrentMaximum,RecordDate[i].TemperatureMaximum-200);
		pprintf("当天最小值\t%dW\t%oV\t%oA\t%oA\t%o℃\n\r",RecordDate[i].RadioMinimum,RecordDate[i].BatteryVoltageMinimum,RecordDate[i].ChargeCurrentMinimum,RecordDate[i].DischargeCurrentMinimum,RecordDate[i].TemperatureMinimum-200);
		pprintf("充电电量:%oAh\t放电电量:%oAh\t辐射量:%dW\n\r",RecordDate[i].ChargeTotal,RecordDate[i].DischargeTotal,RecordDate[i].RadioTotal);
		if(start<=end){
			i++;
			if(i>end)break;
		}
		else{
			if(i==end)break;
			i=i>=99?0:i+1;
		}
	}         
	pprintf("\n\r打印时间：%d年%b月%b日%b时%b分\n\r",Year,Month,Date,Hours,Minutes);
}


void setuploop(void)
{
    unsigned int k,i;
    i=0;
    while(1){
    	clrscr();
        if(prompt[i].type==1)pprintf(prompt[i].title,prompt[i].option[*prompt[i].value]);else
        pprintf(prompt[i].title,*prompt[i].value,*prompt[i].value1,(prompt[i].type==DATE)?*prompt[i].value2+1980:*prompt[i].value2);
        k=wait_key();
        switch (k){
            case INC:i++;if(i>3)i=0;break;
            case DEC:if(i>0)i--;else i=3;break;
            case OK:{                
            	
                if(i==2){view();i=2;break;};
                if(i==3){printhis();i=3;Device=LCD;break;};
                setup(prompt[i]);break;
            }
            default:return;
        }
    }
}

                                  
void systemsetup(void)
{                              
	unsigned char i;                      
	union{
		float value;
		unsigned int j;
	}u;  
	pprintf("系统参数设置\n\n按确定键继续\n\n其它键退出\n\n注意：需要专业人员调整！");
	if(wait_key()!=OK)return;
	for(i=0;i<6;i++){
		u.value=Scale[i];
		sflag=1;
		while(sflag){
			clrscr();
			pprintf("%s%f\n\n按加减键调整\n\n按设置键恢复缺省值\n\n按确定键保存",sprompt[i],u.value);
			switch(wait_key()){
				case ESC:u.value=ScaleDefault[i];break;
				case OK:Scale[i]=u.value;sflag=0;break;
				case INC:{
					u.value+=0.0001;
					gotoxy(0,0);
					pprintf("%s%f\n\n按加减键调整\n\n按设置键恢复缺省值\n\n按确定键保存",sprompt[i],u.value);
					delay(500);
					while(getch()==INC){
						u.value+=0.0001;
						gotoxy(0,0);
						pprintf("%s%f\n\n按加减键调整\n\n按设置键恢复缺省值\n\n按确定键保存",sprompt[i],u.value);
						PCON|=0x10;
						T3=0;
						delay(100);
					}
					break;
				}
				case DEC:{
					u.value-=0.0001;
					gotoxy(0,0);
					pprintf("%s%f\n\n按加减键调整\n\n按设置键恢复缺省值\n\n按确定键保存",sprompt[i],u.value);
	                delay(500);
	                while(getch()==DEC){
	                	u.value-=0.0001;
	                	gotoxy(0,0);
						pprintf("%s%f\n\n按加减键调整\n\n按设置键恢复缺省值\n\n按确定键保存",sprompt[i],u.value);
						PCON|=0x10;
						T3=0;
	                	delay(100);
	                }
	                break;
	            }
	        }                                 
	    }
	}
	clrscr();
	pprintf("格式化数据存储区？\n\n按确定键确认\n\n按其它键退出");
	if(wait_key()!=OK)return;
	clrscr();
	pprintf("确定要格式化数据区？\n\n按确定键确认\n\n按其它键退出");
	if(wait_key()!=OK)return;
	clrscr();
	pprintf("正在格式化数据区...\n\n请等候...");
	for(u.j=0x30;u.j<32768;u.j++){
		XBYTE[u.j]=0;
		PCON|=0x10;
		T3=0;
	}
    PointYear=PointDate=PointMonth=0;
    RecordYear[PointYear].year=RecordMonth[PointMonth].year=RecordDate[PointDate].year=Years+1980;
    RecordMonth[PointMonth].month=RecordDate[PointDate].month=Months;
    RecordDate[PointDate].date=Dates;                                  
	Used=0x55;
	clrscr();
}                                  
                                  
void main(void)
{
	union {
    	unsigned char frame;
    	unsigned int j;
    }m;
    unsigned char k;
    bit keypress;
    Device=LCD;
    Asciibold=1;
    char_point=10;
    lcd_char('a');
    LcdOpen();                                    
    if(getch()==ESC)systemsetup();        
    clrscr();
    pprintf("SJC-2型智能数据采集器");
    delay(10000);
    if(Used!=0x55){
/*    	pprintf("\n\n正在进行数据区格式化,\n\n请等待...");
    	PointYear=PointDate=PointMonth=0;
    	for(m.j=0;m.j<32768;m.j++){
    		XBYTE[m.j]=0;
    		PCON|=0x10;
    		T3=0;
    	}
    	RecordYear[PointYear].year=RecordMonth[PointMonth].year=RecordDate[PointDate].year=Years+1980;
    	RecordMonth[PointMonth].month=RecordDate[PointDate].month=Months;
    	RecordDate[PointDate].date=Dates;                                  
    	Scale[0]=ScaleDefault[0];
    	Scale[1]=ScaleDefault[1];
    	Scale[2]=ScaleDefault[2];
    	Scale[3]=ScaleDefault[3];
    	Scale[4]=ScaleDefault[4];
    	Scale[5]=ScaleDefault[5];
    	Used=0x55;      
    	clrscr();
    	pprintf("格式化完成！\n\n按任意键继续...");*/
    	clrscr();
    	pprintf("数据区有错误!\n\n请执行系统初始化.");
    	wait_key();
    	clrscr();
    }
    if(RecordYear[PointYear].year!=(Years+1980)){
    	PointYear++;
    	RecordYear[PointYear].year=(Years+1980);
    }
    if((RecordMonth[PointMonth].year!=(Years+1980))||(RecordMonth[PointMonth].month!=Months)){
    	PointMonth++;
    	RecordMonth[PointMonth].year=Years+1980;
    	RecordMonth[PointMonth].month=Months;
    }
    if((RecordDate[PointDate].year!=(Years+1980))||(RecordDate[PointDate].month!=Months)||(RecordDate[PointDate].date!=Dates)){
    	PointDate++;
    	RecordDate[PointDate].year=Years+1980;
    	RecordDate[PointDate].month=Months;
    	RecordDate[PointDate].date=Dates;
    }
	DS12887[0xa]=0x20;
    DS12887[0xb]=0x16;
    for(k=0;k<6;k++){
    	Current[k]=0;
		loop[k]=0;
		memory[k]=0;
		Maximum[k]=0;
		Minimum[k]=20000;
		Average[k]=0;
	}
    EX0=EA=1;
    PX0=1;
    m.frame=time=0;
    mdisplay=mflag=1;
    clrscr();
    while(1){
    	PCON|=0x10;
    	T3=0;
        if(!mdisplay&&getch()!=NO){
            LcdOpen();
            mdisplay=1;
        }
        if(time>WAITTIME){
            mdisplay=0;
            LcdClose();
        }
        if(mdisplay){
            switch(getch()){
                case OK:{
                    mflag=1;m.frame=0;
                    clrscr();
                    keypress=1;
                    break;
                }
                case INC:mflag=1;m.frame++;if(m.frame==6)m.frame=0;clrscr();keypress=1;break;
                case DEC:mflag=1;if(m.frame==0)m.frame=5;else m.frame--;clrscr();keypress=1;break;
                case ESC:setuploop();mflag=1;keypress=1;break;
            }
            if(mflag){
            	gotoxy(0,0);
                switch(m.frame){
                    case 0:{
                    	/*clrscr();*/
                    	pprintf("当前时间日期");
                    	pprintf("\n\n%b时%b分%b秒    ",Hours,Minutes,Seconds);
                    	pprintf("\n\n%d年%b月%b日    ",Year,Month,Date);
                    	/*pprintf("\n\n%s%s",msg7,w[Week]);*/
                    	break;
                    }	
                    case 1:{
                    	/*clrscr();*/
                    	pprintf("蓄电池电压     ");
                    	pprintf("\n\n当前值:%oV    ",ad(0));
                        /*pprintf("\n\n%s%oV",msg17,Average[0]);*/
                   		pprintf("\n\n今日最大值:%oV    \n\n今日最小值:%oV    ",Maximum[0],Minimum[0]);
                    	break;
                    }
                    case 2:{
                    	/*clrscr();*/
                    	pprintf("充电电流:     ");
                    	pprintf("\n\n当前值:%oA    ",ad(5));
                    	/*pprintf("\n\n%s%oA",msg17,Average[1]);*/
                    	pprintf("\n\n今日最大值:%oA    \n\n今日最小值:%oA    ",Maximum[5],Minimum[5]);
                    	break;
                    }
                    case 3:{
                    	/*clrscr();*/
                    	pprintf("放电电流:     ");
                    	pprintf("\n\n当前值:%oA    ",ad(4));
                    	/*pprintf("\n\n%s%oA",msg17,Average[2]);*/
                    	pprintf("\n\n今日最大值:%oA    \n\n今日最小值:%oA    ",Maximum[4],Minimum[4]);
                    	break;
                    }
                    case 4:{
                    	/*clrscr();*/
                    	pprintf("辐射量:     ");
                    	pprintf("\n\n当前值:%dW/m    ",ad(1));
                    	/*pprintf("\n\n%s%dW/m",msg17,Average[3]);*/
                    	pprintf("\n\n今日最大值:%dW/m    \n\n今日最小值:%dW/m    ",Maximum[1],Minimum[1]);
                    	break;
                    }
                    case 5:{
                    	/*clrscr();*/
                    	pprintf("环境温度:      ");
                    	pprintf("\n\n当前值:%o℃    ",ad(2)-200);
                    	/*pprintf("\n\n%s%o",msg17,Average[4]);*/
                    	pprintf("\n\n今日最大值:%o℃    \n\n今日最小值:%o℃    ",Maximum[2]-200,Minimum[2]-200);
                    	break;
                    }
                }
                mflag=0;
                if(keypress){
	                for(k=0;k<250;k++){
	                	if(!kbhit())break;
	                	delay(10);
	                }
	                keypress=0;
	        }
            }
            if(intflag){
                intflag=0;
                mflag=1;
            }
            PCON|=0x10;
            T3=0;
        }
    }
}

