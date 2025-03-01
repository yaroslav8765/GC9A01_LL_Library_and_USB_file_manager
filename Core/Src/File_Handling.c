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

void clear_row(uint16_t *row, uint16_t lenght){
	for (int x = 0; x < lenght; x++) {
			row[x] = 0xFFFF;
		}
}

void calculate_shift(	uint16_t infoHeight, 	uint8_t interpolation, 																										\
											uint16_t LCDHeight, 	uint16_t *vertical_offset, 																								\
											uint16_t *v_shift, 		uint16_t mem_vertical_offset) {
    if ((infoHeight / interpolation) >= LCDHeight) {
        *v_shift = 0;
        *vertical_offset = mem_vertical_offset;
    } else if ((*vertical_offset / 10) >= (LCDHeight - (infoHeight / interpolation))) {
        *v_shift = LCDHeight - (infoHeight / interpolation) + 1;
        *vertical_offset = (LCDHeight - (infoHeight / interpolation)) * 10;
    } else {
        *v_shift = *vertical_offset / 10;
    }
}
void calculate_mem_offset(uint16_t size, uint16_t interpolation, uint16_t LCDsize, uint16_t *mem_horizontal_offset) {
    if (size / interpolation <= LCDsize) {
        *mem_horizontal_offset = 0;
    } else if (*mem_horizontal_offset > ((size / interpolation) - LCDsize)) {
        *mem_horizontal_offset = (size / interpolation) - LCDsize;
    }
}


void compress_row(uint16_t *row1, 						uint16_t *row2,																													\
									uint16_t *output, 					uint8_t interpolation, 																									\
									uint16_t horizontal_offset, uint16_t image_width) {
		clear_row(output,LCD_W);
    for (int x = 0; x <= LCD_W; x++) {
        int x_src1 = x * interpolation;
        int x_src2 = x_src1 + 1;
				int16_t free_pixels = LCD_W - (image_width/interpolation);
        uint16_t pixel1_row1 = row1[x_src1];
        uint16_t pixel2_row1 = row1[x_src2];
        uint16_t pixel1_row2 = row2[x_src1];
        uint16_t pixel2_row2 = row2[x_src2];

        uint8_t r1 = ((pixel1_row1 >> 11) & 0x1F);
        uint8_t g1 = ((pixel1_row1 >> 5) & 0x3F);
        uint8_t b1 = (pixel1_row1 & 0x1F);

        uint8_t r2 = ((pixel2_row1 >> 11) & 0x1F);
        uint8_t g2 = ((pixel2_row1 >> 5) & 0x3F);
        uint8_t b2 = (pixel2_row1 & 0x1F);

        uint8_t r3 = ((pixel1_row2 >> 11) & 0x1F);
        uint8_t g3 = ((pixel1_row2 >> 5) & 0x3F);
        uint8_t b3 = (pixel1_row2 & 0x1F);

        uint8_t r4 = ((pixel2_row2 >> 11) & 0x1F);
        uint8_t g4 = ((pixel2_row2 >> 5) & 0x3F);
        uint8_t b4 = (pixel2_row2 & 0x1F);

        uint8_t r = (r1 + r2 + r3 + r4) / 4;
        uint8_t g = (g1 + g2 + g3 + g4) / 4;
        uint8_t b = (b1 + b2 + b3 + b4) / 4;
				output[x + (free_pixels <= 0 ? 0 : horizontal_offset)] = (r << 11) | (g << 5) | b;
				if((x + free_pixels) >=LCD_W-1) break;
    }
}


