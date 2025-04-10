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
static SensorData_t sensorData;            // 传感器数据结构体
static uint32_t lastSensorUpdateTime = 0;  // 上次传感器数据更新时间
static uint32_t lastDisplayUpdateTime = 0;  // 上次显示更新时间
static uint8_t systemError = 0;            // 系统错误标志
static uint8_t isWarmingUp = 1;            // 预热标志
static uint32_t warmupStartTime = 0;       // 预热开始时间
static uint8_t systemStatus = 0;           // 系统状态：0-启动中, 1-预热中, 2-正常运行, 3-错误
/* USER CODE END PV */

/* 私有函数原型 -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void System_Init(void);
static void Process_Sensor_Data(void);
static void Update_Display(void);
static void Send_Debug_Info(void);
static void Process_Input(void);
/* USER CODE END PFP */

/* 私有用户代码 ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  系统初始化
  * @retval 无
  */
static void System_Init(void)
{
  uint8_t initStatus = 0;
  
  // 初始化系统状态
  systemStatus = 0;  // 启动中
  
  // 点亮电源指示灯
  HAL_GPIO_WritePin(PowerLed_GPIO_Port, PowerLed_Pin, GPIO_PIN_SET);
  
  // 初始化DAC7311
  if (DAC7311_Init() != 0) {
    initStatus |= 0x01;  // DAC初始化失败
  }
  
  // 初始化OLED显示
  if (OLED_Init() != 0) {
    initStatus |= 0x02;  // OLED初始化失败
  }
  
  // 显示启动信息
  OLED_Clear();
  OLED_ShowStatus(systemStatus, initStatus);
  OLED_ShowString(0, 2, "CF4 Sensor System");
  OLED_ShowString(0, 3, "System Starting...");
  OLED_Refresh();
  
  // 短暂延时，显示启动信息
  HAL_Delay(1000);
  
  // 切换到预热阶段
  systemStatus = 1;  // 预热中
  
  // 预热阶段初始化
  warmupStartTime = HAL_GetTick();
  isWarmingUp = 1;
  
  // 显示预热信息
  OLED_ShowStatus(systemStatus, 0);
  OLED_ShowString(0, 2, "Warming up...");
  OLED_ShowString(0, 3, "Please wait 60s");
  OLED_Refresh();
  
  // 先不初始化传感器，等待预热时间
  
  systemError = initStatus;
}

/**
  * @brief  传感器数据处理
  * @retval 无
  */
static void Process_Sensor_Data(void)
{
  uint32_t currentTime = HAL_GetTick();
  
  // 检查是否需要更新传感器数据
  if (currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdateTime = currentTime;
    
    // 如果在预热阶段，检查是否已完成预热
    if (isWarmingUp) {
      // 计算预热进度
      uint32_t elapsed = currentTime - warmupStartTime;
      uint8_t percent = (elapsed * 100) / WARMUP_TIME;
      if (percent > 100) percent = 100;
      
      // 显示预热进度条
      OLED_ShowProgress(0, 4, 100, percent);
      
      // 检查是否预热完成
      if (elapsed >= WARMUP_TIME) {
        isWarmingUp = 0;  // 预热完成
        
        // 预热结束后初始化传感器
        if (Sensor_Init() != 0) {
          systemError |= 0x04;  // 传感器初始化失败
          systemStatus = 3;     // 错误状态
          OLED_ShowStatus(systemStatus, systemError);
        } else {
          // 设置系统正常运行状态
          systemStatus = 2;     // 正常运行
          // 点亮运行指示灯
          HAL_GPIO_WritePin(RunLed_GPIO_Port, RunLed_Pin, GPIO_PIN_SET);
          OLED_ShowStatus(systemStatus, 0);
        }
        
        // 延时1秒显示状态信息
        HAL_Delay(1000);
      }
    } else {
      // 预热完成后，正常更新传感器数据
      if (Sensor_UpdateAllData(&sensorData) != 0) {
        // 数据更新失败，获取错误信息
        uint8_t errorCode = Sensor_GetLastError();
        // 如果连续出现错误，设置系统为错误状态
        systemError = 0x08 | errorCode;  // 运行时错误
        systemStatus = 3;  // 错误状态
        OLED_ShowStatus(systemStatus, systemError);
      } else if (systemStatus == 3) {
        // 恢复正常状态
        systemStatus = 2;
        OLED_ShowStatus(systemStatus, 0);
      }
    }
  }
}

/**
  * @brief  更新显示
  * @retval 无
  */
static void Update_Display(void)
{
  uint32_t currentTime = HAL_GetTick();
  
  // 检查是否需要更新显示
  if (currentTime - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdateTime = currentTime;
    
    if (!isWarmingUp && systemStatus == 2) {
      // 正常运行状态下显示传感器数据
      OLED_ShowValue(
        sensorData.zeroPoint,
        sensorData.rangeValue,
        sensorData.concentration,
        sensorData.outputValue,
        sensorData.outputVoltage,
        sensorData.outputCurrent
      );
    }
  }
}

/**
  * @brief  发送调试信息到串口
  * @retval 无
  */
