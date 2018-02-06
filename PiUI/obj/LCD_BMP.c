#include "LCD_BMP.h"
#include "LCD_Driver.h"
#include <stdio.h>	//fseek fread
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>	//memset
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

extern LCD_DIS sLCD_DIS;

uint8_t LCD_ShowBmp(int keys)
{
    DIR *dir;
    struct dirent *ptr;
	char filedirname[30]="";
	FILE *fp;                     //Define a file pointer 
    BMPFILEHEADER bmpFileHeader;  //Define a bmp file header structure
    BMPINF bmpInfoHeader;         //Define a bmp bitmap header structure 

	if(keys==0) dir = opendir("./pic/");
	if(keys==4) dir = opendir("./picself/");
	if(keys==7) dir = opendir("./netpic/");
	while((ptr = readdir(dir)) != NULL)
{// fp = 0x00426aa0
	if(keys==0) sprintf(filedirname,"./pic/%s",      ptr->d_name);
	if(keys==4) sprintf(filedirname,"./picself/%s",ptr->d_name);
	if(keys==7) sprintf(filedirname,"./netpic/%s",ptr->d_name);
//	printf("%s\n",filedirname);

	if(strstr(filedirname,".bmp")==NULL) {printf("Cann't open the bmp file!\n");continue;}//continue继续下一条，break跳出循环

	if((fp = fopen(filedirname, "rb")) == NULL) {printf("Cann't open the file!\n");return 0;}

    //Set the file pointer from the beginning
    fseek(fp, 0, SEEK_SET);                            // fp = 0x00426aa0
    fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp);//	sizeof(BMPFILEHEADER) must be 14,
	fread(&bmpInfoHeader, sizeof(BMPINF), 1, fp);

	int row, col;
    short data;
	RGBQUAD rgb;
	int len = bmpInfoHeader.bBitCount / 8;    //RGB888,one 3 byte = 1 bitbmp
	
	//get bmp data and show
	fseek(fp, bmpFileHeader.bOffset, SEEK_SET); 	
	LCD_SetGramScanWay(U2D_L2R);
    LCD_SetWindows(0, 0, sLCD_DIS.LCD_Dis_Column, sLCD_DIS.LCD_Dis_Page);
    for(row = 0; row < bmpInfoHeader.bHeight; row++) {
        for(col = 0; col < bmpInfoHeader.bWidth; col++) {
			if(fread((char *)&rgb, 1, len, fp) != len){
				//perror("get bmpdata:\r\n");
				continue;
			}
            data = RGB((rgb.rgbRed), (rgb.rgbGreen), (rgb.rgbBlue));
            LCD_SetColor(data, 1, 1);
        }
    }
	fclose(fp); 
	sleep(1);
	}
	closedir(dir); 
	LCD_SetGramScanWay(SCAN_DIR_DFT);
 return 0;
}