void compress_array(unsigned short *array, unsigned short *result,uint16_t size, uint16_t coefficient){
	for(uint16_t i = 0; i!=size; i++){
		result[i] = array[i*coefficient];
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
						if (!(strcmp("SYSTEM~1", USBHfno.fname)) || !(strcmp("System Volume Information",USBHfno.fname))) continue;
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

void process_bottom_to_top(	BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, 																\
														uint16_t *horizontal_offset, 	uint16_t *vertical_offset, 																		\
														uint16_t interpolation, 			uint32_t file_size, 																					\
														unsigned short *buffer1, 			unsigned short *buffer2, 																			\
														unsigned short *output_row) {
    uint32_t offset;
    uint16_t h_shift = 0, v_shift = 0;
    uint16_t mem_horizontal_offset = *horizontal_offset;
    uint16_t mem_vertical_offset = *vertical_offset;

    calculate_mem_offset(infoHeader->biWidth, interpolation, LCD_W, &mem_horizontal_offset);
    calculate_mem_offset(infoHeader->biHeight, interpolation, LCD_H, &mem_vertical_offset);

    for (uint16_t column = 1; column <= LCD_H; column++) {
        if (infoHeader->biWidth * column * 2 >= file_size) break;

        if (column <= infoHeader->biHeight / interpolation) {
            offset = calculate_offset(fileHeader, infoHeader, column, 																								\
						mem_horizontal_offset, mem_vertical_offset, interpolation);
					
            read_and_compress_row(offset, 							file_size, 	buffer1, 																					\
																	buffer2, 							output_row,	interpolation, 																		\
																	infoHeader->biWidth,	&h_shift, 	horizontal_offset);
        } else {
            clear_row(output_row, LCD_W);
        }

        calculate_shift(infoHeader->biHeight, interpolation, LCD_H, vertical_offset, &v_shift, mem_vertical_offset);
        GC9A01_show_picture(output_row, 0, ((LCD_H - 1) - (column - 1)) - v_shift, LCD_W, 1, LCD_W, 1);
        memcpy(buffer1, buffer2, sizeof(unsigned short) * MAX_BUFFER_SIZE);
    }
}
														


void process_top_to_bottom(	BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, 															\
														uint16_t *horizontal_offset, 	uint16_t *vertical_offset,																	\
														uint16_t interpolation, 			uint32_t file_size,																					\
														unsigned short *buffer1,			unsigned short *buffer2, 																		\
														unsigned short *output_row) {
			
    uint32_t offset;
    uint16_t h_shift = 0, v_shift = 0;
    uint16_t mem_horizontal_offset = *horizontal_offset;
    uint16_t mem_vertical_offset = *vertical_offset;

    calculate_mem_offset(infoHeader->biWidth, interpolation, LCD_W, &mem_horizontal_offset);
    calculate_mem_offset(infoHeader->biHeight, interpolation, LCD_H, &mem_vertical_offset);

    for (int16_t column = LCD_H; column >= 0; column--) {
        if (infoHeader->biWidth * column * 2 >= file_size) break;

        if (column <= infoHeader->biHeight / interpolation) {
        offset = (fileHeader->bfOffBits + 																																						\
				((((infoHeader->biWidth * 2 + 3) / 4) * 4) * 																																	\
				(column+ (mem_vertical_offset)) +																																							\
				(mem_horizontal_offset*2))*interpolation);
            read_and_compress_row(offset, file_size, buffer1, buffer2, output_row,																		\
						interpolation, infoHeader->biWidth, &h_shift, horizontal_offset);
        } else {
            clear_row(output_row, LCD_W);
        }

        calculate_shift(infoHeader->biHeight, interpolation, LCD_H, vertical_offset, &v_shift, mem_vertical_offset);
        GC9A01_show_picture(output_row, 0, column + v_shift, LCD_W, 1, LCD_W, 1);
        memcpy(buffer1, buffer2, sizeof(unsigned short) * MAX_BUFFER_SIZE);
    }
}

uint32_t calculate_offset(BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, 																\
													uint16_t column, 							uint16_t mem_horizontal_offset,																\
													uint16_t mem_vertical_offset, uint16_t interpolation) {
														
    return fileHeader->bfOffBits		+ (((infoHeader->biWidth * 2) 	* (column + mem_vertical_offset))									\
																		+ (mem_horizontal_offset * 2)) 	* interpolation;
}

void read_and_compress_row(	uint32_t offset, 						uint32_t file_size,																						\
														unsigned short *buffer1,		unsigned short *buffer2,																			\
														unsigned short *output_row, uint16_t interpolation, 																			\
														uint16_t image_width, 			uint16_t *h_shift, 																						\
														uint16_t *horizontal_offset) {
															
    f_lseek_with_error_message(offset, file_size);

    f_read(&USBHFile, buffer2, image_width < LCD_W * interpolation ? image_width * 2 : LCD_W * interpolation * 2, &br);

    calculate_shift(image_width, interpolation, LCD_W, horizontal_offset, h_shift, *horizontal_offset);
    compress_row(buffer1, buffer2, output_row, interpolation, *h_shift, image_width);
}


FRESULT Read_File_and_print_BMP(char *name, 								uint16_t *horizontal_offset,															\
																uint16_t *vertical_offset, 	uint16_t interpolation) {
    uint32_t file_size;
    uint16_t column = 0;
    uint16_t mem_horizontal_offset = *horizontal_offset;
    uint16_t mem_vertical_offset = *vertical_offset;
    uint16_t h_shift = 0;
    uint16_t v_shift = 0;
    unsigned short buffer1[MAX_BUFFER_SIZE];
    unsigned short buffer2[MAX_BUFFER_SIZE];
    unsigned short output_row[LCD_W];
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    uint32_t offset;

    if (check_if_file_exist(name) != FR_OK || check_if_file_opens(name) != FR_OK) {
        return fresult;
    }

    file_size = f_size(&USBHFile);
    read_BMP_header(&fileHeader, &infoHeader);

    if (fileHeader.bfType != 0x4D42 || infoHeader.biBitCount != 16) {
        GC9A01_Text("Unsupported BMP format. Only 16-bit colors are supported.", 1);
        current_mode = error;
        f_close(&USBHFile);
        return FR_INVALID_OBJECT;
    }

    if (infoHeader.biHeight > 0) {
        process_bottom_to_top(&fileHeader, 				&infoHeader,																												\
															horizontal_offset, 	vertical_offset,																										\
															interpolation, 			file_size,																													\
															buffer1, 						buffer2, 																														\
															output_row);
    } else {
        infoHeader.biHeight = -infoHeader.biHeight;
        process_top_to_bottom(&fileHeader, 				&infoHeader, 																												\
															horizontal_offset, 	vertical_offset,																										\
															interpolation, 			file_size, 																													\
															buffer1, 						buffer2,																														\
															output_row);
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
