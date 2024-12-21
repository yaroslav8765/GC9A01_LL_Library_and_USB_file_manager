#include "File_Handling.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "cg9a01.h"

extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

extern Device_mode current_mode;
FILINFO USBHfno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pUSBHFatFS;
DWORD fre_clust;
uint32_t total, free_space;


void compress_row(uint16_t *row1, uint16_t *row2, uint16_t *output, uint8_t interpolation) {
    for (int x = 0; x < 240; x++) {
        int x_src1 = x * interpolation; // ?????? ??????? ???????
        int x_src2 = x_src1 + 1;       // ?????? ??????? ???????
        // ????????? ?????? ? ????????? ???????? ??????
        uint16_t pixel1 = (row1[x_src1] + row2[x_src1]) / 2;
        uint16_t pixel2 = (row1[x_src2] + row2[x_src2]) / 2;
        output[x] = (pixel1 + pixel2) / 2;
    }
}

FRESULT read_BMP_header(BITMAPFILEHEADER *fileheader,BITMAPINFOHEADER *infoheader){
	fresult = f_read(&USBHFile, fileheader, sizeof(BITMAPFILEHEADER), &br);
  if (fresult != FR_OK) {
		f_close(&USBHFile);
    GC9A01_Text("ERROR in reading header ...  \n Press any button",1);
		current_mode = error;
		return fresult; 
	}
	fresult = f_read(&USBHFile, infoheader, sizeof(BITMAPINFOHEADER), &br);
	if (fresult != FR_OK ) {
		f_close(&USBHFile);
		GC9A01_Text("ERROR in reading header  ...  \n Press any button",1);
		current_mode = error;
		return fresult; 
	}
	
}

FRESULT f_lseek_with_error_message(uint32_t offset, uint32_t file_size){
	if (offset >= file_size) {
		GC9A01_Text("Seek error! \n Press any button", 1);
		current_mode = error;	 
		}			
	fresult = f_lseek(&USBHFile, offset);
	if (fresult != FR_OK) {
		GC9A01_Text("Seek error! \n Press any button", 1);
		current_mode = error;
		return fresult;
	}
}


void Mount_USB (void)
{
	fresult = f_mount(&USBHFatFS, USBHPath, 1);
	if (fresult != FR_OK){
		GC9A01_Text("ERROR in mounting USB ...  \n Press any button",1);
		current_mode = error;
	}
}

void Unmount_USB (void)
{
	fresult = f_mount(NULL, USBHPath, 1);
	if (fresult != FR_OK){
		GC9A01_Text("ERROR in UNMOUNTING USB ... \n Press any button",1);
		current_mode = error;
	} 
}

FRESULT check_if_file_exist(char *name){
	 fresult = f_stat(name, &USBHfno);
		if (fresult != FR_OK) {
			char *buf = malloc(100 * sizeof(char));
			sprintf(buf, "ERROR!!! *%s* does not exist \n Press any button", name);
			current_mode = error;
			GC9A01_Text(buf, 1);
			free(buf);
		}
		return fresult;
}

