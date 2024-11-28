/*
 * File_Handling_RTOS.c
 *
 *  Created on: 26-June-2020
 *      Author: Controllerstech.com
 */

#include "File_Handling.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "cg9a01.h"
//extern UART_HandleTypeDef huart2;
#define UART &huart2



/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */



extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

FILINFO USBHfno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pUSBHFatFS;
DWORD fre_clust;
uint32_t total, free_space;


void Send_Uart (char *string)
{
	
}

void Send_Uart_list (char *string, uint8_t coordinats)
{
	GC9A01_SetTextColor(WHITE);
	GC9A01_SetFont(&Font16);
	uint8_t x = 10;
	size_t length = strlen(string);  // ????? ??????
  for (size_t i = 0; i < length - 1; i++)  // ??????????????? ????? ????????? ????????
  {
    GC9A01_DrawChar(x, coordinats+50, string[i]);
    x += 10;
  }

	//HAL_UART_Tranmit(UART, (uint8_t *)string, strlen (string), HAL_MAX_DELAY);
}

void Mount_USB (void)
{
	fresult = f_mount(&USBHFatFS, USBHPath, 1);
	if (fresult != FR_OK) Send_Uart ("ERROR!!! in mounting USB ...\n\n");
	else Send_Uart("USB mounted successfully...\n");
}

void Unmount_USB (void)
{
	fresult = f_mount(NULL, USBHPath, 1);
	if (fresult == FR_OK) Send_Uart ("USB UNMOUNTED successfully...\n\n\n");
	else Send_Uart("ERROR!!! in UNMOUNTING USB \n\n\n");
}

/* Start node to be scanned (***also used as work area***) */
uint8_t Scan_USB_for_amount_of_files(char *pat) {
    uint8_t file_num = 0;
    DIR dir;
    FILINFO USBHfno;
    FRESULT fresult;

    char *path = malloc(256 * sizeof(char));
    if (path == NULL) {
        return 0;
    }

    snprintf(path, 256, "%s", pat);

    fresult = f_opendir(&dir, path);
    if (fresult == FR_OK) {
        for (;;) {
            fresult = f_readdir(&dir, &USBHfno); 
            if (fresult != FR_OK || USBHfno.fname[0] == 0) {
                break; 
            }
            
            if (!(USBHfno.fattrib & AM_DIR)) {
                file_num++; 
            }
        }
        f_closedir(&dir);
    }

    free(path);
    return file_num;
}



FRESULT Scan_USB (char* pat, struct MenuMember *member, uint8_t page)
{
    uint8_t file_num = 0;
    DIR dir;
   // UINT i;
	  page --;
    char path[256];
    sprintf(path, "%s", pat);
    fresult = f_opendir(&dir, path);
    if (fresult == FR_OK)
    {
        char buf[30];

        for (;;)
        {       
            fresult = f_readdir(&dir, &USBHfno); 
            if (fresult != FR_OK || USBHfno.fname[0] == 0) break; 
            
            if (file_num >= page * AMOUNT_OF_MENU_MEMBERS && file_num < (page + 1) * AMOUNT_OF_MENU_MEMBERS)
            {
                if (USBHfno.fattrib & AM_DIR) 
                {
                    if (!(strcmp("SYSTEM~1", USBHfno.fname)) || !(strcmp("System Volume Information", USBHfno.fname))) continue;
                    snprintf(buf, sizeof(buf), "%s.DIR", USBHfno.fname);
                }
                else 
                {
                    snprintf(buf, sizeof(buf), "%s", USBHfno.fname);
                }
                
           
                strcpy(member[file_num - (page * AMOUNT_OF_MENU_MEMBERS)].text, buf);
                member[file_num - (page * AMOUNT_OF_MENU_MEMBERS)].number = file_num + 1;
            }
            file_num++;
        }
        f_closedir(&dir);
    }

    for (uint8_t i = file_num - (page * AMOUNT_OF_MENU_MEMBERS); i < AMOUNT_OF_MENU_MEMBERS; i++)
    {
        strcpy(member[i].text, "empty");
        member[i].number = 0;
    }

    return fresult;
}


