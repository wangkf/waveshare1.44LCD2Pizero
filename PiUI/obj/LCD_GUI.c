#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>

#include "LCD_GUI.h"

#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define CPUAVG_PATH "/proc/loadavg"

#define MAX_SIZE 32
#define NETWORK_FILE "/etc/network/interfaces"

time_t timep;
struct tm *p;
char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

char get_temp(void);
//char get_cpuavg(void);
char* getip(char* ip_buf);
char* get_temp2(void);
int min,hour,sec,mday,mmon,myear;
unsigned char week;
struct tm *localtime(const time_t *timep);

extern LCD_DIS sLCD_DIS;

void GUI_Swop(POINT Point1, POINT Point2)
{
    POINT Temp;
    Temp = Point1;
    Point1 = Point2;
    Point2 = Temp;
}

char get_temp(void)
{
    int fd;
    double temp = 0;
    char buf[MAX_SIZE];
    // 打开/sys/class/thermal/thermal_zone0/temp
    fd = open(TEMP_PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open thermal_zone0/temp\n");
		close(fd);
        return -1;
    }
    // 读取内容
    if (read(fd, buf, MAX_SIZE) < 0) {
        fprintf(stderr, "failed to read temp\n");
		close(fd);
        return -1;
    }
    // 转换为浮点数打印
    temp = atoi(buf) / 1000.0;
        // 关闭文件
        close(fd);
        return temp;
}

