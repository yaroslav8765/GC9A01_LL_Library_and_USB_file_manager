/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "GC9A01.h"
#include "font.h"
#include "File_Handling.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	extern ApplicationTypeDef Appli_state;
	extern uint8_t shift;
	volatile uint8_t dma_spi_fl1=0;
	uint32_t dma_spi_cnt=1;
	uint8_t current_active_menu_member = 0;
	uint8_t current_page = 1;
	uint8_t amount_of_files = 0;
	uint8_t txt_file_page = 1;
	uint8_t enable_menu_member_animation = 1;
	uint8_t enable_button_handling = 1;
	char buffer[256];
	char path[512] = "/";
	char path_txt[512];
	uint16_t horizontal_offset = 0; 
	uint16_t vertical_offset = 0;
	uint16_t interpolation_coef = 5;
	uint16_t screen_buf[240] = {0};
	FATFS fs;
	DIR dr;
	FIL fl;
	FRESULT res;
	
	struct MenuMember Members[8];

	usb_drive_state USB_Storage_state = disconected;
	usb_drive_state USB_Storage_last_state = disconected;
	Device_mode current_mode = view_file_menu;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
	void delay_ms(uint32_t ms);
	int generate_random_number(int min, int max);
	void display_test_animation(void);
	void ClearBuffer(char *buffer, size_t length);
	void clear_and_set_root(char *array, size_t size);
	void refresh_BMP_image(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	for(int i = AMOUNT_OF_MENU_MEMBERS; i >= 0; i--){
	strcpy(Members[i].text, "File ");
	Members[i].number = i + 1;
	Members[i].state = unactive;
	}
	Members[current_active_menu_member].state = active;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
//  MX_FATFS_Init();
//  MX_USB_HOST_Init();
//  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
	LL_DMA_ClearFlag_TC3(DMA2);
  LL_DMA_ClearFlag_TE3(DMA2);
	LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_3);
  LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_3);
	LL_SPI_EnableDMAReq_TX(SPI1);
	LL_SPI_Enable(SPI1);
	
	GC9A01_Initial();
	GC9A01_ClearScreen(WHITE);
	GC9A01_SetBackColor(BLACK);
	GC9A01_SetFont(&Font16);
	GC9A01_SetTextColor(WHITE);
	
	MX_USB_HOST_Init();
	MX_FATFS_Init();
	
	while(Appli_state != APPLICATION_READY){
		MX_USB_HOST_Process();
	}
	Mount_USB();
	uint8_t files = Scan_USB_for_amount_of_files("/");
	Members[7].state = unactive;
	Scan_USB(path, Members,current_page);
	amount_of_files = Scan_USB_for_amount_of_files("/");
	Unmount_USB();
		
	MX_TIM1_Init();
	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableCounter(TIM1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		check_for_USB_storage_connection();
		if(enable_button_handling == 1){
			
			if(LL_GPIO_IsInputPinSet(UP_GPIO_Port,UP_Pin) == 1 && LL_GPIO_IsInputPinSet(DOWN_GPIO_Port,DOWN_Pin) == 1){
				UP_DOWN_button_handrel();
				shift = 0;
			}
			if(LL_GPIO_IsInputPinSet(LEFT_GPIO_Port,LEFT_Pin) == 1 && LL_GPIO_IsInputPinSet(RIGHT_GPIO_Port,RIGHT_Pin) == 1){
				LEFT_RIGHT_button_handrel();
				shift = 0;
			}
			if(LL_GPIO_IsInputPinSet(LEFT_GPIO_Port,LEFT_Pin) == 1){
				LEFT_button_handrel();
			}
			if(LL_GPIO_IsInputPinSet(RIGHT_GPIO_Port,RIGHT_Pin) == 1){
				RIGHT_button_handrel();
			}
			if(LL_GPIO_IsInputPinSet(UP_GPIO_Port,UP_Pin) == 1){
				UP_button_handrel();
				shift = 0;
			}
			if(LL_GPIO_IsInputPinSet(DOWN_GPIO_Port,DOWN_Pin) == 1){
				DOWN_button_handrel();
				shift = 0;
			}
			if(LL_GPIO_IsInputPinSet(BUTTON_GPIO_Port,BUTTON_Pin) == 0){
				BACK_TO_MENU_button_handler();
				shift = 0;
			}
		}
		
		if(enable_menu_member_animation == 1){
			menu_active_member_running_text_animation \
			(&Members[get_active_menu_member(Members)],get_active_menu_member(Members));
			delay_ms(5);
		}
		//GC9A01_show_picture(tohru1,50,50,140,140,140,140);
		//display_test_animation();
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_23, 354, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_23, 354, LL_RCC_PLLQ_DIV_8);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(96195652);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_3, LL_DMA_CHANNEL_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_3, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_3);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* TIM1 interrupt Init */
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = 1000;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 10000;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_UPDATE);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, DC_Pin|RES_Pin|LL_GPIO_PIN_10);

  /**/
  LL_GPIO_ResetOutputPin(CS_GPIO_Port, CS_Pin);

  /**/
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DC_Pin|RES_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LEFT_Pin|RIGHT_Pin|UP_Pin|DOWN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void delay_ms(uint32_t ms) {
    uint32_t delay_cycles = ms * 100000;
    while (delay_cycles--) {
        __asm__("nop"); 
    }
}

