/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 锟斤拷锟斤拷锟斤拷
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * 锟斤拷锟斤拷锟绞癸拷玫锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷诒锟斤拷锟斤拷锟斤拷锟斤拷母锟侥柯硷拷械锟絃ICENSE锟侥硷拷锟斤拷锟揭碉拷锟斤拷锟斤拷锟斤拷
  * 锟斤拷锟矫伙拷锟斤拷姹撅拷锟斤拷锟结供LICENSE锟侥硷拷锟斤拷锟斤拷"原锟斤拷"锟结供锟斤拷
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "led.h"  // LED控制头文件
//#include "oled.h"  // OLED显示头文件（暂时禁用）
#include "sensor.h"  // 传感器相关头文件
#include "dac7311.h"  // DAC控制头文件
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_UPDATE_INTERVAL   500       // 传感器数据更新间隔(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // 显示更新间隔(ms)
#define WARMUP_TIME              10000     // 预热时间(ms)，修改为10秒
#define DAC_VREF                 3.3f      // DAC参考电压（3.3V）
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 传感器相关变量
static SensorData_t sensorData = {0};       // 传感器数据结构体，初始化为0
static uint32_t lastSensorUpdateTime = 0;   // 上次传感器数据更新时间
//static uint32_t lastDisplayUpdateTime = 0;   // 上次显示更新时间（暂时禁用）

// 系统状态相关变量
static uint32_t warmupStartTime = 0;        // 预热开始时间
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
// 函数声明
//static void UpdateDisplay(SensorData_t *data);  // 显示更新函数声明（暂时禁用）
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
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();  // 初始化所有GPIO
  MX_I2C1_Init();  // 初始化I2C1
  MX_USART1_UART_Init();  // 初始化USART1，用于调试输出
  MX_USART3_UART_Init();  // 初始化USART3，用于传感器通信

  /* USER CODE BEGIN 2 */
  // 初始化LED
  LED_Init();  // 初始化LED控制模块
  LED_UpdateState(SYSTEM_INIT);  // 设置LED为初始化状态，LED1闪烁，LED2熄灭
  
  // 记录预热开始时间
  warmupStartTime = HAL_GetTick();  // 记录预热开始时间
  
  /* 注释掉OLED初始化相关代码 */
  
  // 初始化DAC7311
  printf("初始化DAC7311...\r\n");
  if (DAC7311_Init() != 0) {
    printf("DAC7311初始化失败!\r\n");
    LED_UpdateState(SYSTEM_ERROR);  // 错误状态：LED1闪烁，LED2熄灭
    while(1) {
      LED_UpdateState(SYSTEM_ERROR);
      HAL_Delay(10);
    }
  }
  printf("DAC7311初始化完成\r\n");
  
  // 初始化时设置DAC输出为0
  DAC7311_SetValue(0);
  
  // 初始化传感器
  printf("初始化传感器...\r\n");
  if (Sensor_Init() != 0) {
    printf("传感器初始化失败! 错误代码: %d\r\n", Sensor_GetLastError());
    LED_UpdateState(SYSTEM_ERROR);  // 错误状态：LED1闪烁，LED2熄灭
    while(1) {
      LED_UpdateState(SYSTEM_ERROR);
      HAL_Delay(10);
    }
  }
  printf("传感器初始化完成\r\n");
  
  // 在初始化阶段保持LED状态更新
  uint32_t currentTime = HAL_GetTick();
  printf("开始初始化阶段检查...\r\n");
  while(currentTime - warmupStartTime < 1000)
  {
    printf("检查传感器通信...\r\n");
    if (Sensor_CheckConnection() != 0)
    {
      printf("传感器通信失败!\r\n");
      LED_UpdateState(SYSTEM_ERROR);  // 错误状态：LED1闪烁，LED2熄灭
      while(1) {
        LED_UpdateState(SYSTEM_ERROR);
        HAL_Delay(10);
      }
    }
    printf("传感器通信正常\r\n");
    LED_UpdateState(SYSTEM_INIT);  // 初始化状态：LED1闪烁，LED2熄灭
    currentTime = HAL_GetTick();
    HAL_Delay(100);
  }
  printf("初始化阶段检查完成\r\n");
  
  // 进入预热阶段
  LED_UpdateState(SYSTEM_WARMUP);  // 预热状态：LED1常亮，LED2闪烁
  
  // 等待预热完成
  while(HAL_GetTick() - warmupStartTime < WARMUP_TIME) {
    LED_UpdateState(SYSTEM_WARMUP);  // 保持更新LED状态
    HAL_Delay(100);
  }
  
  // 预热完成，进入正常工作状态
  LED_UpdateState(SYSTEM_WORKING);  // 工作状态：LED1常亮，LED2由DAC更新控制
  printf("系统预热完成，开始正常工作\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    currentTime = HAL_GetTick();
    
    // 更新传感器数据
    if(currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL)
    {
      // 更新所有传感器数据
      if(Sensor_UpdateAllData(&sensorData) != 0)
      {
        printf("传感器数据更新失败!\r\n");
        LED_UpdateState(SYSTEM_ERROR);  // 错误状态：LED1闪烁，LED2熄灭
      }
      else
      {
        LED_UpdateState(SYSTEM_WORKING);  // 工作状态：LED1常亮，LED2由DAC更新控制
        printf("[DAC] 当前输出值: %d\r\n", sensorData.outputValue);
      }
      
      lastSensorUpdateTime = currentTime;
    }
    
    HAL_Delay(10);  // 短暂延时，避免CPU占用过高
  }
  /* USER CODE END 3 */
}

/* 注释掉UpdateDisplay函数实现
static void UpdateDisplay(SensorData_t *data)
{
    if (data == NULL) {
        return;
    }
    
    OLED_Clear();
    char buf[32];
    
    sprintf(buf, "PPM:%d", data->concentration);
    OLED_ShowString(0, 0, buf);
    
    sprintf(buf, "DAC:%d", data->outputValue);
    OLED_ShowString(0, 2, buf);
    
    sprintf(buf, "V:%.2fV", data->outputVoltage);
    OLED_ShowString(0, 4, buf);
    
    sprintf(buf, "I:%.2fmA", data->outputCurrent);
    OLED_ShowString(0, 6, buf);
    
    OLED_Refresh();
}
*/

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
  /* 锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷约锟斤拷锟绞碉拷锟斤拷锟斤拷锟斤拷锟紿AL锟斤拷锟襟返伙拷状态 */
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
  /* 锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷约锟斤拷锟绞碉拷锟斤拷锟斤拷锟斤拷锟斤拷募锟斤拷锟斤拷锟斤拷泻牛锟??
     锟斤拷锟界：printf("锟斤拷锟斤拷锟斤拷锟斤拷值锟斤拷锟侥硷拷%s锟斤拷锟斤拷%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
