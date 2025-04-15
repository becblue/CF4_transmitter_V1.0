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
#include "oled.h"  // OLED显示头文件
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
#define WARMUP_TIME              3000      // 预热时间(ms)
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
static uint32_t lastDisplayUpdateTime = 0;   // 上次显示更新时间

// 系统状态相关变量
static uint32_t warmupStartTime = 0;        // 预热开始时间
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
// 函数声明
static void UpdateDisplay(SensorData_t *data);  // 显示更新函数声明
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
  MX_GPIO_Init();  // 初始化所有GPIO
  MX_I2C1_Init();  // 初始化I2C1，用于OLED显示
  MX_USART1_UART_Init();  // 初始化USART1，用于调试输出
  MX_USART3_UART_Init();  // 初始化USART3，用于传感器通信
  /* USER CODE BEGIN 2 */
  // 初始化LED
  LED_Init();  // 初始化LED控制模块
  LED_UpdateState(SYSTEM_INIT);  // 设置LED为初始化状态，LED1闪烁，LED2熄灭
  
  // 记录预热开始时间
  warmupStartTime = HAL_GetTick();  // 记录预热开始时间
  
  // 初始化OLED显示
  printf("初始化OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {
    printf("OLED初始化失败!\r\n");
    LED_UpdateState(SYSTEM_ERROR);  // 设置LED为错误状态
    while(1) {  // 停留在错误状态
      LED_UpdateState(SYSTEM_ERROR);  // 保持LED错误状态显示
      HAL_Delay(10);  // 小延时，避免CPU占用过高
    }
  }
  printf("OLED初始化完成\r\n");
  
  // 初始化DAC7311
  printf("初始化DAC7311...\r\n");  // 打印初始化开始信息
  if (DAC7311_Init() != 0) {  // 初始化DAC7311
    printf("DAC7311初始化失败!\r\n");  // 打印错误信息
    LED_UpdateState(SYSTEM_ERROR);  // 设置LED为错误状态
    OLED_Clear();  // 清空OLED显示
    OLED_ShowString(0, 0, "DAC Init Failed!");  // 显示错误信息
    OLED_Refresh();  // 刷新OLED显示
    while(1) {  // 停留在错误状态
      LED_UpdateState(SYSTEM_ERROR);  // 保持LED错误状态显示
      HAL_Delay(10);  // 小延时，避免CPU占用过高
    }
  }
  printf("DAC7311初始化完成\r\n");  // 打印初始化完成信息
  
  // 初始化时设置DAC输出为0
  DAC7311_SetValue(0);  // 设置DAC输出为0
  
  // 初始化传感器
  printf("初始化传感器...\r\n");
  if (Sensor_Init() != 0) {
    printf("传感器初始化失败! 错误代码: %d\r\n", Sensor_GetLastError());
    LED_UpdateState(SYSTEM_ERROR);  // 设置LED为错误状态
    OLED_Clear();
    OLED_ShowString(0, 0, "Sensor Error!");
    OLED_ShowString(0, 1, "Code: ");
    char errStr[8];
    sprintf(errStr, "%d", Sensor_GetLastError());
    OLED_ShowString(42, 1, errStr);
    OLED_Refresh();
    while(1) {  // 停留在错误状态
      LED_UpdateState(SYSTEM_ERROR);  // 保持LED错误状态显示
      HAL_Delay(10);  // 小延时，避免CPU占用过高
    }
  }
  printf("传感器初始化完成\r\n");
  
  // 在初始化阶段保持LED状态更新
  uint32_t currentTime = HAL_GetTick();
  printf("开始初始化阶段检查...\r\n");
  while(currentTime - warmupStartTime < 1000)  // 初始化阶段持续1秒
  {
    // 只检查传感器通信状态，使用05指令
    printf("检查传感器通信...\r\n");
    if (Sensor_CheckConnection() != 0)  // 只检查通信状态
    {
      printf("传感器通信失败!\r\n");
      LED_UpdateState(SYSTEM_ERROR);  // 设置LED为错误状态
      OLED_Clear();
      OLED_ShowString(0, 0, "Sensor Error!");
      OLED_ShowString(0, 2, "Init Failed!");
      OLED_Refresh();
      while(1) {  // 停留在错误状态
        LED_UpdateState(SYSTEM_ERROR);  // 保持LED错误状态显示
        HAL_Delay(10);  // 小延时，避免CPU占用过高
      }
    }
    printf("传感器通信正常\r\n");
    LED_UpdateState(SYSTEM_INIT);  // LED1闪烁，LED2熄灭
    currentTime = HAL_GetTick();
    HAL_Delay(100);  // 添加延时，避免过于频繁的通信
  }
  printf("初始化阶段检查完成\r\n");
  
  // 进入预热阶段
  LED_UpdateState(SYSTEM_WARMUP);  // 切换到预热状态，LED1常亮，LED2闪烁
  
  // 系统预热
  uint8_t warmupSeconds = 3; // 预热时间3秒
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Warming Up...");
    char timeStr[16];
    sprintf(timeStr, "Wait %ds", warmupSeconds - i);  // 显示剩余时间
    OLED_ShowString(0, 2, timeStr);
    OLED_Refresh();
    
    // 在每秒开始时只检查传感器通信状态
    if (Sensor_CheckConnection() != 0)  // 只检查通信状态
    {
        LED_UpdateState(SYSTEM_ERROR);  // 设置LED为错误状态
        OLED_Clear();
        OLED_ShowString(0, 0, "Sensor Error!");
        OLED_ShowString(0, 2, "Warmup Failed!");
        OLED_Refresh();
        while(1) {  // 停留在错误状态
            LED_UpdateState(SYSTEM_ERROR);  // 保持LED错误状态显示
            HAL_Delay(10);  // 小延时，避免CPU占用过高
        }
    }
    
    // 在这一秒内只更新LED状态，不进行任何传感器查询
    uint32_t startTick = HAL_GetTick();
    while(HAL_GetTick() - startTick < 1000)  // 持续1秒
    {
        LED_UpdateState(SYSTEM_WARMUP);  // LED1常亮，LED2闪烁
        HAL_Delay(100);  // 每100ms更新一次LED状态
    }
    DAC7311_SetValue(0);  // 确保预热阶段DAC输出保持为0
  }
  
  // 预热结束，进入工作状态
  LED_UpdateState(SYSTEM_WORKING);  // 切换到工作状态，LED1常亮，LED2由DAC更新控制
  
  // 预热完成后的第一次数据获取
  HAL_Delay(100);  // 短暂延时，确保传感器完全就绪
  
  // 获取初始传感器数据
  if (Sensor_UpdateAllData(&sensorData) == 0)
  {
    OLED_Clear();
    char buf[32];
    
    // 显示浓度值
    sprintf(buf, "PPM:%d", sensorData.concentration);
    OLED_ShowString(0, 0, buf);
    
    // 显示量程值
    sprintf(buf, "Range:%d", sensorData.rangeValue);
    OLED_ShowString(0, 2, buf);
    
    // 显示零点值
    sprintf(buf, "Zero:%d", SENSOR_ZERO_POINT);
    OLED_ShowString(0, 3, buf);
    
    // 显示DAC输出值
    sprintf(buf, "DAC:%d/4095", sensorData.outputValue);
    OLED_ShowString(0, 6, buf);
    
    OLED_Refresh();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t currentTime = HAL_GetTick();
    static uint8_t errorCount = 0;  // 错误计数器
    static uint8_t isInErrorState = 0;  // 错误状态标志
    
    // 检查是否需要更新传感器数据
    if (currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL)
    {
      if (Sensor_UpdateAllData(&sensorData) == 0)  // 传感器通信成功
      {
        errorCount = 0;  // 清除错误计数
        if (isInErrorState)  // 如果之前处于错误状态
        {
          isInErrorState = 0;  // 清除错误状态
          LED_UpdateState(SYSTEM_WORKING);  // 恢复到工作状态
          
          // 恢复正常显示
          OLED_Clear();
          UpdateDisplay(&sensorData);
        }
        
        // 更新DAC输出为实际值
        DAC7311_SetVoltage((float)sensorData.outputValue * DAC_VREF / 4095.0f, DAC_VREF);
        if (!isInErrorState)  // 只在非错误状态下更新LED2
        {
          LED_HandleDAC7311Update();  // 更新LED2状态
        }
      }
      else  // 传感器通信失败
      {
        errorCount++;  // 增加错误计数
        if (errorCount >= 3)  // 连续3次通信失败
        {
          if (!isInErrorState)  // 如果还未处于错误状态
          {
            isInErrorState = 1;  // 设置错误状态标志
            LED_UpdateState(SYSTEM_ERROR);  // 切换到错误状态
            
            // 显示错误信息和DAC输出变化信息
            printf("[错误] 传感器通信失败，DAC输出将渐变至满量程\r\n");
            printf("[DAC] 当前输出值: %d，目标值: %d\r\n", sensorData.outputValue, DAC7311_FULL_SCALE);
            
            // 准备显示内容
            char dacStr[32];
            OLED_Clear();  // 清屏
            
            // 第一行显示错误信息
            OLED_ShowString(0, 0, "Sensor Error!");
            
            // 第二行显示检查连接提示
            OLED_ShowString(0, 2, "Check Connection");
            
            // 第三行显示当前DAC值
            sprintf(dacStr, "DAC:%d", sensorData.outputValue);
            OLED_ShowString(0, 4, dacStr);
            
            // 第四行显示目标值
            sprintf(dacStr, "Target:%d", DAC7311_FULL_SCALE);
            OLED_ShowString(0, 6, dacStr);
            
            // 一次性刷新显示
            OLED_Refresh();
            
            // 渐变DAC输出到满量程
            DAC7311_RampToValue(DAC7311_FULL_SCALE);
            
            // 更新显示最终DAC值
            OLED_Clear();  // 再次清屏
            OLED_ShowString(0, 0, "Sensor Error!");
            OLED_ShowString(0, 2, "Check Connection");
            sprintf(dacStr, "DAC Output:");
            OLED_ShowString(0, 4, dacStr);
            sprintf(dacStr, "Set to %d", DAC7311_FULL_SCALE);
            OLED_ShowString(0, 6, dacStr);
            OLED_Refresh();  // 刷新显示
            
            printf("[DAC] 输出已设置为满量程: %d\r\n", DAC7311_FULL_SCALE);
          }
          LED_UpdateState(SYSTEM_ERROR);  // 保持错误状态的LED显示
        }
      }
      lastSensorUpdateTime = currentTime;
    }
    
    // 只在非错误状态下更新显示和LED状态
    if (!isInErrorState)
    {
      // 更新显示
      if (currentTime - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL)
      {
        UpdateDisplay(&sensorData);
        lastDisplayUpdateTime = currentTime;
      }
      
      // 更新LED状态
      LED_UpdateState(SYSTEM_WORKING);
    }
    else
    {
      // 在错误状态下保持LED错误显示
      LED_UpdateState(SYSTEM_ERROR);
    }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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

/**
  * @brief  更新显示内容
  * @param  data: 传感器数据结构体指针
  * @retval None
  */
static void UpdateDisplay(SensorData_t *data)
{
    if (data == NULL) {  // 检查参数有效性
        return;
    }
    
    OLED_Clear();  // 清除显示
    char buf[32];   // 显示缓冲区
    
    // 显示浓度值
    sprintf(buf, "PPM:%d", data->concentration);
    OLED_ShowString(0, 0, buf);
    
    // 显示量程值
    sprintf(buf, "Range:%d", data->rangeValue);
    OLED_ShowString(0, 2, buf);
    
    // 显示零点值
    sprintf(buf, "Zero:%d", SENSOR_ZERO_POINT);
    OLED_ShowString(0, 3, buf);
    
    // 显示DAC输出值
    sprintf(buf, "DAC:%d/4095", data->outputValue);
    OLED_ShowString(0, 6, buf);
    
    OLED_Refresh();  // 刷新显示
}