static void Send_Debug_Info(void)
{
  static uint32_t lastDebugTime = 0;
  uint32_t currentTime = HAL_GetTick();
  
  // 每2秒发送一次调试信息
  if (currentTime - lastDebugTime >= 2000) {
    lastDebugTime = currentTime;
    
    // 构造调试信息
    char buffer[100];
    int len;
    
    if (isWarmingUp) {
      // 预热阶段信息
      uint32_t elapsed = currentTime - warmupStartTime;
      uint8_t percent = (elapsed * 100) / WARMUP_TIME;
      len = sprintf(buffer, "System warming up: %d%% complete\r\n", percent);
    } else {
      // 运行阶段信息
      len = sprintf(buffer, "CF4 Data: Zero=%d, Range=%d, Conc=%d, Out=%d, %.2fV, %.2fmA, Status=%d\r\n",
                   sensorData.zeroPoint,
                   sensorData.rangeValue,
                   sensorData.concentration,
                   sensorData.outputValue,
                   sensorData.outputVoltage,
                   sensorData.outputCurrent,
                   systemStatus);
    }
    
    // 通过USART2发送到上位机
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, 100);
  }
}

/**
  * @brief  处理按键输入
  * @retval 无
  */
static void Process_Input(void)
{
  static uint32_t lastKey1Time = 0;
  static uint32_t lastKey2Time = 0;
  static uint8_t key1LastState = 1;
  static uint8_t key2LastState = 1;
  uint32_t currentTime = HAL_GetTick();
  uint8_t key1State, key2State;
  
  // 读取按键状态
  key1State = HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin);
  key2State = HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin);
  
  // 按键1处理 (防抖动)
  if (key1State != key1LastState && currentTime - lastKey1Time > 50) {
    lastKey1Time = currentTime;
    key1LastState = key1State;
    
    // 按键释放时执行操作(上升沿触发)
    if (key1State == 1) {
      // 按键1操作 - 显示系统信息
      OLED_Clear();
      OLED_ShowString(0, 0, "System Info");
      OLED_ShowString(0, 1, "----------------");
      
      char buffer[21];
      sprintf(buffer, "Status: %d", systemStatus);
      OLED_ShowString(0, 2, buffer);
      
      sprintf(buffer, "Error: 0x%02X", systemError);
      OLED_ShowString(0, 3, buffer);
      
      sprintf(buffer, "Runtime: %ds", currentTime / 1000);
      OLED_ShowString(0, 4, buffer);
      
      // 显示固件版本
      OLED_ShowString(0, 6, "Firmware: v1.0");
      OLED_Refresh();
      
      // 等待3秒
      HAL_Delay(3000);
      
      // 恢复正常显示
      if (!isWarmingUp) {
        OLED_ShowValue(
          sensorData.zeroPoint,
          sensorData.rangeValue,
          sensorData.concentration,
          sensorData.outputValue,
          sensorData.outputVoltage,
          sensorData.outputCurrent
        );
      }
    }
  }
  
  // 按键2处理 (防抖动)
  if (key2State != key2LastState && currentTime - lastKey2Time > 50) {
    lastKey2Time = currentTime;
    key2LastState = key2State;
    
    // 按键释放时执行操作(上升沿触发)
    if (key2State == 1) {
      // 按键2操作 - 显示另一组信息
      // 此处可根据实际需求添加功能
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  应用程序入口点
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU配置--------------------------------------------------------*/

  /* 复位所有外设，初始化Flash接口和Systick */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化所有已配置的外设 */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  
  /* USER CODE BEGIN 2 */
  // 初始化OLED显示
  OLED_Init(); // 初始化OLED显示屏
  OLED_Clear(); // 清空显示
  
  // 显示启动界面
  OLED_ShowStatus(0, 0); // 显示启动状态
  HAL_Delay(1000); // 延时1秒
  
  // 初始化DAC7311
  DAC7311_Init(); // 初始化DAC7311
  
  // 初始化传感器通信
  OLED_ShowStatus(1, 0); // 显示预热状态
  if (Sensor_Init() != 0) {
    // 传感器初始化失败
    OLED_ShowStatus(3, Sensor_GetLastError()); // 显示错误状态
    HAL_Delay(2000); // 显示2秒错误信息
  }
  
  // 系统预热
  uint8_t warmupSeconds = 60; // 预热时间60秒
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_ShowProgress(0, 7, warmupSeconds, i+1); // 显示预热进度
    HAL_Delay(1000); // 延时1秒
  }
  
  // 预热完成，进入正常工作模式
  OLED_ShowStatus(2, 0); // 显示运行状态
  HAL_Delay(1000); // 显示1秒
  /* USER CODE END 2 */

  /* 无限循环 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 更新传感器数据
    SensorData_t sensorData; // 定义传感器数据结构体
    
    // 获取最新传感器数据
    if (Sensor_UpdateAllData(&sensorData) == 0) {
      // 更新成功，显示数据
      OLED_ShowValue(
        sensorData.zeroPoint,     // 零点值
        sensorData.rangeValue,    // 量程值
        sensorData.concentration, // 浓度值
        sensorData.outputValue,   // 输出值
        sensorData.outputVoltage, // 输出电压
        sensorData.outputCurrent  // 输出电流
      );
    } else {
      // 更新失败，显示错误信息
      OLED_ShowStatus(3, Sensor_GetLastError()); // 显示错误状态
    }
    
    // 点亮运行指示灯
    HAL_GPIO_TogglePin(RunLed_GPIO_Port, RunLed_Pin); // 翻转运行指示灯状态
    
    // 延时
    HAL_Delay(500); // 500毫秒刷新一次
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
