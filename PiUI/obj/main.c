#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <time.h>
#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include "LCD_BMP.h"
#include "DEV_Config.h"

int main(void)
{
	//1.系统初始化，GPIO端口初始化
	if(System_Init())
	exit(0);	
	//初始化wiringPi库
	//初始化失败时，打印出提示信息，并且退出
	//if(wiringPiSetupGpio()<0){printf("can't init wiringPi\n");return -1;}
	int flagKEYA=0,flagKEYB=0,flagKEYC=0,flagKEYUP=0,flagKEYDOWN=0,flagKEYLEFT=0,flagKEYRIGHT=0,flagKEYPRESS=0;

	void myInterruptServiceKeyA(){flagKEYA=1;}
	void myInterruptServiceKeyB(){flagKEYB=1;}
	void myInterruptServiceKeyC(){flagKEYC=1;}
	void myInterruptServiceKeyUP(){flagKEYUP=1;}
	void myInterruptServiceKeyDOWN(){flagKEYDOWN=1;}
	void myInterruptServiceKeyLEFT(){flagKEYLEFT=1;}
	void myInterruptServiceKeyRIGHT(){flagKEYRIGHT=1;}
	void myInterruptServiceKeyPRESS(){flagKEYPRESS=1;}

	if(wiringPiISR(KEYA,          INT_EDGE_FALLING,&myInterruptServiceKeyA)<0){printf("unable ISR-KEYA\n");}  //注册中断程序，设置引脚下降沿触发
	if(wiringPiISR(KEYB,          INT_EDGE_FALLING,&myInterruptServiceKeyB)<0){printf("unable ISR-KEYB\n");}
	if(wiringPiISR(KEYC,          INT_EDGE_FALLING,&myInterruptServiceKeyC)<0){printf("unable ISR-KEYC\n");}
	if(wiringPiISR(KEYUP,       INT_EDGE_FALLING,&myInterruptServiceKeyUP)<0){printf("unable ISR-KEYUP\n");}
	if(wiringPiISR(KEYDOWN,INT_EDGE_FALLING,&myInterruptServiceKeyDOWN)<0){printf("unable ISR-KEYDOWN\n");}
	if(wiringPiISR(KEYLEFT,     INT_EDGE_FALLING,&myInterruptServiceKeyLEFT)<0){printf("unable ISR-KEYLEFT\n");}
	if(wiringPiISR(KEYRIGHT,  INT_EDGE_FALLING,&myInterruptServiceKeyRIGHT)<0){printf("unable ISR-KEYRIGHT\n");}
	if(wiringPiISR(KEYPRESS,  INT_EDGE_FALLING,&myInterruptServiceKeyPRESS)<0){printf("unable ISR-KEYPRESS\n");}

	//2.初始显示界面
	LCD_SCAN_DIR LCD_ScanDir = SCAN_DIR_DFT;
	LCD_Init(LCD_ScanDir );	

	GUI_Showparts(0);	//开机画面显示
	Driver_Delay_ms(1000);
	GUI_Show();//系统仪表盘界面
	for(;;)
	{
	//检测按键是否松开，没有松开的话，一直等待
	if(flagKEYA){while(is_KEYA==LOW);GUI_Show();flagKEYA=0;}
	if(flagKEYB){while(is_KEYB==LOW);LCD_ShowBmp(0);GUI_Show();flagKEYB=0;}
	if(flagKEYC){while(is_KEYC==LOW);GUI_Showparts(0);Driver_Delay_ms(3000);GUI_Show();flagKEYC=0;}
	if(flagKEYUP){while(is_KEYUP==LOW);LCD_ShowBmp(4);GUI_Show();flagKEYUP=0;}
	if(flagKEYDOWN){while(is_KEYDOWN==LOW);printf("DOWN\n");flagKEYDOWN=0;}
	if(flagKEYLEFT){while(is_KEYLEFT==LOW);printf("LEFT\n");flagKEYLEFT=0;}
	if(flagKEYRIGHT){while(is_KEYRIGHT==LOW);printf("RIGHT\n"); flagKEYRIGHT=0;}
	if(flagKEYPRESS){while(is_KEYPRESS==LOW);LCD_ShowBmp(7);GUI_Show();flagKEYPRESS=0;}

	if(time(0)%60<5)GUI_Showparts(60);//每分钟部分刷新，更新部分系统信息（考虑到系统延时，按分钟刷新时留5秒延时）
		GUI_Showparts(1);//每秒钟局部刷新，比如秒针闪烁
		Driver_Delay_ms(600);
	}
	//3.系统退出
	System_Exit();
 return 0;
}