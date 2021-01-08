/*==============================*/
/*	���ݲɼ�������		*/
/*	99.9�ڶ���		*/
/*	�û�:����		*/
/*	�ļ���:tibet.C		*/
/*==============================*/
#include <reg552.h>
#include <stdio.h>
#include <stdarg.h>
#include <absacc.h>
#include <math.h>
#include "ASCII.H"
#include "Ha.h"

typedef struct{
    char type;              /*  ��ʽ��־                */
                            /*  0=��ֵ                  */
                            /*  1=�ַ���ѡ��            */
                            /*  2=ʱ��                  */
                            /*  3=����                  */
    char title[32];         /*  ��ʾ��Ϣ�ַ���        */
    int max,min;            /*  �����Сֵ                  */
    char **option;          /*  �ַ���ѡ�����ʾָ��        */
    unsigned char *value;
    unsigned char *value1;
    unsigned char *value2;
}setup_index;               /*  ��������Ҫ����Ϣ�ṹ        */
#define CLOCK 2
#define DATE    3

/*----------------------------------------------*/
/*ÿ���¼����,ռ��322�ֽ�X98��=31566�ֽ�.	*/
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
/*	������,ռ��9�ֽ�*12��*10��=1080�ֽ�.	*/
/*----------------------------------------------*/
typedef struct{
	unsigned char month;
	unsigned int year,RadioTotal,ChargeTotal,DischargeTotal;
}recordmonth;

/*--------------------------------------*/
/*	������,ռ��8�ֽ�*10��=80�ֽ�.	*/
/*--------------------------------------*/
typedef struct{
	unsigned int year;
	unsigned int RadioTotal,ChargeTotal,DischargeTotal;
}recordyear;

/*==============*/
/*  ϵͳ�궨��  */
/*==============*/
#define MODEL 10            /*��������  */
#define DELAY_SCALE 115     /*  ��ʱ�ӳ���ϵ��  */
#define WAITTIME 10*60       /*  �������ȴ�ʱ��  */
#define COMP_ZERO   200
#define FLASH_TIME  50     /*  ��˸ʱ��    */
#define INPUT_DELAY 10      /*  �������ӵȴ�ʱ��    */
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
/*	Ӳ���ӿڶ���	*/
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

/*		��ӡ�ڶ���		*/
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




unsigned char x;				/*��ʾ����Ĵ�ֱ����*/
unsigned char y;                 /*��ʾ�����ˮƽ����*/

static bit first;
bit flag,ff;
bit pflag,round;
	code unsigned char *sprompt[]={"���ص�ѹϵ����","������ϵ����","�¶ȼ���ϵ����","�������ϵ��","�ŵ����ϵ����","������ϵ����"};
	code float ScaleDefault[6]={3.9063,1.6540,1,0.4883,2.9297,1.9531};
	bit sflag;

bit intflag,mflag,mdisplay,Asciibold;    /*  �жϱ�־    */
bit Prn,Device;
unsigned int time;
unsigned char Month,Date,Week,Hours,Minutes,Seconds;
unsigned char char_point=10; /*  ���ñ���ָ��    */
unsigned int Year;

code char *pmode[]={"��","��"};

    code setup_index prompt[]={
        {DATE,"��ǰ����:\n\n %b-%b-%d",0,0,NULL,&Months,&Dates,&Years},
        {CLOCK,"��ǰʱ��:\n\n %b:%b:%b",0,0,NULL,&Hourss,&Minutess,&Secondss},
        {0,"�����ʷ����",0,0,NULL,NULL,NULL,NULL},
        {0,"��ӡ��ʷ����",0,0,NULL,NULL,NULL,NULL}
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

/*Һ����ʾ�����������ַ���ʾ*/
void  lcd_char(unsigned char dot)  /*ҳ��ַ,Y��ַ,�ַ���������*/
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
    	WriteCommand[chip]=((x+1)&0x7)|0xb8;     /*ҳ��ַ����*/
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
    WriteCommand[chip]=(x&0x7)|0xb8;     /*ҳ��ַ����*/
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
    WriteCommand[chip]=(x&7)|0xb8;     /*ҳ��ַ����*/
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
	    WriteCommand[chip]=(x&7)|0xb8;     /*ҳ��ַ����*/
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
		pprintf("��ӡ��δ׼����...\n\n��ȷ��������.\n\n�����ü��˳���ӡ.");
		if((k=wait_key())==ESC||k==NO){Prn=0;Device=PRINTER;return;}
		clrscr();                  
		pprintf("���ڴ�ӡ...");
		Device=PRINTER;
	}    