FRESULT Read_File(char *name, uint8_t page, char *buffer, uint16_t lenght) {
    uint32_t file_size;

    fresult = f_stat(name, &USBHfno);
    if (fresult != FR_OK) {
        char *buf = malloc(100 * sizeof(char));
        sprintf(buf, "ERROR!!! *%s* does not exist\n\n", name);
        GC9A01_Text(buf, 1);
        free(buf);
        return fresult;
    }

    fresult = f_open(&USBHFile, name, FA_READ);
    if (fresult != FR_OK) {
        char *buf = malloc(100 * sizeof(char));
        sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
        GC9A01_Text(buf, 1);
        free(buf);
        return fresult;
    }

    file_size = f_size(&USBHFile);

    char *buffer2 = malloc(file_size);
    if (buffer2 == NULL) {
        GC9A01_Text("Memory allocation failed\n\n", 1);
        f_close(&USBHFile);
        return FR_NOT_ENOUGH_CORE;
    }

    fresult = f_read(&USBHFile, buffer2, file_size, &br);
    if (fresult != FR_OK) {
        char *buf = malloc(100 * sizeof(char));
        sprintf(buf, "ERROR!!! No. %d in reading file *%s*\n\n", fresult, name);
        GC9A01_Text(buf, 1);
        free(buf);
        free(buffer2);
        f_close(&USBHFile);
        return fresult;
    }

		 memcpy(buffer, buffer2 + ((page-1) * lenght), lenght);

    free(buffer2);
    f_close(&USBHFile);

    return fresult;
}


typedef struct __attribute__((packed)) {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct __attribute__((packed)) {
	uint32_t biSize;
	int32_t  biWidth;
	int32_t  biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;	
	uint32_t biSizeImage;
	int32_t  biXPelsPerMeter;
	int32_t  biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} BITMAPINFOHEADER;


FRESULT Read_File_and_print_BMP(char *name, uint16_t *horizontal_offset, uint16_t *vertical_offset) {
  uint32_t file_size;
	uint16_t shift_H = LCD_H + 0;
	uint16_t shift_V = LCD_W + 0;
	unsigned short buffer2[LCD_W];
	BITMAPFILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;

  fresult = f_stat(name, &USBHfno);
  if (fresult != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
    sprintf(buf, "ERROR!!! *%s* does not exist\n\n", name);
    GC9A01_Text(buf, 1);
    free(buf);
    return fresult;
	}

  fresult = f_open(&USBHFile, name, FA_READ);
  if (fresult != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
    GC9A01_Text(buf, 1);
    free(buf);
    return fresult;
	}

	file_size = f_size(&USBHFile);
		
		
/*******************************READ BMP HEADER***************************/
	fresult = f_read(&USBHFile, &fileHeader, sizeof(BITMAPFILEHEADER), &br);
  if (fresult != FR_OK) {
		f_close(&USBHFile);
    return fresult;
	}
	fresult = f_read(&USBHFile, &infoHeader, sizeof(BITMAPINFOHEADER), &br);
	if (fresult != FR_OK ) {
		f_close(&USBHFile);
		return fresult;
	}
	
	if(fileHeader.bfType == 0x4D42 || infoHeader.biBitCount != 16 ){
			
		if(infoHeader.biWidth < LCD_W && infoHeader.biHeight < LCD_H){
			vertical_offset = 0;
			horizontal_offset = 0;
		} else {
			if(*vertical_offset > infoHeader.biHeight - LCD_H ){
				*vertical_offset = infoHeader.biHeight - LCD_H;
			}
			if(*horizontal_offset > infoHeader.biWidth - LCD_W){
				*horizontal_offset = infoHeader.biWidth - LCD_W;
			}
		}
			
		for (uint16_t column = LCD_H; column >= 0; column--) {
			if (infoHeader.biWidth * column * 2 >= file_size) break;

			fresult = f_lseek(&USBHFile,64 + ((infoHeader.biWidth*2) * (column+ (*vertical_offset)) + (*horizontal_offset*2)));
			if (fresult != FR_OK) {
				GC9A01_Text("Seek error!\n", 1);
				break;
			}

			fresult = f_read(&USBHFile, buffer2, LCD_W*2, &br);
			if (fresult != FR_OK) {
				GC9A01_Text("Read error!\n", 1);
				break;
			}
					
			GC9A01_show_picture(buffer2, 0, (LCD_H-1) - column, LCD_W, 1, LCD_W, 1);
		}
	} else {
		GC9A01_Text("Wrong with rows direction!\n", 1);
	}
		f_close(&USBHFile);
    return fresult;
}









uint8_t get_depth_of_dir(char *path){
	uint8_t result = 0;
	 for (int i = strlen(path) - 1; i >= 0; i--){
		if(path[i] == '/'){
			result++;
		}
	}
	return result;
}
