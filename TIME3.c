// lcd1602.c
// kompajlirati sa -lwiringPi -lwiringPiDev
#include <stdio.h>
#include <string.h> 
#include <errno.h>
#include <wiringPi.h>
#include <lcd.h>

#include <bcm2835.h> 
#include <unistd.h>

#define changeHexToInt(hex) ((((hex)>>4) *10 ) + ((hex)%16) ) 

//adrese registara
#define SEK 0x02 
#define MIN 0x03 
#define SAT 0x04 
#define DAN 0x05
#define MES 0x07
#define GOD 0x08

// dodela vrednosti za konkretne pinove 
// prema gornjoj tabeli i semi DVK512
const int RS = 3; 
const int EN = 14; 
const int D0 = 4; 
const int D1 = 12; 
const int D2 = 13; 
const int D3 = 6; 

unsigned char WriteBuf[2]; 
 unsigned char ReadBuf;
 unsigned char g8563_Store[6]; 
 // sec,min,sat
 unsigned char init8563_Store[6]={0x00,0x59,0x08,0,0,0};
void P8563_settime() 
{ 
 WriteBuf[0] = SEK; 
 WriteBuf[1] = g8563_Store[0]; 
bcm2835_i2c_write(WriteBuf,2); 
 WriteBuf[0] = MIN; 
 WriteBuf[1] = g8563_Store[1]; 
 bcm2835_i2c_write(WriteBuf,2); 
 
 WriteBuf[0] = SAT; 
 WriteBuf[1] = g8563_Store[2]; 
 bcm2835_i2c_write(WriteBuf,2); 

 WriteBuf[0] = DAN;
 WriteBuf[1] = g8563_Store[3]; 
 bcm2835_i2c_write(WriteBuf,2); 
 WriteBuf[0] = MES; 
 WriteBuf[1] = g8563_Store[4]; 
 bcm2835_i2c_write(WriteBuf,2); 
 WriteBuf[0] = GOD; 
 WriteBuf[1] = g8563_Store[5]; 
 bcm2835_i2c_write(WriteBuf,2); 
}
void P8563_init() 
{ 
 unsigned char i; 
 for(i=0;i<=6;i++) 
 g8563_Store[i]=init8563_Store[i]; 
 
 P8563_settime(); 
 printf("Postavi početno tekuće vreme\n"); 
 
//inicijalizacija RTC-a
 WriteBuf[0] = 0x0; 
 WriteBuf[1] = 0x00; //normalni rezim rada
 bcm2835_i2c_write(WriteBuf,2); 
} 
void P8563_Readtime() 
{ 
 unsigned char time[7]; 
 WriteBuf[0] = SEK; 
 bcm2835_i2c_write_read_rs(WriteBuf, 1, time, 7); 
 
 g8563_Store[0] = time[0] & 0x7f; 
 g8563_Store[1] = time[1] & 0x7f; 
 g8563_Store[2] = time[2] & 0x3f; 
 g8563_Store[3] = time[3] & 0x3f; 
 g8563_Store[4] = time[5] & 0x1f; 
 g8563_Store[5] = time[6] & 0xff; 
 
 g8563_Store[0] = changeHexToInt(g8563_Store[0]);
 g8563_Store[1] = changeHexToInt(g8563_Store[1]);
 g8563_Store[2] = changeHexToInt(g8563_Store[2]);
 g8563_Store[3] = changeHexToInt(g8563_Store[3]);
 g8563_Store[4] = changeHexToInt(g8563_Store[4]);
 g8563_Store[5] = changeHexToInt(g8563_Store[5]);
}

int main(){
int lcd_h;
if (wiringPiSetup() < 0){
 fprintf (stderr, "Greška pri inicijalizaciji: %s\n", strerror (errno)) ;
 return 1 ;
}

if (!bcm2835_init()) 
 return 1; 
 
 bcm2835_i2c_begin(); 
// adresa PCF8563 na I2C magistrali
 bcm2835_i2c_setSlaveAddress(0x51); 
 bcm2835_i2c_set_baudrate(10000); 
 printf("start..........\n"); 
 P8563_init() ;


lcd_h = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
 
 while(1) 
 { 
 P8563_Readtime();

lcdPosition(lcd_h, 0,0);
//g8563_Store[5], g8563_Store[4], g8563_Store[3]
printf("%d", g8563_Store[0]);
lcdPrintf(lcd_h,"AaA");
lcdPosition(lcd_h, 0,1);
//g8563_Store[2], g8563_Store[1], g8563_Store[0]
lcdPrintf(lcd_h,"SsS");
 bcm2835_delay(5000); 
lcdClear(lcd_h);
} 
 
 bcm2835_i2c_end(); 
 bcm2835_close(); 
 
 return 0; 
}