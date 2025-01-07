#ifndef FILE_HANDLING_H_
#define FILE_HANDLING_H_

#include "fatfs.h"
#include "string.h"
#include "stdio.h"
#include "main.h"

#define MAX_INTERPOLATION_COEF 10
#define MAX_BUFFER_SIZE MAX_INTERPOLATION_COEF*LCD_W
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


void Mount_USB (void);

void Unmount_USB (void);

void clear_row(uint16_t *row, uint16_t lenght);

void calculate_shift(	uint16_t infoHeight, 	uint8_t interpolation,																										\
											uint16_t LCDHeight,		uint16_t *vertical_offset, 																								\
											uint16_t *v_shift, 		uint16_t mem_vertical_offset);

void calculate_mem_offset(uint16_t size, uint16_t interpolation, uint16_t LCDsize, uint16_t *mem_horizontal_offset);

void compress_row(uint16_t *row1, 						uint16_t *row2,																													\
									uint16_t *output, 					uint8_t interpolation, 																									\
									uint16_t horizontal_offset, uint16_t image_width);

void compress_array(unsigned short *array, unsigned short *result,uint16_t size, uint16_t coefficient);

FRESULT Scan_USB (char* pat, struct MenuMember *member, uint8_t page);

uint8_t Scan_USB_for_amount_of_files(char *pat);

FRESULT Read_File (char *name, uint8_t page,char *buffer, uint16_t lenght);

FRESULT Read_File_and_print_BMP(char *name, 								uint16_t *horizontal_offset,															\
																uint16_t *vertical_offset,	uint16_t interpolation);

uint8_t get_depth_of_dir(char *path);

uint32_t calculate_offset(BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, 																\
													uint16_t column, 							uint16_t mem_horizontal_offset,																\
													uint16_t mem_vertical_offset, uint16_t interpolation);

void read_and_compress_row(	uint32_t offset, 						uint32_t file_size, 																					\
														unsigned short *buffer1, 		unsigned short *buffer2, 																			\
														unsigned short *output_row, uint16_t interpolation,																				\
														uint16_t image_width, 			uint16_t *h_shift, 																						\
														uint16_t *horizontal_offset);



#endif