void ClearBuffer(char *buffer, size_t length) {
    if (buffer != NULL) {
        memset(buffer, 0, length);
    }
}

int generate_random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}

void clear_and_set_root(char *array, size_t size) {
    memset(array, 0, size);
    if (size > 1) {
        array[0] = '/';
        array[1] = '\0';
    }
}

void refresh_BMP_image(void){
	
	uint8_t active_member = get_active_menu_member(Members);	
	if(chek_menu_member_for_the_file_type(Members[active_member],"bmp") == 1){
		enable_menu_member_animation = 0;
		snprintf(path_txt, sizeof(path_txt), "%.*s/%s", (int)(strlen(path)), path, Members[active_member].text);
		current_mode = view_image;
		GC9A01_ClearScreen(WHITE);
		Mount_USB();
		Read_File_and_print_BMP(path_txt,&horizontal_offset,&vertical_offset,interpolation_coef );
		Unmount_USB();
	
	}
}

void display_test_animation(void){
		for(uint8_t i=0;i<10;i++)
    {
      GC9A01_DrawCircle(generate_random_number(10, 120),\
			generate_random_number(10, 120), generate_random_number(10, 70),\
			generate_random_number(0, 0xFFFF));
      delay_ms(20);
      GC9A01_DrawCircle(generate_random_number(10, 120),\
			generate_random_number(120, 210), generate_random_number(10, 70),\
			generate_random_number(0, 0xFFFF));
			delay_ms(20);
			GC9A01_DrawCircle(generate_random_number(120, 210),\
			generate_random_number(10, 120), generate_random_number(10, 70),\
			generate_random_number(0, 0xFFFF));
      delay_ms(20);
      GC9A01_DrawCircle(generate_random_number(120, 210),\
			generate_random_number(120, 210), generate_random_number(10, 70),\
			generate_random_number(0, 0xFFFF));
      delay_ms(20);
    }
		delay_ms(200);
		GC9A01_ClearScreen(WHITE);
		GC9A01_show_picture(tohru1,50,50,140,140,140,140);
		delay_ms(500);
		GC9A01_ClearScreen(WHITE);
		for(uint8_t i=0;i<10;i++)
    {
      GC9A01_draw_line(generate_random_number(0, 0xFFFF),\
			generate_random_number(10, 120), generate_random_number(10, 120),\
			generate_random_number(10, 210),generate_random_number(10, 210));
      delay_ms(20);
      GC9A01_draw_line(generate_random_number(0, 0xFFFF),\
			generate_random_number(10, 120), generate_random_number(120, 210),\
			generate_random_number(10, 210),generate_random_number(10, 210));
      delay_ms(20);
			GC9A01_draw_line(generate_random_number(0, 0xFFFF),\
			generate_random_number(120, 210), generate_random_number(10, 210),\
			generate_random_number(10, 210),generate_random_number(10, 210));
      delay_ms(20);
      GC9A01_draw_line(generate_random_number(0, 0xFFFF),\
			generate_random_number(120, 210), generate_random_number(10, 200),\
			generate_random_number(10, 210),generate_random_number(10, 210));
      delay_ms(20);
    }
		delay_ms(500);
		GC9A01_ClearScreen(WHITE);
		for(uint8_t i=0;i<10;i++)
    {
      GC9A01_DrawRect(generate_random_number(0, 0xFFFF),\
			generate_random_number(10, 120), generate_random_number(10, 120),\
			generate_random_number(10, 120),generate_random_number(10, 120));
      delay_ms(20);
      GC9A01_DrawRect(generate_random_number(0, 0xFFFF),\
			generate_random_number(10, 120), generate_random_number(120, 210),\
			generate_random_number(10, 120),generate_random_number(120, 210));
      delay_ms(20);
			GC9A01_DrawRect(generate_random_number(0, 0xFFFF),\
			generate_random_number(120, 210), generate_random_number(10, 210),\
			generate_random_number(120, 210),generate_random_number(10, 210));
      delay_ms(20);
      GC9A01_DrawRect(generate_random_number(0, 0xFFFF),\
			generate_random_number(120, 210), generate_random_number(10, 200),\
			generate_random_number(120, 210),generate_random_number(10, 210));
      delay_ms(20);
    }
		delay_ms(500);
		GC9A01_ClearScreen(WHITE);
		GC9A01_SetBackColor(BLACK);
		GC9A01_SetFont(&Font24);
		GC9A01_SetTextColor(WHITE);
		GC9A01_String(60,110,"Hello");
		GC9A01_Rainbow_String(60,130,"WORLD");
		delay_ms(500);
		GC9A01_ClearScreen(WHITE);
}