/*
	char get_cpuavg(void)
{
    int fd;
    double cpuavg = 0;
    char buf[MAX_SIZE];
    fd = open(CPUAVG_PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open cpuavg\n");
		close(fd);
        return -1;
    }
    // 读取内容
    if (read(fd, buf, MAX_SIZE) < 0) {
        fprintf(stderr, "failed to read cpuavg\n");
		close(fd);
        return -1;
    }
    // 转换为浮点数打印
    cpuavg = atoi(buf) ;
        // 关闭文件
        close(fd);
        return cpuavg;
}
*/
char* getip(char* ip_buf)
{
    struct ifreq temp;
    struct sockaddr_in *myaddr;
    int fd = 0;
    int ret = -1;
    strcpy(temp.ifr_name, "wlan0");
    if((fd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        return NULL;
    }
    ret = ioctl(fd, SIOCGIFADDR, &temp);
    close(fd);
    if(ret < 0) return NULL; 
	myaddr = (struct sockaddr_in *)&(temp.ifr_addr);
	strcpy(ip_buf, inet_ntoa(myaddr->sin_addr));
   return ip_buf;
} 

/********************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
			Xpoint		:   The x coordinate of the point
			Ypoint		:   The y coordinate of the point
			Color		:   Set color
			Dot_Pixel	:	point size
********************************************************************************/
void GUI_DrawPoint(POINT Xpoint, POINT Ypoint, COLOR Color,
                   DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE)
{
    if(Xpoint > sLCD_DIS.LCD_Dis_Column || Ypoint > sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    uint16_t XDir_Num ,YDir_Num;
    if(DOT_STYLE == DOT_STYLE_DFT) {
        for(XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for(YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                LCD_SetPointlColor(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for(XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for(YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                LCD_SetPointlColor(Xpoint + XDir_Num - 1 , Ypoint + YDir_Num -1 , Color);
            }
        }
    }
}

/********************************************************************************
function:	Draw a line of arbitrary slope
parameter:
********************************************************************************/
void GUI_DrawLine(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                  COLOR Color, LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
{

    if(Xstart > sLCD_DIS.LCD_Dis_Column || Ystart > sLCD_DIS.LCD_Dis_Page ||
       Xend > sLCD_DIS.LCD_Dis_Column || Yend > sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    if(Xstart > Xend)
        GUI_Swop(Xstart,Xend);
    if(Ystart > Yend)
        GUI_Swop(Ystart,Yend);

    POINT Xpoint = Xstart;
    POINT Ypoint = Ystart;
    int32_t dx =(int32_t)Xend -(int32_t)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int32_t dy =(int32_t)Yend -(int32_t)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int32_t XAddway = Xstart < Xend ? 1 : -1;
    int32_t YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    int32_t Esp = dx + dy;
    int8_t Line_Style_Temp = 0;

    for(;;) {
        Line_Style_Temp++;
        //Painted dotted line, 2 point is really virtual
        if(Line_Style == LINE_DOTTED && Line_Style_Temp %3 == 0) {
            //printf("LINE_DOTTED\r\n");
            GUI_DrawPoint(Xpoint, Ypoint, GUI_BACKGROUND, Dot_Pixel, DOT_STYLE_DFT);
            Line_Style_Temp = 0;
        } else {
            GUI_DrawPoint(Xpoint, Ypoint, Color, Dot_Pixel, DOT_STYLE_DFT);
        }
        if(2 * Esp >= dy) {
            if(Xpoint == Xend) break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if(2 * Esp <= dx) {
            if(Ypoint == Yend) break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/********************************************************************************
function:	Draw a rectangle
parameter:
********************************************************************************/
void GUI_DrawRectangle(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                       COLOR Color, DRAW_FILL Filled, DOT_PIXEL Dot_Pixel )
{
    if(Xstart > sLCD_DIS.LCD_Dis_Column || Ystart > sLCD_DIS.LCD_Dis_Page ||
       Xend > sLCD_DIS.LCD_Dis_Column || Yend > sLCD_DIS.LCD_Dis_Page) {
        return;
    }
//	printf("sLCD_DIS.LCD_Dis_Column = %d\r\n",sLCD_DIS.LCD_Dis_Column);
//	printf("sLCD_DIS.LCD_Dis_Page = %d\r\n",sLCD_DIS.LCD_Dis_Page);
    if(Xstart > Xend)
        GUI_Swop(Xstart,Xend);
    if(Ystart > Yend)
        GUI_Swop(Ystart,Yend);

    POINT Ypoint;
    if(Filled ) {
        for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            GUI_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color , LINE_SOLID, Dot_Pixel);
        }
    } else {
        GUI_DrawLine(Xstart, Ystart, Xend, Ystart, Color , LINE_SOLID, Dot_Pixel);
        GUI_DrawLine(Xstart, Ystart, Xstart, Yend, Color , LINE_SOLID, Dot_Pixel);
        GUI_DrawLine(Xend, Yend, Xend, Ystart, Color , LINE_SOLID, Dot_Pixel);
        GUI_DrawLine(Xend, Yend, Xstart, Yend, Color , LINE_SOLID, Dot_Pixel);
    }
}

/********************************************************************************
function:	Use the 8-point method to draw a circle of the
				specified size at the specified position.
parameter:
********************************************************************************/
void GUI_DrawCircle(POINT X_Center, POINT Y_Center, LENGTH Radius,
                    COLOR Color, DRAW_FILL  Draw_Fill , DOT_PIXEL Dot_Pixel)
{
    if(X_Center > sLCD_DIS.LCD_Dis_Column || Y_Center >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    //Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    int16_t Esp = 3 -(Radius << 1 );

    int16_t sCountY;
    if(Draw_Fill == DRAW_FULL) {
        while(XCurrent <= YCurrent ) { //Realistic circles
            for(sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                GUI_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //1
                GUI_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //2
                GUI_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //3
                GUI_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //4
                GUI_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //5
                GUI_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //6
                GUI_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );             //7
                GUI_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );
            }
            if(Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 *(XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while(XCurrent <= YCurrent ) {
            GUI_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //1
            GUI_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //2
            GUI_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //3
            GUI_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //4
            GUI_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //5
            GUI_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //6
            GUI_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //7
            GUI_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );             //0

            if(Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 *(XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

/********************************************************************************
function:	Show English characters
parameter:
			Color_Background : Select the background color of the English character
			Color_Foreground : Select the foreground color of the English character
********************************************************************************/
void GUI_DisChar(POINT Xpoint, POINT Ypoint, const char Acsii_Char,
                 sFONT* Font, COLOR Color_Background, COLOR Color_Foreground)
{
    POINT Page, Column;

    if(Xpoint >= sLCD_DIS.LCD_Dis_Column || Ypoint >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    uint32_t Char_Offset =(Acsii_Char - ' ') * Font->Height *(Font->Width / 8 +(Font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &Font->table[Char_Offset];

    for(Page = 0; Page < Font->Height; Page ++ ) {
        for(Column = 0; Column < Font->Width; Column ++ ) {

            //To determine whether the font background color and screen background color is consistent
            if(FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
                if(*ptr &(0x80 >>(Column % 8)))
                    GUI_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            } else {
                if(*ptr &(0x80 >>(Column % 8))) {
                    GUI_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                } else {
                    GUI_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            //One pixel is 8 bits
            if(Column % 8 == 7)
                ptr++;
        }/* Write a line */
        if(Font->Width % 8 != 0)
            ptr++;
    }/* Write all */
}

/********************************************************************************
function:	Display the string
parameter:
			Color_Background : Select the background color of the English character
			Color_Foreground : Select the foreground color of the English character
********************************************************************************/
void GUI_DisString_EN(POINT Xstart, POINT Ystart, const char * pString,
                      sFONT* Font,COLOR Color_Background, COLOR Color_Foreground )
{
    POINT Xpoint = Xstart;
    POINT Ypoint = Ystart;

    if(Xstart >= sLCD_DIS.LCD_Dis_Column || Ystart >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    while(* pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
        if((Xpoint + Font->Width ) > sLCD_DIS.LCD_Dis_Column ) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if((Ypoint  + Font->Height ) > sLCD_DIS.LCD_Dis_Page ) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        GUI_DisChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width + 1;
    }
}

/****************************************WHITE****************************************
function:	Display the string
parameter:

			Ystart : Y coordinate
			Nummber: The number displayed

			Color_Background : Select the background color of the English character
			Color_Foreground : Select the foreground color of the English character
********************************************************************************/
#define  ARRAY_LEN 255
void GUI_DisNum(POINT Xpoint, POINT Ypoint, int32_t Nummber,
                sFONT* Font,COLOR Color_Background, COLOR Color_Foreground )
{

    int16_t Num_Bit = 0, Str_Bit = 0;
    uint8_t Str_Array[ARRAY_LEN] = {0},Num_Array[ARRAY_LEN] = {0};
    uint8_t *pStr = Str_Array;

    if(Xpoint >= sLCD_DIS.LCD_Dis_Column || Ypoint >= sLCD_DIS.LCD_Dis_Page) {
        return;
    }

    //Converts a number to a string
    while(Nummber) {
        Num_Array[Num_Bit] = Nummber % 10 + '0';
        Num_Bit++;
        Nummber /= 10;
    }

    //The string is inverted
    while(Num_Bit > 0) {
        Str_Array[Str_Bit] = Num_Array[Num_Bit -1];
        Str_Bit ++;
        Num_Bit --;
    }

    //show
    GUI_DisString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Background, Color_Foreground );
}

void GUI_Disbitmap(POINT Xpoint, POINT Ypoint, const unsigned char *pBmp,
						COLOR Color_Foreground,POINT Width, POINT Height)
{
    POINT i, j, byteWidth = (Width + 7)/8;
    for(j = 0; j < Height; j++) {
        for(i = 0; i <Width; i ++) {
            if(*(pBmp + j*byteWidth + i/8) & (128 >> (i & 7))) {
                GUI_DrawPoint(Xpoint+i, Ypoint+j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
        }
    }
}

/********************************************************************************
function:	LCD_Show
parameter:
********************************************************************************/
void GUI_Show(void)
{
	LCD_Clear(GUI_BACKGROUND);
	char dateInfo[16],timeInfo[16];
		time(&timep);
		p=localtime(&timep);
		myear=p->tm_year;
		mmon=p->tm_mon;
		mday=p->tm_mday;
		min=p->tm_min;
		week=p->tm_wday;
		hour=p->tm_hour;
		sec=p->tm_sec;

		// 获得 system usage / info
		struct sysinfo sys_info;
		if(sysinfo(&sys_info) != 0)
		{
		      printf("sysinfo-Error\n");
		}
		char cpuInfo[10],mweek[10];
		char uptimeInfo[160],ramInfo[10],tempInfo[10],ipInfo[16];
		
		unsigned long uptime = sys_info.uptime /60;// 启动时间
		unsigned long avgCpuLoad = sys_info.loads[0] /1000;// CPU占用
		unsigned long totalRam = sys_info.totalram / 1024 / 1024;
		unsigned long freeRam = sys_info.freeram /1024 /1024;// 内存使用量及占用
		
		unsigned int usedRam = totalRam - freeRam;
		unsigned int ram_load = (usedRam * 100) / totalRam;

		sprintf(dateInfo, "%04d-%02d-%02d",myear+1900,mmon+1,mday);
		sprintf(timeInfo, "%02d:%02d:%02d",hour,min,sec);
		sprintf(mweek, "%s",wday[week]);
		sprintf(cpuInfo, "CPU:%ld%%", avgCpuLoad);
		sprintf(uptimeInfo, "Up:%ldmin", uptime);
		sprintf(ramInfo, "RAM:%.3dM %.2d%%", usedRam,ram_load);
		sprintf(tempInfo, "TEMP:%.2dC", get_temp());// 树莓派CPU温度

		getip(ipInfo);//IP 信息

//	printf("GUI Display String \r\n");
	//开始显示标题栏图标
	GUI_Disbitmap(4,3 , Signal816,GREEN,16,8);
	GUI_Disbitmap(20,3 , Msg816,BRED,16,8);
	GUI_Disbitmap(36,3 , Bluetooth88,BLUE,8,8);
	GUI_Disbitmap(45,3 , GPRS88,BROWN,8,8);
	GUI_Disbitmap(54,3 , Alarm88,GRAY,8,8);
	GUI_Disbitmap(112,3 , Bat816,GRED,16,8);
	GUI_DisString_EN(72,1,"MUYUN",&Font12,GUI_BACKGROUND,GREEN);
	//竖线
	GUI_DrawLine(1,15,1,128,WHITE,0,1);
	GUI_DrawLine(128,15,128,128,WHITE,0,1);
	//短竖线
	GUI_DrawLine(62,70,62,94,WHITE,0,1);
	//横线
	GUI_DrawLine(1,15,128,15,WHITE,0,1);
	GUI_DrawLine(1,70,128,70,WHITE,0,1);
	GUI_DrawLine(1,94,128,94,WHITE,0,1);
	GUI_DrawLine(1,110,128,110,WHITE,0,1);
	GUI_DrawLine(1,128,128,128,WHITE,0,1);
	//各种系统信息显示
	GUI_DisString_EN(5,20,uptimeInfo,&Font16,GUI_BACKGROUND,RED);
	GUI_DisString_EN(5,36,    dateInfo,&Font16,GUI_BACKGROUND,WHITE);
	GUI_DisString_EN(5,54,    timeInfo,&Font16,GUI_BACKGROUND,GBLUE);
	GUI_DisString_EN(102,56,  mweek,&Font12,GUI_BACKGROUND,RED);
	GUI_DisString_EN(5,78,     cpuInfo,&Font12,GUI_BACKGROUND,YELLOW);
	GUI_DisString_EN(64,78, tempInfo,&Font12,GUI_BACKGROUND,RED);
	GUI_DisString_EN(5,96,    ramInfo,&Font12,GUI_BACKGROUND,GBLUE);
	GUI_DisString_EN(5,114,     ipInfo,&Font12,GUI_BACKGROUND,GREEN);
/*
#define WHITE          0xFFFF
#define BLACK          0x0000	  
#define BLUE           0x001F  
#define BRED           0XF81F
#define GRED 		   0XFFE0
#define GBLUE		   0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define BROWN 		   0XBC40 
#define BRRED 		   0XFC07 
#define GRAY  		   0X8430 
*/
}

void GUI_Showparts(int parts)
{
	char timeInfo1[16],timeInfo2[16];
	time(&timep);
	p=localtime(&timep);
	min=p->tm_min;
	week=p->tm_wday;
	hour=p->tm_hour;
	sec=p->tm_sec;

if(parts==0)
{
	LCD_Clear(GUI_BACKGROUND);
//	GUI_Disbitmap(44,70,Bmp4016,WHITE,40,16);
	GUI_Disbitmap(1,1,gmy,WHITE,128,128);
}

if(parts==1){
	sprintf(timeInfo2, "%02d",sec);
	GUI_DrawRectangle(75,54,101,70,BLACK,1,1);
	GUI_DisString_EN(75,54,  timeInfo2,&Font16,GUI_BACKGROUND,GRAY);
}

if(parts==60)
{
	sprintf(timeInfo1, "%02d:%02d",hour,min);
	GUI_DrawRectangle(5,54,65,70,BLACK,1,1);
	GUI_DisString_EN(5,54,  timeInfo1,&Font16,GUI_BACKGROUND,GBLUE);

// 获得 system usage / info
		struct sysinfo sys_info;
		if(sysinfo(&sys_info) != 0)
		{
		      printf("sysinfo-Error\n");
		}
		char cpuInfo[10];
		char uptimeInfo[160],ramInfo[10],tempInfo[10],ipInfo[16];
		
		unsigned long uptime = sys_info.uptime /60;// 启动时间
		unsigned long avgCpuLoad = sys_info.loads[0] /1000;// CPU占用
		unsigned long totalRam = sys_info.totalram / 1024 / 1024;
		unsigned long freeRam = sys_info.freeram /1024 /1024;// 内存使用量及占用
		unsigned int usedRam = totalRam - freeRam;
		unsigned int ram_load = (usedRam * 100) / totalRam;

		sprintf(cpuInfo, "CPU:%ld%%", avgCpuLoad);
		sprintf(uptimeInfo, "Up:%ldmin", uptime);
		sprintf(ramInfo, "RAM:%.3dM %.2d%%", usedRam,ram_load);
		sprintf(tempInfo, "TEMP:%.2dC", get_temp());// 树莓派CPU温度

		getip(ipInfo);//IP 信息

//	printf("GUI Display_parts String \r\n");
	//开始显示
	//各种系统信息显示
	GUI_DrawRectangle(5,20,127,36,BLACK,1,1);
	GUI_DisString_EN(5,20,uptimeInfo,&Font16,GUI_BACKGROUND,RED);
	GUI_DrawRectangle(5,78,61,93,BLACK,1,1);
	GUI_DisString_EN(5,78,    cpuInfo,&Font12,GUI_BACKGROUND,YELLOW);
	GUI_DrawRectangle(63,78,127,93,BLACK,1,1);
	GUI_DisString_EN(64,78, tempInfo,&Font12,GUI_BACKGROUND,RED);
	GUI_DrawRectangle(5,96,127,109,BLACK,1,1);
	GUI_DisString_EN(5,96,    ramInfo,&Font12,GUI_BACKGROUND,GBLUE);
	GUI_DrawRectangle(5,114,127,127,BLACK,1,1);
	GUI_DisString_EN(5,114,       ipInfo,&Font12,GUI_BACKGROUND,GREEN);
}
}