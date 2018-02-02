#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>

/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
#include <wiringPi.h>

//GPIO config
#define SPI_MISO 9
#define SPI_MOSI 10
#define SPI_SCK  11
#define LCD_CS   8
#define LCD_RST  27
#define LCD_DC   25
#define LCD_BL   24

#define KEYA 21
#define KEYB 20
#define KEYC 16
#define KEYUP 6
#define KEYDOWN 19
#define KEYLEFT 5
#define KEYRIGHT 26
#define KEYPRESS 13

//SPI
#define SPI_MOSI_0	digitalWrite(SPI_MOSI,LOW)
#define SPI_MOSI_1	digitalWrite(SPI_MOSI,HIGH)

#define is_SPI_MISO	digitalRead(SPI_MISO)

#define SPI_SCK_0	digitalWrite(SPI_SCK,LOW)
#define SPI_SCK_1	digitalWrite(SPI_SCK,HIGH)

//LCD
#define LCD_CS_0	digitalWrite(LCD_CS,LOW)
#define LCD_CS_1	digitalWrite(LCD_CS,HIGH)

#define LCD_RST_0	digitalWrite(LCD_RST,LOW)
#define LCD_RST_1	digitalWrite(LCD_RST,HIGH)

#define LCD_DC_0	digitalWrite(LCD_DC,LOW)
#define LCD_DC_1	digitalWrite(LCD_DC,HIGH)

#define LCD_BL_0	digitalWrite(LCD_BL,LOW)
#define LCD_BL_1	digitalWrite(LCD_BL,HIGH)

/*------------------------------------------------------------------------------------------------------*/
//按键服务函数
extern int flagKEYA;
extern int flagKEYB;
extern int flagKEYC;
extern int flagKEYUP;
extern int flagKEYDOWN;
extern int flagKEYLEFT;
extern int flagKEYRIGHT;
extern int flagKEYPRESS;

#define is_KEYA	digitalRead(KEYA)
#define is_KEYB	digitalRead(KEYB)
#define is_KEYC	digitalRead(KEYC)
#define is_KEYUP	digitalRead(KEYUP)
#define is_KEYDOWN	digitalRead(KEYDOWN)
#define is_KEYLEFT	digitalRead(KEYLEFT)
#define is_KEYRIGHT	digitalRead(KEYRIGHT)
#define is_KEYPRESS	digitalRead(KEYPRESS)

/*------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------*/
uint8_t System_Init(void);
void System_Exit(void);

void SPI_Write_Byte(uint8_t value);
void Driver_Delay_ms(uint32_t xms);
void Driver_Delay_us(uint32_t xus);

#endif
