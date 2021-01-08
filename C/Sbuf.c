/*==================================*/
/*  serial port                     */
/*==================================*/

#include <reg552.h>

#define MAXLENGTH	20			//最大帧字节长度
extern bit received;
extern unsigned char buf[10];
extern unsigned char point;
extern bit TXEN;
unsigned char serial_error;
bit rec=0;
union{
	unsigned int i;
	unsigned char c[2];
}chksum;
union{
	unsigned char c[2];
	unsigned int i;
}length;          

#define ADR1 1
#define ADR2 2
#define CID1 3

unsigned char high(unsigned char);
unsigned char low(unsigned char);                                

//*****************************
//串口初始化                0xfd=19200,0xfa=9600,0xf4=4800,0xe8=2400,0xd0=1200
void serial_init (void)  {
  S0CON  = 0x70;                      /* mode 1: 8-bit UART, enable receiver   */
  TMOD |= 0x20;                      /* timer 1 mode 2: 8-Bit reload          */
//  PCON |= 0x00;
   TH1 = 0xf4;//fa,          //baud*2    /* reload value 19200 baud       */
  TR1   = 1;                         /* timer 1 run                           */
  ES0    = 1; REN=1; EA=1; //SM2=1;          //SM2=1时收到的第9位为1才置位RI标志  
  PS0=1;
 //TMOD |=0x01;  //th1 auto load 2X8,th0 1X16
 //TH0=31; TL0=0; //X 32 =1S
 //TR0=1; ET0=1;
}

/*==============================================================*/
/*  command string format:                                      */
/*  SOI,ADR1,ADR2,CID1,CID2,LENGTH,DATA,CHKSUM,EOI				*/
/*  0x7e,0,  0,   20H, X,   XX    ,X,   X,     0D 				*/
/*==============================================================*/ 
void serial(void) interrupt 4 using 1
{
	unsigned char schar;
	if(TI){
    	TI=0;
    	TXEN=1;
    }
	if(RI){             
		schar=S0BUF;
//		if(schar==0x7e)S0BUF=schar;
		RI=0;    
//		S0BUF=~S0BUF;
		if(!rec){
			if(schar==0x7e){				//SOI
				point=0;
				rec=1;
				chksum.i=0;    
//				S0BUF='0';
			} 
		}
		else{
			if(point==0){
				if(schar!=ADR1)rec=0;		//ADR1
				else{
					chksum.i+=schar;
					point++;
				}
				return;
			}
			if(point==1){
				if(schar!=ADR2)rec=0;		//ADR2
				else{
					chksum.i+=schar;
					point++;
				}
				return;
			}
			if(point==2){
				if(schar!=CID1)rec=0;		//CID1
				else{
					chksum.i+=schar;
					point++;
				}
				return;
			}
			if(point==3){
				buf[0]=schar;				//CID2
				chksum.i+=buf[0];
				point++;
				return;
			}
			if(point==4){
				buf[1]=schar;				//LENGTH.H
				chksum.i+=buf[1];
				point++;
				return;
			}
			if(point==5){				   //LENGTH.L
				buf[2]=schar;
				chksum.i+=buf[2];
				point++;
				if(high(buf[1])==((~(low(buf[1])+high(buf[2])+low(buf[2]))+1)&0xf)){
					length.c[0]=buf[1]&0xf;
					length.c[1]=buf[2];
				}
				else{
			 		serial_error=1;
			 		rec=0;
				}
				return;
			}
			if(point>5 && point<=length.i+5){	//DATA PACKET
				buf[point-3]=schar;
				chksum.i+=schar;
				point++;
				return;
			}
			if(point==length.i+6){			    //CHKECKSUM.H
				buf[9]=schar;
				point++;
			    return;
			}
			if(point==length.i+7){
				chksum.i=~chksum.i+1;
				if((chksum.c[1])!=schar || chksum.c[0]!=buf[9]){
					serial_error=1;
					rec=0;
				}
				else point++;
				return;
			}
			if(point==length.i+8){
				if(schar==0xd){
					received=1;
					rec=0;
				}
				return;
			}   
			if(point>MAXLENGTH){
				rec=0;
				point=0;
				return;
			}    	
    	}
	}
}

unsigned int sendchar(unsigned char cc)
{
	while(!TXEN);
	S0BUF=cc;
	TXEN=0;
	return cc;
}

unsigned int sendint(unsigned int ii)
{                                              
	union{
		unsigned char c[2];
		unsigned int i;
	}un;
	un.i=ii;
	return sendchar(un.c[1])+sendchar(un.c[0]);
}

unsigned int sendfloat(float ff)
{
	union{
		unsigned char cc[4];
 		float ft;
  	}f;
  	unsigned int chksum;
  	f.ft=ff;
  	chksum=sendchar(f.cc[0]);
  	chksum+=sendchar(f.cc[1]);
  	chksum+=sendchar(f.cc[2]);
  	chksum+=sendchar(f.cc[3]);
  	return chksum;
}
  	

unsigned int sendrecord(unsigned char *pchar,unsigned int num)
{
	unsigned int chksum,i;
	chksum=0;
	for(i=0;i<num;i++){
		chksum+=sendchar(pchar[i]);
	   	PCON|=0x10;
    	T3=0;
	}
	return chksum;
}
	
                                                 

