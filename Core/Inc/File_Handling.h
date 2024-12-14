#ifndef FILE_HANDLING_H_
#define FILE_HANDLING_H_

#include "fatfs.h"
#include "string.h"
#include "stdio.h"
#include "main.h"

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
FRESULT Scan_USB (char* pat, struct MenuMember *member, uint8_t page);
uint8_t Scan_USB_for_amount_of_files(char *pat);
FRESULT Read_File (char *name, uint8_t page,char *buffer, uint16_t lenght);
FRESULT Read_File_and_print_BMP(char *name, uint16_t *horizontal_offset, uint16_t *vertical_offset);
uint8_t get_depth_of_dir(char *path);



#endif