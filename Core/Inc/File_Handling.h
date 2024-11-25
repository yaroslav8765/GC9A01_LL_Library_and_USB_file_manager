/*
 * File_Handling_RTOS.h
 *
 *  Created on: 30-April-2020
 *      Author: Controllerstech
 */

#ifndef FILE_HANDLING_H_
#define FILE_HANDLING_H_

#include "fatfs.h"
#include "string.h"
#include "stdio.h"
#include "main.h"

/* mounts the USB*/
void Mount_USB (void);

/* unmounts the USB*/
void Unmount_USB (void);

/* Start node to be scanned (***also used as work area***) */
FRESULT Scan_USB (char* pat, struct MenuMember *member, uint8_t page);
uint8_t Scan_USB_for_amount_of_files(char *pat);
/* Only supports removing files from home directory. Directory remover to be added soon */

/* read data from the file
 * @ name : is the path to the file*/
FRESULT Read_File (char *name, uint8_t page,char *buffer, uint16_t lenght);
FRESULT Read_File_and_print_BMP(char *name, uint16_t horizontal_offset, uint16_t vertical_offset);
uint8_t get_depth_of_dir(char *path);



#endif /* FILE_HANDLING_RTOS_H_ */