next:	
	PrnPort=cc;
	Stb=0;               
	Stb=1;
}

void int0() interrupt 0			/*	Ӳʱ��ÿ���ж�һ��	*/
{
	unsigned char ch;
	DS12887[0xc]=DS12887[0xc];
	LED1=~LED1;
	LED2=~LED2;
	time++;
	if(Secondss==0){	/*	����,ÿ���Ӳ���һ��,A/D�������Ƚ������Сֵ	*/
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
		if(Minutess==0){	/*	����,ÿСʱһ��,�����¼ÿСʱƽ��ֵ	*/
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
			if(Hourss==0){	/*	��Сʱ,ÿ��һ��,��¼���������Сֵ	*/
/*				RecordDate[PointDate].WindCurrentMaximum=Maximum[3];
				RecordDate[PointDate].WindCurrentMinimum=Minimum[3];*/
				RecordMonth[PointMonth].RadioTotal+=RecordDate[PointDate].RadioTotal;	/*	���ۼ�	*/
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
				if(Dates==1){	/*	ÿ��һ��ִ���������ۼ�	*/
					RecordYear[PointYear].RadioTotal+=(RecordMonth[PointMonth].RadioTotal/1000);
					RecordYear[PointYear].ChargeTotal+=(RecordMonth[PointMonth].ChargeTotal/1000);
					RecordYear[PointYear].DischargeTotal+=(RecordMonth[PointMonth].DischargeTotal/1000);
					PointMonth++;
					RecordMonth[PointMonth].year=Years;
					RecordMonth[PointMonth].month=Months;
				
				if(Months==1){ 	/*	һ��һ��	*/
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
    unsigned char flash=0,key_time;     /*flash:��˸ʱ���ʱ,key_time:����ʱ���ʱ*/
    unsigned char last_key;       /*last_key:�����ϴμ�ֵ,char_point:ʱ����������ָ��*/
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
				pprintf("��ѡ�����:");
				pprintf("%d��",RecordYear[year]);
				switch(wait_key()){
					case INC:year=(year>=PointYear)?0:year+1;break;
					case DEC:year=(year<=0)?PointYear:year-1;break;
					case OK:{*/
						while(1){
							clrscr();
							pprintf("%d��",RecordYear[year].year);
							pprintf("\n\n������:%dkW",RecordYear[year].RadioTotal);
							pprintf("\n\n������:%dkAH",RecordYear[year].ChargeTotal);
							pprintf("\n\n�ŵ����:%dkAH",RecordYear[year].DischargeTotal);
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
				pprintf("��ѡ�����:");
				pprintf("%d",year);
				switch(wait_key()){
					case INC:year=(year>=RecordMonth[PointMonth].year)?RecordMonth[0].year:year+1;break;
					case DEC:year=(year<=RecordMonth[0].year)?RecordMonth[PointMonth].year:year-1;break;
					case OK:{
						while(1){
							clrscr();
							pprintf("%d��",year);
							pprintf("\n\n��ѡ���·�:%b",month);
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
											pprintf("��ָ��ʱ������!");
											wait_key();
											return;
										}
										pprintf("%d��%b��",RecordMonth[i].year,RecordMonth[i].month);
										pprintf("\n\n������:%dW",RecordMonth[i].RadioTotal);
										pprintf("\n\n������:%dAH",RecordMonth[i].ChargeTotal);
										pprintf("\n\n�ŵ����:%dAH",RecordMonth[i].DischargeTotal);
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
				pprintf("��ѡ�����:%d",year);
				switch(wait_key()){
					case INC:year=(year>=RecordDate[PointDate].year)?RecordDate[start].year:year+1;break;
					case DEC:year=(year<=RecordDate[start].year)?RecordDate[PointDate].year:year-1;break;
					case OK:{
						while(1){
							clrscr();
							pprintf("��ѡ�����:%d",year);
							pprintf("\n\n�·�:%b",month);
							switch(wait_key()){
								case INC:month=(month>=12)?1:month+1;break;
								case DEC:month=(month<=1)?12:month-1;break;
								case OK:{
									while(1){
										clrscr();
										pprintf("%d��",year);
										pprintf("\n\n%b��",month);
										pprintf("\n\n%b��",date);
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
														pprintf("û��ָ������!");
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
	unsigned char j,item=0,*prompt[]={"����������","����·�����","���ÿ������"};
	while(1){
		clrscr();
		pprintf("��ʷ�������");
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
		pprintf("��ʷ���ݴ�ӡ\n\n��ʼ����:%d��%b��%b��",RecordDate[start].year,RecordDate[start].month,RecordDate[start].date);
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
		pprintf("��ʷ���ݴ�ӡ\n\n��ʼ����:%d��%b��%b��\n\n��������:%d��%b��%b��",RecordDate[start].year,RecordDate[start].month,RecordDate[start].date,RecordDate[end].year,RecordDate[end].month,RecordDate[end].date);
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
	pprintf("���ڴ�ӡ...");
	Device=PRINTER;
	pprintf("\t\t��ʷ���ݴ�ӡ\n\r");
	i=start;
	while(1){
		pprintf("\n\r%d��%b��%b��\n\r",RecordDate[i].year,RecordDate[i].month,RecordDate[i].date);
		pprintf("ʱ��\t������\t���ص�ѹ\t������\t�ŵ����\t�¶�\n\r");
		for(h=0;h<24;h++)pprintf("%b:00\t%dW\t%oV\t%oA\t%oA\t%o��\n\r",h+1,RecordDate[i].RadioAverage[h],RecordDate[i].BatteryVoltageAverage[h],RecordDate[i].ChargeCurrentAverage[h],RecordDate[i].DischargeCurrentAverage[h],RecordDate[i].TemperatureAverage[h]-200);
		pprintf("�������ֵ\t%dW\t%oV\t%oA\t%oA\t%o��\n\r",RecordDate[i].RadioMaximum,RecordDate[i].BatteryVoltageMaximum,RecordDate[i].ChargeCurrentMaximum,RecordDate[i].DischargeCurrentMaximum,RecordDate[i].TemperatureMaximum-200);
		pprintf("������Сֵ\t%dW\t%oV\t%oA\t%oA\t%o��\n\r",RecordDate[i].RadioMinimum,RecordDate[i].BatteryVoltageMinimum,RecordDate[i].ChargeCurrentMinimum,RecordDate[i].DischargeCurrentMinimum,RecordDate[i].TemperatureMinimum-200);
		pprintf("������:%oAh\t�ŵ����:%oAh\t������:%dW\n\r",RecordDate[i].ChargeTotal,RecordDate[i].DischargeTotal,RecordDate[i].RadioTotal);
		if(start<=end){
			i++;
			if(i>end)break;
		}
		else{
			if(i==end)break;
			i=i>=99?0:i+1;
		}
	}         
	pprintf("\n\r��ӡʱ�䣺%d��%b��%b��%bʱ%b��\n\r",Year,Month,Date,Hours,Minutes);
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
	pprintf("ϵͳ��������\n\n��ȷ��������\n\n�������˳�\n\nע�⣺��Ҫרҵ��Ա������");
	if(wait_key()!=OK)return;
	for(i=0;i<6;i++){
		u.value=Scale[i];
		sflag=1;
		while(sflag){
			clrscr();
			pprintf("%s%f\n\n���Ӽ�������\n\n�����ü��ָ�ȱʡֵ\n\n��ȷ��������",sprompt[i],u.value);
			switch(wait_key()){
				case ESC:u.value=ScaleDefault[i];break;
				case OK:Scale[i]=u.value;sflag=0;break;
				case INC:{
					u.value+=0.0001;
					gotoxy(0,0);
					pprintf("%s%f\n\n���Ӽ�������\n\n�����ü��ָ�ȱʡֵ\n\n��ȷ��������",sprompt[i],u.value);
					delay(500);
					while(getch()==INC){
						u.value+=0.0001;
						gotoxy(0,0);
						pprintf("%s%f\n\n���Ӽ�������\n\n�����ü��ָ�ȱʡֵ\n\n��ȷ��������",sprompt[i],u.value);
						PCON|=0x10;
						T3=0;
						delay(100);
					}
					break;
				}
				case DEC:{
					u.value-=0.0001;
					gotoxy(0,0);
					pprintf("%s%f\n\n���Ӽ�������\n\n�����ü��ָ�ȱʡֵ\n\n��ȷ��������",sprompt[i],u.value);
	                delay(500);
	                while(getch()==DEC){
	                	u.value-=0.0001;
	                	gotoxy(0,0);
						pprintf("%s%f\n\n���Ӽ�������\n\n�����ü��ָ�ȱʡֵ\n\n��ȷ��������",sprompt[i],u.value);
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
	pprintf("��ʽ�����ݴ洢����\n\n��ȷ����ȷ��\n\n���������˳�");
	if(wait_key()!=OK)return;
	clrscr();
	pprintf("ȷ��Ҫ��ʽ����������\n\n��ȷ����ȷ��\n\n���������˳�");
	if(wait_key()!=OK)return;
	clrscr();
	pprintf("���ڸ�ʽ��������...\n\n��Ⱥ�...");
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
    pprintf("SJC-2���������ݲɼ���");
    delay(10000);
    if(Used!=0x55){
/*    	pprintf("\n\n���ڽ�����������ʽ��,\n\n��ȴ�...");
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
    	pprintf("��ʽ����ɣ�\n\n�����������...");*/
    	clrscr();
    	pprintf("�������д���!\n\n��ִ��ϵͳ��ʼ��.");
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
                    	pprintf("��ǰʱ������");
                    	pprintf("\n\n%bʱ%b��%b��    ",Hours,Minutes,Seconds);
                    	pprintf("\n\n%d��%b��%b��    ",Year,Month,Date);
                    	/*pprintf("\n\n%s%s",msg7,w[Week]);*/
                    	break;
                    }	
                    case 1:{
                    	/*clrscr();*/
                    	pprintf("���ص�ѹ     ");
                    	pprintf("\n\n��ǰֵ:%oV    ",ad(0));
                        /*pprintf("\n\n%s%oV",msg17,Average[0]);*/
                   		pprintf("\n\n�������ֵ:%oV    \n\n������Сֵ:%oV    ",Maximum[0],Minimum[0]);
                    	break;
                    }
                    case 2:{
                    	/*clrscr();*/
                    	pprintf("������:     ");
                    	pprintf("\n\n��ǰֵ:%oA    ",ad(5));
                    	/*pprintf("\n\n%s%oA",msg17,Average[1]);*/
                    	pprintf("\n\n�������ֵ:%oA    \n\n������Сֵ:%oA    ",Maximum[5],Minimum[5]);
                    	break;
                    }
                    case 3:{
                    	/*clrscr();*/
                    	pprintf("�ŵ����:     ");
                    	pprintf("\n\n��ǰֵ:%oA    ",ad(4));
                    	/*pprintf("\n\n%s%oA",msg17,Average[2]);*/
                    	pprintf("\n\n�������ֵ:%oA    \n\n������Сֵ:%oA    ",Maximum[4],Minimum[4]);
                    	break;
                    }
                    case 4:{
                    	/*clrscr();*/
                    	pprintf("������:     ");
                    	pprintf("\n\n��ǰֵ:%dW/m    ",ad(1));
                    	/*pprintf("\n\n%s%dW/m",msg17,Average[3]);*/
                    	pprintf("\n\n�������ֵ:%dW/m    \n\n������Сֵ:%dW/m    ",Maximum[1],Minimum[1]);
                    	break;
                    }
                    case 5:{
                    	/*clrscr();*/
                    	pprintf("�����¶�:      ");
                    	pprintf("\n\n��ǰֵ:%o��    ",ad(2)-200);
                    	/*pprintf("\n\n%s%o",msg17,Average[4]);*/
                    	pprintf("\n\n�������ֵ:%o��    \n\n������Сֵ:%o��    ",Maximum[2]-200,Minimum[2]-200);
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

