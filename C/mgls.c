/*==============================*/
/*  MGLS.C                      */
/*  液晶显示驱动程序            */
/*  适用型号:MGLS-19264         */
/*  编制开始时间:1998.10.28     */
/*==============================*/

/*==========================================================================*/
/*              程序说明及编程思路:                                         */
/*      请参看《内置HD61102控制驱动器图形液晶显示模块》，清华蓬远科贸公司   */
/*MGLS-19264可以理解成是由三块液晶显示器合成的,公用一套接口电路,            */
/*由/CSA和/CSB组合选通,/CSA=/CSB=0,选中第一块61102,列地址(Y                 */
/*Address)范围是0-63,/CSA=0,/CSB=1,选中第二块61102,Y=64-127;                */
/*/CSA=1,/CSB=0,选中第三块61102,Y=128-191;/CSA=/CSB=1是无效的.              */
/*  这样，所有的操作都要根据不同的列地址对相应的控制器来进行。              */
/*==========================================================================*/
#pragma LARGE

#include <reg552.h>
#include <stdarg.h>
#include "ASCII.H"
#include "HZ.h"
#include "Hz.c"
#include "ha.h"

/*======================*/
/*	硬件接口定义	*/
/*======================*/
#define base			((char *)0x29000)
#define WriteCommand	((char *)0x29000)
#define ReadCommand	((char *)0x29008)
#define WriteData				((char *)0x29004)
#define ReadData				((char *)0x2900c)
sbit backlight=P4^4;
unsigned char x;				/*显示坐标的垂直坐标*/
unsigned int y;					/*显示坐标的水平坐标*/
unsigned char char_point=10; /*  设置变量指针    */
bit intflag,mflag,mdisplay,Asciibold;    /*  中断标志    */
lcd_int(unsigned chip)
{
    while(ReadCommand[chip]>=0x80);
}

clrscr()
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

lcd_char(unsigned char dot)  /*页地址,Y地址,字符点阵数组*/
{
    unsigned char i,chip,*p;
    static union{
    	unsigned char c[2];
    	unsigned int d;
    }c;
    static bit first;
    if(dot<=160){
    	first=1;
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
    if(y>=128)chip=1;
	else{
        if(y>=64)chip=2;
			else chip=0;
	}
    lcd_int(chip);
    WriteCommand[chip]=(x&7)|0xb8;     /*页地址设置*/
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=0;i<16;i++){
    	lcd_int(chip);
        WriteData[chip]=CHINESEDOT[c.c[0]][i];
    }
    lcd_int(chip);
    WriteCommand[chip]=((x+1)&7)|0xb8;
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=16;i<32;i++){
        lcd_int(chip);
        WriteData[chip]=CHINESEDOT[c.c[0]][i];
    }
    lcd_int(chip);
    WriteCommand[chip]=0xc0;
    y=y+16;
    if(y>=192){
        y=0;
        x=(x>=7)?0:x+2;
    }
}
/*单个英文字符显示*/
lcd_asc(unsigned char dot)
{
    unsigned char i,chip;
	if(y>=128)chip=1;
	else{
		if(y>=64)chip=2;
			else chip=0;
	}
    lcd_int(chip);
    WriteCommand[chip]=(x&0x7)|0xb8;     /*页地址设置*/
    lcd_int(chip);
    WriteCommand[chip]=(y&0x3f)|0x40;
    for(i=0;i<8;i++){
        lcd_int(chip);
        WriteData[chip]=*(ASCII+dot*8+i);
    }
    lcd_int(chip);
    WriteCommand[chip]=0xc0;
    y=y+8;
    if(y>=192){
        y=0;
        x=(x>=7)?0:x+1;
    }
}

/*单个英文字符显示*/
lcd_ascb(unsigned char dot)
{
    unsigned char i,chip,*p,buf[8],j,l;
	if(y>=128)chip=1;
	else{
		if(y>=64)chip=2;
			else chip=0;
	}
    p=ASCII+dot*8;
    lcd_int(chip);
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
        x=(x>=7)?0:x+1;
    }
}

gotoxy(char x1,char y1)
{
    x=x1;
    y=y1;
}

LcdOpen()
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

LcdClose()
{
	unsigned char chip;
    for(chip=0;chip<3;chip++){
    	lcd_int(chip);
    	WriteCommand[chip]=0x3e;
    	lcd_int(chip);
    	WriteCommand[chip]=0xc0;
    }
    clrscr();
    backlight=0;
}

pprintf (char *format,...)
{
    unsigned char format_flag,*ptr,cc;
	unsigned int div_val, u_val, basemumber;
    va_list ap;
    va_start(ap,format);
    cc=0;
    for (;;){
        while ((format_flag = *format++) != '%'){      /* Until '%' or '\0' */
            if (!format_flag){
                return (1);
            }
            if(format_flag=='\n'){
            	x=(x>=7)?0:x+1;
            	y=0;
            }
            else if(Asciibold)lcd_char(format_flag);else lcd_char(format_flag);
        }
        switch (format_flag = *format++){
            case 'c':
                format_flag = va_arg(ap,int);
            default:
                if(Asciibold)lcd_ascb(format_flag);else lcd_asc(format_flag);
            continue;
            case 's':
                ptr = va_arg(ap,char *);
            while (format_flag = *ptr++){
                if(cc==char_point)lcd_char(' ');
                else lcd_char(format_flag);
            }
            continue;
            case 'b':
            	div_val=100;
            	u_val=va_arg(ap,unsigned char);
            	goto l1;
            case 'o':
            case 'd':
                div_val = 10000;
            u_val = va_arg(ap,int);
l1:			basemumber = 10;
                while (div_val > 1 && div_val > u_val){
                    div_val /= 10;
                }
                if(format_flag=='o' && div_val==1){
                    if(cc==char_point)if(Asciibold)lcd_ascb(' ');else lcd_asc(' ');
                    else if(Asciibold)lcd_ascb('0');else lcd_asc('0');
                }
            do{
                if(format_flag=='o' && div_val==1){
                    if(cc==char_point)if(Asciibold)lcd_ascb(' ');else lcd_asc(' ');
                    else if(Asciibold)lcd_ascb('.');else lcd_asc('.');
                }
                if(cc==char_point)if(Asciibold)lcd_ascb(' ');else lcd_asc(' ');
                else if(Asciibold)lcd_ascb(u_val / div_val+48);else lcd_asc(u_val / div_val+48);
                u_val %= div_val;
                div_val /= basemumber;
            }
            while (div_val);
        }
        cc++;
    }
}

main()
{
	LcdOpen();
	clrscr();
	lcd_char('a');
	pprintf("我是中国人!");
}
