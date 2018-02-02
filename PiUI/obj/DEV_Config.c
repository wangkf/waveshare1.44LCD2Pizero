#include "DEV_Config.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

uint8_t System_Init(void)
{
	//1.wiringPiSetupGpio
    //if(wiringPiSetup() < 0)//use wiringpi Pin number table
    if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
        printf(" set wiringPi lib failed	!!! \r\n");
        return 1;
    } else {
       printf(" set wiringPi lib success  !!! \r\n");
    }
    //2.GPIO config
    pinMode(LCD_RST,OUTPUT);
    pinMode(LCD_DC,OUTPUT);
    pinMode(LCD_BL,OUTPUT);
    pinMode(LCD_CS,OUTPUT);	//must set cs output,Otherwise it does not work

	pinMode(KEYA,INPUT); //引脚0为输入模式
  pullUpDnControl(KEYA,PUD_UP);  //设置0号引脚上拉
  pinMode(KEYB,INPUT);
  pullUpDnControl(KEYB,PUD_UP);
  pinMode(KEYC,INPUT);
  pullUpDnControl(KEYC,PUD_UP);
  pinMode(KEYUP,INPUT);
  pullUpDnControl(KEYUP,PUD_UP);
  pinMode(KEYDOWN,INPUT);
  pullUpDnControl(KEYDOWN,PUD_UP);
  pinMode(KEYLEFT,INPUT);
  pullUpDnControl(KEYLEFT,PUD_UP);
  pinMode(KEYRIGHT,INPUT);
  pullUpDnControl(KEYRIGHT,PUD_UP);
  pinMode(KEYPRESS,INPUT);
  pullUpDnControl(KEYPRESS,PUD_UP);
  //
    //3.spi init
    //wiringPiSPISetup(0,9000000);
    wiringPiSPISetupMode(0, 32000000, 0);
    return 0;
}

void System_Exit(void)
{
	LCD_CS_1;
}

/*********************************************


*********************************************/
void SPI_Write_Byte(uint8_t value)
{
    int read_data;
    read_data = wiringPiSPIDataRW(0,&value,1);
    if(read_data < 0)
        perror("wiringPiSPIDataRW failed\r\n");
}

/*********************************************


*********************************************/
void Driver_Delay_ms(uint32_t xms)
{
    delay(xms);
}

void Driver_Delay_us(uint32_t xus)
{
    int j;
    for(j=xus; j > 0; j--);
}
