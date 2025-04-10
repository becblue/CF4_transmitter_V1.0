/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * 本软件使用的许可条款可在本软件组件的根目录中的LICENSE文件中找到。
  * 如果没有随本软件提供LICENSE文件，则按"原样"提供。
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* 私有包含文件 ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* 私有类型定义 -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* 私有宏定义 ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_UPDATE_INTERVAL   500       // 传感器数据更新间隔(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // 显示更新间隔(ms)
#define WARMUP_TIME              60000     // 预热时间(ms)
/* USER CODE END PD */

/* 私有宏 -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* 私有变量 ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* 这些变量暂时未使用，根据需要取消注释
static SensorData_t sensorData;            // 传感器数据结构体
static uint32_t lastSensorUpdateTime = 0;  // 上次传感器数据更新时间
static uint32_t lastDisplayUpdateTime = 0;  // 上次显示更新时间
static uint8_t systemError = 0;            // 系统错误标志
static uint8_t isWarmingUp = 1;            // 预热标志
static uint32_t warmupStartTime = 0;       // 预热开始时间
static uint8_t systemStatus = 0;           // 系统状态：0-启动中, 1-预热中, 2-正常运行, 3-错误
*/
/* USER CODE END PV */

/* 私有函数原型 -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* 私有用户代码 ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  主函数
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
  MX_USART1_UART_Init();  // 首先初始化调试串口
  printf("\r\n\r\n系统启动...\r\n");  // 立即输出启动信息
  printf("初始化其他外设...\r\n");
  
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  
  printf("外设初始化完成\r\n");
  printf("当前系统时钟频率: %d Hz\r\n", HAL_RCC_GetSysClockFreq());
  
  /* USER CODE BEGIN 2 */
  // 初始化OLED显示
  printf("初始化OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {  // 初始化OLED显示屏，传入I2C1句柄
    printf("OLED初始化失败!\r\n");
    Error_Handler();
  }
  printf("OLED初始化成功\r\n");
  
  OLED_Clear(); // 清空显示
  OLED_ShowString(0, 0, "System Starting...");
  OLED_Refresh(); // 刷新显示
  printf("OLED显示测试信息\r\n");
  
  // 初始化DAC7311
  if (DAC7311_Init(&hspi1, DAC1SYNC_GPIO_Port, DAC1SYNC_Pin) != 0) {
    // DAC初始化失败
    OLED_Clear();
    OLED_ShowString(0, 0, "DAC Init Failed!");
    OLED_Refresh();
    Error_Handler();
  }
  
  // 初始化传感器通信
  OLED_Clear();
  OLED_ShowString(0, 0, "Warming up...");
  OLED_Refresh();
  if (Sensor_Init() != 0) {
    // 传感器初始化失败
    OLED_Clear();
    OLED_ShowString(0, 0, "Sensor Error!");
    OLED_ShowString(0, 1, "Code: ");
    char errStr[8];
    sprintf(errStr, "%d", Sensor_GetLastError());
    OLED_ShowString(42, 1, errStr);
    OLED_Refresh();
    HAL_Delay(2000); // 显示2秒错误信息
  }
  
  // 系统预热
  uint8_t warmupSeconds = 60; // 预热时间60秒
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "System Warming Up");
    OLED_ShowProgress(0, 2, warmupSeconds, i+1);
    OLED_Refresh();
    HAL_Delay(1000); // 延时1秒
  }
  
  // 预热完成，进入正常工作模式
  OLED_Clear();
  OLED_ShowString(0, 0, "System Ready");
  OLED_Refresh();
  HAL_Delay(1000); // 显示1秒
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(1000);  // 每秒发送一次心跳信息
    printf("系统运行中...\r\n");
  }
  /* USER CODE END 3 */
}

/**
  * @brief 系统时钟配置
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 根据RCC_OscInitTypeDef结构中指定的参数初始化RCC振荡器
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

  /** 初始化CPU、AHB和APB总线时钟
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
  * @brief  错误处理函数
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 用户可以添加自己的实现来报告HAL错误返回状态 */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  报告源文件名和源行号，其中发生assert_param错误
  * @param  file: 指向源文件名的指针
  * @param  line: assert_param错误行源码号
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 用户可以添加自己的实现来报告文件名和行号，
     例如：printf("参数错误值：文件%s，行%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