void UP_DOWN_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			
		break;
		case view_txt:
			
		break;
		case view_image:
			if(interpolation_coef < 10){
				interpolation_coef ++;
				refresh_BMP_image();
			}
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void LEFT_RIGHT_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			
		break;
		case view_txt:
			
		break;
		case view_image:
			if(interpolation_coef > 1){
				interpolation_coef --;
				refresh_BMP_image();
			}
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void LEFT_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			view_menu_LEFT_button_handler();
		break;
		case view_txt:
			view_txt_LEFT_button_handler();
		break;
		case view_image:
			view_image_LEFT_button_handler();
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void RIGHT_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			view_menu_RIGHT_button_handler();
		break;
		case view_txt:
			view_txt_RIGHT_button_handler();
		break;
		case view_image:
			view_image_RIGHT_button_handler();
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void UP_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			view_menu_UP_button_handler();
		break;
		case view_txt:
			view_txt_UP_button_handler();
		break;
		case view_image:
			view_image_UP_button_handler();
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void DOWN_button_handrel(){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	switch(current_mode){
		case view_file_menu:
			view_menu_DOWN_button_handler();
		break;
		case view_txt:
			view_txt_DOWN_button_handler();
		break;
		case view_image:
			view_image_DOWN_button_handler();
		break;
		case error:
			BACK_TO_MENU_button_handler();
		break;
	}
}

void BACK_TO_MENU_button_handler(){
	current_mode = view_file_menu;
	GC9A01_ClearScreen(WHITE);
	enable_menu_member_animation = 1;
	vertical_offset = 0;
	horizontal_offset = 0;
	interpolation_coef = 1;
	shift = 0;
	ShowMenu(Members, current_page);
}	
	
