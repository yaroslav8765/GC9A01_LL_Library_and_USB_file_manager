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
            
            if (file_num >= page * 8 && file_num < (page + 1) * 8)
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
                
           
                strcpy(member[file_num - (page * 8)].text, buf);
                member[file_num - (page * 8)].number = file_num + 1;
            }
            file_num++;
        }
        f_closedir(&dir);
    }

    for (uint8_t i = file_num - (page * 8); i < 8; i++)
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

uint8_t get_depth_of_dir(char *path){
	uint8_t result = 0;
	 for (int i = strlen(path) - 1; i >= 0; i--){
		if(path[i] == '/'){
			result++;
		}
	}
	return result;
}
