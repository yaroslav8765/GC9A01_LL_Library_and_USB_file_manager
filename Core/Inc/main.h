/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "math.h"
#include "fonts.h"
#include "GC9A01.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void LEFT_button_handrel();
void RIGHT_button_handrel();
void UP_button_handrel();
void DOWN_button_handrel();

void view_menu_DOWN_button_handler();
void view_menu_UP_button_handler();
void view_menu_LEFT_button_handler();
void view_menu_RIGHT_button_handler();

void view_txt_DOWN_button_handler();
void view_txt_UP_button_handler();
void view_txt_LEFT_button_handler();
void view_txt_RIGHT_button_handler();

void view_image_DOWN_button_handler();
void view_image_UP_button_handler();
void view_image_LEFT_button_handler();
void view_image_RIGHT_button_handler();

void check_for_USB_storage_connection();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define BUTTON_Pin LL_GPIO_PIN_0
#define BUTTON_GPIO_Port GPIOA
#define DC_Pin LL_GPIO_PIN_2
#define DC_GPIO_Port GPIOA
#define RES_Pin LL_GPIO_PIN_3
#define RES_GPIO_Port GPIOA
#define CS_Pin LL_GPIO_PIN_10
#define CS_GPIO_Port GPIOB
#define LEFT_Pin LL_GPIO_PIN_12
#define LEFT_GPIO_Port GPIOB
#define RIGHT_Pin LL_GPIO_PIN_13
#define RIGHT_GPIO_Port GPIOB
#define UP_Pin LL_GPIO_PIN_14
#define UP_GPIO_Port GPIOB
#define DOWN_Pin LL_GPIO_PIN_15
#define DOWN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