void view_menu_DOWN_button_handler(){
	if(USB_Storage_state == connected){
		enable_menu_member_animation = 1;
		if(current_active_menu_member<AMOUNT_OF_MENU_MEMBERS - 1){
			current_active_menu_member++;
			if(current_active_menu_member !=0){
				Members[current_active_menu_member-1].state = unactive;
				refresh_menu_member(Members[current_active_menu_member-1],current_active_menu_member-1);
			}
			Members[current_active_menu_member].state = active;
			refresh_menu_member(Members[current_active_menu_member],current_active_menu_member);
		}else{
			if(amount_of_files>=((current_page)*AMOUNT_OF_MENU_MEMBERS)){
				current_active_menu_member = 0;
				Members[current_active_menu_member].state = active;
				Members[AMOUNT_OF_MENU_MEMBERS - 1].state = unactive;
				current_page ++;
				Mount_USB();
				Scan_USB(path, Members,current_page);
				GC9A01_ClearScreen(WHITE);
				ShowMenu(Members,current_page);
				Unmount_USB();
			}
		} 
	}
}

void view_menu_UP_button_handler(){
	if(USB_Storage_state == connected){
		enable_menu_member_animation = 1;
		if (current_active_menu_member > 0) {
			current_active_menu_member--;
			Members[current_active_menu_member + 1].state = unactive;
			refresh_menu_member(Members[current_active_menu_member + 1], current_active_menu_member + 1);

			Members[current_active_menu_member].state = active;
			refresh_menu_member(Members[current_active_menu_member], current_active_menu_member);
		} else {
			if (current_page > 1) {
        current_page--;
				current_active_menu_member = AMOUNT_OF_MENU_MEMBERS - 1;
        Members[current_active_menu_member].state = active;
				Members[0].state = unactive;
			  Mount_USB();
        Scan_USB(path, Members, current_page);
        Unmount_USB();
				GC9A01_ClearScreen(WHITE);
        ShowMenu(Members, current_page);
			}
    }
	}
}

void view_menu_LEFT_button_handler() {
      if (strcmp("/", path) != 0) { 
        uint16_t len = strlen(path);

        if (path[len - 1] == '/') {
            path[len - 1] = '\0';
            len--;
        }

        for (uint16_t i = len - 1; i >= 0; i--) {
            if (path[i] == '/') {
                path[i] = '\0';
                break;
            }
        }
        if (strlen(path) == 0) {
            strcpy(path, "/");
        }
		Members[current_active_menu_member].state = unactive;
		current_active_menu_member = 0;
		Members[current_active_menu_member].state = active;
		current_page = 1;
		Mount_USB();
    Scan_USB(path, Members, current_page);
		amount_of_files = Scan_USB_for_amount_of_files(path);
    Unmount_USB();
		GC9A01_ClearScreen(WHITE);
    ShowMenu(Members, current_page);
    }
}

void view_menu_RIGHT_button_handler(){
	uint8_t active_member = get_active_menu_member(Members);	
	
	//case .txt file
	if(chek_menu_member_for_the_file_type(Members[active_member],"txt") == 1){
		enable_menu_member_animation = 0;
		GC9A01_SetFont(default_font);
		GC9A01_SetFont(&Font16);
		extern GC9A01_DrawPropTypeDef lcdprop;
		char buffer[lcdprop.pFont->Width * lcdprop.pFont->Height];
		
		snprintf(path_txt, sizeof(path_txt), "%.*s/%s", (int)(strlen(path)), path, Members[active_member].text);

		Mount_USB();
		Read_File(path_txt,txt_file_page,buffer, (lcdprop.pFont->Width * lcdprop.pFont->Height)-11 );
		Unmount_USB();
		GC9A01_Text(buffer,1);
		current_mode = view_txt;
		
	//directory case
	}else if(chek_menu_member_for_the_file_type(Members[active_member],"DIR") == 1){
		current_page = 1;
		Members[current_active_menu_member].state = unactive;
		current_active_menu_member = 0;
		Members[current_active_menu_member].state = active;
		
		uint8_t lenght_of_current_path = strlen(path);
		uint8_t len = strlen(Members[active_member].text);
		char temp[len - 3];
		strncpy(temp, Members[active_member].text, len - 4);
		temp[len - 3] = '\0';
		sprintf(path+lenght_of_current_path, "/%.*s",(int)(strlen(Members[active_member].text) - 4),	Members[active_member].text);
		
		Mount_USB();
		Scan_USB(path, Members,current_page);
		amount_of_files = Scan_USB_for_amount_of_files(path);
		Unmount_USB();
		GC9A01_ClearScreen(WHITE);
		ShowMenu(Members,current_page);
		
	}else if(chek_menu_member_for_the_file_type(Members[active_member],"bmp") == 1){
		enable_menu_member_animation = 0;
		snprintf(path_txt, sizeof(path_txt), "%.*s/%s", (int)(strlen(path)), path, Members[active_member].text);
		current_mode = view_image;
		GC9A01_ClearScreen(WHITE);
		Mount_USB();
		Read_File_and_print_BMP(path_txt,&horizontal_offset,&vertical_offset,interpolation_coef );
		Unmount_USB();
	
	}
	/*
		other cases
	*/
}

