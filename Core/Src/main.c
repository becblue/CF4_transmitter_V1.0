/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * ï¿½ï¿½ï¿½ï¿½ï¿½Ê¹ï¿½Ãµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä¸ï¿½Ä¿Â¼ï¿½Ðµï¿½LICENSEï¿½Ä¼ï¿½ï¿½ï¿½ï¿½Òµï¿½ï¿½ï¿½ï¿½ï¿½
  * ï¿½ï¿½ï¿½Ã»ï¿½ï¿½ï¿½æ±¾ï¿½ï¿½ï¿½ï¿½á¹©LICENSEï¿½Ä¼ï¿½ï¿½ï¿½ï¿½ï¿½"Ô­ï¿½ï¿½"ï¿½á¹©ï¿½ï¿½
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_UPDATE_INTERVAL   500       // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý¸ï¿½ï¿½Â¼ï¿½ï¿?(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // ï¿½ï¿½Ê¾ï¿½ï¿½ï¿½Â¼ï¿½ï¿?(ms)
#define WARMUP_TIME              3000      // Ô¤ï¿½ï¿½Ê±ï¿½ï¿½(ms)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* ï¿½ï¿½Ð©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±Î´Ê¹ï¿½Ã£ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÒªÈ¡ï¿½ï¿½×¢ï¿½ï¿½
static SensorData_t sensorData;            // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý½á¹¹ï¿½ï¿½
static uint32_t lastSensorUpdateTime = 0;  // ï¿½Ï´Î´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý¸ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
static uint32_t lastDisplayUpdateTime = 0;  // ï¿½Ï´ï¿½ï¿½ï¿½Ê¾ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
static uint8_t systemError = 0;            // ÏµÍ³ï¿½ï¿½ï¿½ï¿½ï¿½Ö?
static uint8_t isWarmingUp = 1;            // Ô¤ï¿½È±ï¿½Ö¾
static uint32_t warmupStartTime = 0;       // Ô¤ï¿½È¿ï¿½Ê¼Ê±ï¿½ï¿½
static uint8_t systemStatus = 0;           // ÏµÍ³×´Ì¬ï¿½ï¿½0-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, 1-Ô¤ï¿½ï¿½ï¿½ï¿½, 2-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, 3-ï¿½ï¿½ï¿½ï¿½
*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // ³õÊ¼»¯OLEDÏÔÊ¾
  printf("³õÊ¼»¯OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {
    printf("OLED³õÊ¼»¯Ê§°Ü!\r\n");
    Error_Handler();
  }
  printf("OLED³õÊ¼»¯³É¹¦\r\n");
  
  OLED_Clear();
  OLED_ShowString(0, 0, "System Starting...");
  OLED_Refresh();
  printf("OLEDÏÔÊ¾Æô¶¯ÐÅÏ¢\r\n");
  
  // ³õÊ¼»¯DAC7311
  if (DAC7311_Init() != 0) {
    // DAC³õÊ¼»¯Ê§°Ü
    OLED_Clear();
    OLED_ShowString(0, 0, "DAC Init Failed!");
    OLED_Refresh();
    Error_Handler();
  }
  
  // ³õÊ¼»¯´«¸ÐÆ÷
  OLED_Clear();
  OLED_ShowString(0, 0, "Warming up...");
  OLED_Refresh();
  if (Sensor_Init() != 0) {
    // ³õÊ¼»¯Ê§°Ü
    OLED_Clear();
    OLED_ShowString(0, 0, "Sensor Error!");
    OLED_ShowString(0, 1, "Code: ");
    char errStr[8];
    sprintf(errStr, "%d", Sensor_GetLastError());
    OLED_ShowString(42, 1, errStr);
    OLED_Refresh();
    HAL_Delay(2000); // ÏÔÊ¾2Ãë´íÎóÐÅÏ¢
  }
  
  // ÏµÍ³Ô¤ÈÈ
  uint8_t warmupSeconds = 3; // Ô¤Ô¤ÈÈÊ±¼ä3Ãë
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "System Warming Up");
    char timeStr[16];
    sprintf(timeStr, "Wait %ds", warmupSeconds - i);  // ÏÔÊ¾Ê£ÓàÊ±¼ä
    OLED_ShowString(0, 2, timeStr);
    OLED_Refresh();
    HAL_Delay(1000); // µÈ´ý1Ãë
  }
  
  // Ô¤ÈÈÍê³É
  OLED_Clear();
  OLED_ShowString(0, 0, "System Ready");
  OLED_Refresh();
  HAL_Delay(1000); // ÏÔÊ¾1Ãë
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 1. ´«¸ÐÆ÷Êý¾Ý²É¼¯´¦Àí
    SensorData_t sensorData;  // ¶¨Òå´«¸ÐÆ÷Êý¾Ý½á¹¹Ìå
    if (Sensor_UpdateAllData(&sensorData) == 0)  // ³¢ÊÔ¸üÐÂËùÓÐ´«¸ÐÆ÷Êý¾Ý
    {
      // 2. Êý¾Ý´¦ÀíºÍÏÔÊ¾²¿·Ö
      // ÔÚOLEDÉÏÏÔÊ¾Êý¾Ý
      OLED_Clear();  // ÇåÆÁ
      
      // ÏÔÊ¾Å¨¶ÈÖµ
      char buf[32];
      sprintf(buf, "Å¨¶È: %d", sensorData.concentration);
      OLED_ShowString(0, 0, buf);
      
      // ÏÔÊ¾Êä³öµçÑ¹
      sprintf(buf, "µçÑ¹: %.2fV", sensorData.outputVoltage);
      OLED_ShowString(0, 2, buf);
      
      // ÏÔÊ¾Êä³öµçÁ÷
      sprintf(buf, "µçÁ÷: %.2fmA", sensorData.outputCurrent);
      OLED_ShowString(0, 4, buf);
      
      // 3. µ÷ÊÔÐÅÏ¢Êä³ö
      printf("\r\n[%lu ms] ´«¸ÐÆ÷Êý¾Ý¸üÐÂ£º\r\n", HAL_GetTick());
      printf("Å¨¶È: %d\r\n", sensorData.concentration);
      printf("Êä³öÖµ: %d\r\n", sensorData.outputValue);
      printf("µçÑ¹: %.2fV\r\n", sensorData.outputVoltage);
      printf("µçÁ÷: %.2fmA\r\n", sensorData.outputCurrent);
    }
    else
    {
      // Êý¾Ý¸üÐÂÊ§°Ü£¬ÏÔÊ¾´íÎóÐÅÏ¢
      printf("\r\n[%lu ms] ´íÎó£º%s\r\n", HAL_GetTick(), Sensor_GetErrorString(Sensor_GetLastError()));
      
      // ÔÚOLEDÉÏÏÔÊ¾´íÎóÐÅÏ¢
      OLED_Clear();
      OLED_ShowString(0, 0, "Êý¾Ý¸üÐÂÊ§°Ü");
      OLED_ShowString(0, 2, Sensor_GetErrorString(Sensor_GetLastError()));
    }
    
    HAL_Delay(1000);  // Ã¿Ãë¸üÐÂÒ»´ÎÊý¾Ý
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô¼ï¿½ï¿½ï¿½Êµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½HALï¿½ï¿½ï¿½ó·µ»ï¿½×´Ì¬ */
  __disable_irq();
  while (1)
  {
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
  /* ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô¼ï¿½ï¿½ï¿½Êµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÐºÅ£ï¿?
     ï¿½ï¿½ï¿½ç£ºprintf("ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Öµï¿½ï¿½ï¿½Ä¼ï¿½%sï¿½ï¿½ï¿½ï¿½%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