FRESULT check_if_file_opens(char *name){
	fresult = f_open(&USBHFile, name, FA_READ);
  if (fresult != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
		sprintf(buf, "ERROR!!! No. %d in opening file *%s* \n Press any button", fresult, name);
		current_mode = error;
    GC9A01_Text(buf, 1);
    free(buf);
	}
	return fresult;
}

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
	
  if (check_if_file_exist(name) != FR_OK) {
    return fresult;
	}
	if (check_if_file_opens(name) != FR_OK) {
    return fresult;
	}


	file_size = f_size(&USBHFile);

	char *buffer2 = malloc(file_size);
  if (buffer2 == NULL) {
		GC9A01_Text("Memory allocation failed\n\n", 1);
		current_mode = error;
    f_close(&USBHFile);
    return FR_NOT_ENOUGH_CORE;
	}

	fresult = f_read(&USBHFile, buffer2, file_size, &br);
	if (fresult != FR_OK) {
		char *buf = malloc(100 * sizeof(char));
    sprintf(buf, "ERROR!!! No. %d in reading file *%s* \n Press any button", fresult, name);
    GC9A01_Text(buf, 1);
		current_mode = error;
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

#define temp_height 1

FRESULT Read_File_and_print_BMP(char *name, uint16_t *horizontal_offset, uint16_t *vertical_offset, uint16_t interpolation) {
  uint32_t file_size;
	uint16_t column = 0;
	unsigned short buffer1[LCD_W*interpolation];
	unsigned short buffer2[LCD_W*interpolation];
	unsigned short output_row[LCD_W];
	BITMAPFILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;
	uint32_t offset = fileHeader.bfOffBits 							+		\
                    ((infoHeader.biWidth * 2) 				* 	\
										(column + (*vertical_offset) ) 		+		\
                    (*horizontal_offset * 2));
  if (check_if_file_exist(name) != FR_OK) {
    return fresult;
	}
	if (check_if_file_opens(name) != FR_OK) {
    return fresult;
	}

	file_size = f_size(&USBHFile);
	read_BMP_header(&fileHeader,&infoHeader);

/*******************MY IDEAS*********/
	

		
/*************************************************************************/
	
	
	

/***************************BOTTOM-TOP CASE*******************************/
	if(fileHeader.bfType == 0x4D42 && infoHeader.biBitCount == 16 && infoHeader.biHeight > 0 ){
		
		if(infoHeader.biWidth	/ interpolation <= LCD_W || infoHeader.biHeight/ interpolation <= LCD_H){	
			if(infoHeader.biWidth / interpolation <= LCD_W ){	
				*horizontal_offset = 0;
			}
			if( (infoHeader.biHeight) / interpolation <= LCD_H){
				*vertical_offset = 0;
			}
		}
		else {
			if(*vertical_offset > (infoHeader.biHeight/interpolation) - LCD_H ){
				*vertical_offset = (infoHeader.biHeight/interpolation) - LCD_H;
			}
			if(*horizontal_offset > (infoHeader.biWidth/interpolation) - LCD_W){
				*horizontal_offset = (infoHeader.biWidth/interpolation) - LCD_W;
			}
		}
		
/****************************************************************************/	
		
		
		
		
		
		
		
		
		//fresult = f_lseek_with_error_message(offset, file_size);
				
		//fresult = f_read(&USBHFile, buffer1 + (LCD_W * (interpolation - 1)), infoHeader.biWidth < LCD_W * interpolation ? infoHeader.biWidth * 2 : LCD_W * interpolation * 2, &br);
		
		for (column = 1 ; column + 1<= LCD_H; column +=interpolation) {
			if (infoHeader.biWidth * column * 2 >= file_size) break;

				
				offset = fileHeader.bfOffBits 											+		\
                          ((infoHeader.biWidth * 2) 				* 	\
													(column + (*vertical_offset) ) 		+		\
                          (*horizontal_offset * 2));
				
				fresult = f_lseek_with_error_message(offset, file_size);
				
				fresult = f_read(&USBHFile, buffer2 + (LCD_W * (interpolation - 1)), infoHeader.biWidth < LCD_W * interpolation ? infoHeader.biWidth * 2 : LCD_W * interpolation * 2, &br);
				
				if (fresult != FR_OK) {
					GC9A01_Text("Read error! \n Press any button", 1);
					current_mode = error;
					break;
				}
			compress_row(buffer1, buffer2, output_row, interpolation);
			GC9A01_show_picture(output_row, 0, ((LCD_H - 1) - column), LCD_W, 1, LCD_W, 1);
			memcpy(buffer1, output_row, sizeof(buffer1));
		}

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
/***************************TOP-BOTTOM CASE****************************/
	} else if(fileHeader.bfType == 0x4D42 && infoHeader.biBitCount == 16 && infoHeader.biHeight < 0){
		
		infoHeader.biHeight = infoHeader.biHeight * (-1);
		uint16_t vertical_offset_2 = 0;
		
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
		
		vertical_offset_2 = infoHeader.biHeight - *vertical_offset - LCD_H;
		uint32_t row_size = ((infoHeader.biWidth * 2 + 3) / 4) * 4;

		for (uint16_t column = 0; column < LCD_H; column++) {
				if (fileHeader.bfOffBits + row_size * column >= file_size) break;

				fresult = f_lseek(&USBHFile, fileHeader.bfOffBits + (row_size * (column + vertical_offset_2)) + (*horizontal_offset * 2));
				if (fresult != FR_OK) {
						GC9A01_Text("Seek error! \n Press any button", 1);
						current_mode = error;
						break;
				}
				//if infoHeader.biWidth < LCD_W * Interpotation -> read image_W * 2, otherwise read LCD_W * interpolation * 2 // pizda
				//fresult = f_read(&USBHFile, buffer2, infoHeader.biWidth < LCD_W * interpolation ? infoHeader.biWidth * 2 : LCD_W * interpolation * 2, &br);
				if (fresult != FR_OK) {
						GC9A01_Text("Read error! \n Press any button", 1);
						current_mode = error;
						break;
				}

				GC9A01_show_picture(buffer2, 0, column, LCD_W, 1, LCD_W, 1);
		}
	
		
/****************************OTHER CASES*****************************/
	}else {
		GC9A01_Text("Please, use only 16-bit ccolors  \n Press any button", 1);
		current_mode = error;
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