void view_txt_DOWN_button_handler(){
	txt_file_page ++;
	view_menu_RIGHT_button_handler();
}

void view_txt_UP_button_handler(){
	txt_file_page --;
	view_menu_RIGHT_button_handler();
}

void view_txt_LEFT_button_handler(){
	GC9A01_ClearScreen(WHITE);
	current_mode = view_file_menu;
	txt_file_page = 1;
	enable_menu_member_animation = 1;
	shift = 0;
	ShowMenu(Members, current_page);
}

void view_txt_RIGHT_button_handler(){
	//nothing 
}

void view_image_DOWN_button_handler(){
	if(vertical_offset >= 64){
		vertical_offset  = vertical_offset - 64;
	} else {
		vertical_offset = 0;
	}
	refresh_BMP_image();
	
	}

	
void view_image_UP_button_handler(){
	vertical_offset  = vertical_offset + 64;
	refresh_BMP_image();

}
	
void view_image_LEFT_button_handler(){
	if(horizontal_offset >= 64){
		horizontal_offset  = horizontal_offset - 64;
	} else {
		horizontal_offset = 0;
	}
	refresh_BMP_image();
}
	
void view_image_RIGHT_button_handler(){
	horizontal_offset = horizontal_offset + 64;
	refresh_BMP_image();

}

void check_for_USB_storage_connection(){
	MX_USB_HOST_Process();
	if(Appli_state == APPLICATION_READY){
		USB_Storage_state = connected;
		if(	USB_Storage_last_state == disconected){
			enable_menu_member_animation = 1;
			enable_button_handling = 1;
			current_mode = view_file_menu;
			GC9A01_ClearScreen(WHITE);
			current_page = 1;
			Members[current_active_menu_member].state = unactive;
			current_active_menu_member = 0;
			Members[current_active_menu_member].state = active;
			Mount_USB();
			clear_and_set_root(path, 512);
			Scan_USB(path, Members,current_page);
			Unmount_USB();
			ShowMenu(Members,current_page);
		}
		USB_Storage_last_state = connected;
	} else {
		USB_Storage_state = disconected;
		if(	USB_Storage_last_state == connected){
			enable_menu_member_animation = 0;
			enable_button_handling = 0;
			GC9A01_ClearScreen(WHITE);
			GC9A01_SetTextColor(default_text_color);
			GC9A01_SetBackColor(default_background_color);
			GC9A01_SetFont(default_font);
			GC9A01_Rainbow_String(40,100, "Storage");
			GC9A01_Rainbow_String(40,120, "Disconected");
		}
		USB_Storage_last_state = disconected;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
		LL_GPIO_SetOutputPin	(	LED_GPIO_Port,LED_Pin	);	
		GC9A01_ClearScreen(GREEN);			
		LL_GPIO_ResetOutputPin	(	LED_GPIO_Port,LED_Pin	);	
		GC9A01_ClearScreen(BLUE);	
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
