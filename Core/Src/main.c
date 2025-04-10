/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : ������
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * �����ʹ�õ����������ڱ��������ĸ�Ŀ¼�е�LICENSE�ļ����ҵ���
  * ���û���汾����ṩLICENSE�ļ�����"ԭ��"�ṩ��
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* �����ļ� ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* ˽�а����ļ� ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* ˽�����Ͷ��� -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* ˽�к궨�� ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_UPDATE_INTERVAL   500       // ���������ݸ��¼��(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // ��ʾ���¼��(ms)
#define WARMUP_TIME              60000     // Ԥ��ʱ��(ms)
/* USER CODE END PD */

/* ˽�к� -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* ˽�б��� ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static SensorData_t sensorData;            // ���������ݽṹ��
static uint32_t lastSensorUpdateTime = 0;  // �ϴδ��������ݸ���ʱ��
static uint32_t lastDisplayUpdateTime = 0;  // �ϴ���ʾ����ʱ��
static uint8_t systemError = 0;            // ϵͳ�����־
static uint8_t isWarmingUp = 1;            // Ԥ�ȱ�־
static uint32_t warmupStartTime = 0;       // Ԥ�ȿ�ʼʱ��
static uint8_t systemStatus = 0;           // ϵͳ״̬��0-������, 1-Ԥ����, 2-��������, 3-����
/* USER CODE END PV */

/* ˽�к���ԭ�� -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void System_Init(void);
static void Process_Sensor_Data(void);
static void Update_Display(void);
static void Send_Debug_Info(void);
static void Process_Input(void);
/* USER CODE END PFP */

/* ˽���û����� ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  ϵͳ��ʼ��
  * @retval ��
  */
static void System_Init(void)
{
  uint8_t initStatus = 0;
  
  // ��ʼ��ϵͳ״̬
  systemStatus = 0;  // ������
  
  // ������Դָʾ��
  HAL_GPIO_WritePin(PowerLed_GPIO_Port, PowerLed_Pin, GPIO_PIN_SET);
  
  // ��ʼ��DAC7311
  if (DAC7311_Init() != 0) {
    initStatus |= 0x01;  // DAC��ʼ��ʧ��
  }
  
  // ��ʼ��OLED��ʾ
  if (OLED_Init() != 0) {
    initStatus |= 0x02;  // OLED��ʼ��ʧ��
  }
  
  // ��ʾ������Ϣ
  OLED_Clear();
  OLED_ShowStatus(systemStatus, initStatus);
  OLED_ShowString(0, 2, "CF4 Sensor System");
  OLED_ShowString(0, 3, "System Starting...");
  OLED_Refresh();
  
  // ������ʱ����ʾ������Ϣ
  HAL_Delay(1000);
  
  // �л���Ԥ�Ƚ׶�
  systemStatus = 1;  // Ԥ����
  
  // Ԥ�Ƚ׶γ�ʼ��
  warmupStartTime = HAL_GetTick();
  isWarmingUp = 1;
  
  // ��ʾԤ����Ϣ
  OLED_ShowStatus(systemStatus, 0);
  OLED_ShowString(0, 2, "Warming up...");
  OLED_ShowString(0, 3, "Please wait 60s");
  OLED_Refresh();
  
  // �Ȳ���ʼ�����������ȴ�Ԥ��ʱ��
  
  systemError = initStatus;
}

/**
  * @brief  ���������ݴ���
  * @retval ��
  */
static void Process_Sensor_Data(void)
{
  uint32_t currentTime = HAL_GetTick();
  
  // ����Ƿ���Ҫ���´���������
  if (currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdateTime = currentTime;
    
    // �����Ԥ�Ƚ׶Σ�����Ƿ������Ԥ��
    if (isWarmingUp) {
      // ����Ԥ�Ƚ���
      uint32_t elapsed = currentTime - warmupStartTime;
      uint8_t percent = (elapsed * 100) / WARMUP_TIME;
      if (percent > 100) percent = 100;
      
      // ��ʾԤ�Ƚ�����
      OLED_ShowProgress(0, 4, 100, percent);
      
      // ����Ƿ�Ԥ�����
      if (elapsed >= WARMUP_TIME) {
        isWarmingUp = 0;  // Ԥ�����
        
        // Ԥ�Ƚ������ʼ��������
        if (Sensor_Init() != 0) {
          systemError |= 0x04;  // ��������ʼ��ʧ��
          systemStatus = 3;     // ����״̬
          OLED_ShowStatus(systemStatus, systemError);
        } else {
          // ����ϵͳ��������״̬
          systemStatus = 2;     // ��������
          // ��������ָʾ��
          HAL_GPIO_WritePin(RunLed_GPIO_Port, RunLed_Pin, GPIO_PIN_SET);
          OLED_ShowStatus(systemStatus, 0);
        }
        
        // ��ʱ1����ʾ״̬��Ϣ
        HAL_Delay(1000);
      }
    } else {
      // Ԥ����ɺ��������´���������
      if (Sensor_UpdateAllData(&sensorData) != 0) {
        // ���ݸ���ʧ�ܣ���ȡ������Ϣ
        uint8_t errorCode = Sensor_GetLastError();
        // ����������ִ�������ϵͳΪ����״̬
        systemError = 0x08 | errorCode;  // ����ʱ����
        systemStatus = 3;  // ����״̬
        OLED_ShowStatus(systemStatus, systemError);
      } else if (systemStatus == 3) {
        // �ָ�����״̬
        systemStatus = 2;
        OLED_ShowStatus(systemStatus, 0);
      }
    }
  }
}

/**
  * @brief  ������ʾ
  * @retval ��
  */
static void Update_Display(void)
{
  uint32_t currentTime = HAL_GetTick();
  
  // ����Ƿ���Ҫ������ʾ
  if (currentTime - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
    lastDisplayUpdateTime = currentTime;
    
    if (!isWarmingUp && systemStatus == 2) {
      // ��������״̬����ʾ����������
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
  * @brief  ���͵�����Ϣ������
  * @retval ��
  */
static void Send_Debug_Info(void)
{
  static uint32_t lastDebugTime = 0;
  uint32_t currentTime = HAL_GetTick();
  
  // ÿ2�뷢��һ�ε�����Ϣ
  if (currentTime - lastDebugTime >= 2000) {
    lastDebugTime = currentTime;
    
    // ���������Ϣ
    char buffer[100];
    int len;
    
    if (isWarmingUp) {
      // Ԥ�Ƚ׶���Ϣ
      uint32_t elapsed = currentTime - warmupStartTime;
      uint8_t percent = (elapsed * 100) / WARMUP_TIME;
      len = sprintf(buffer, "System warming up: %d%% complete\r\n", percent);
    } else {
      // ���н׶���Ϣ
      len = sprintf(buffer, "CF4 Data: Zero=%d, Range=%d, Conc=%d, Out=%d, %.2fV, %.2fmA, Status=%d\r\n",
                   sensorData.zeroPoint,
                   sensorData.rangeValue,
                   sensorData.concentration,
                   sensorData.outputValue,
                   sensorData.outputVoltage,
                   sensorData.outputCurrent,
                   systemStatus);
    }
    
    // ͨ��USART2���͵���λ��
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, 100);
  }
}

/**
  * @brief  ����������
  * @retval ��
  */
static void Process_Input(void)
{
  static uint32_t lastKey1Time = 0;
  static uint32_t lastKey2Time = 0;
  static uint8_t key1LastState = 1;
  static uint8_t key2LastState = 1;
  uint32_t currentTime = HAL_GetTick();
  uint8_t key1State, key2State;
  
  // ��ȡ����״̬
  key1State = HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin);
  key2State = HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin);
  
  // ����1���� (������)
  if (key1State != key1LastState && currentTime - lastKey1Time > 50) {
    lastKey1Time = currentTime;
    key1LastState = key1State;
    
    // �����ͷ�ʱִ�в���(�����ش���)
    if (key1State == 1) {
      // ����1���� - ��ʾϵͳ��Ϣ
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
      
      // ��ʾ�̼��汾
      OLED_ShowString(0, 6, "Firmware: v1.0");
      OLED_Refresh();
      
      // �ȴ�3��
      HAL_Delay(3000);
      
      // �ָ�������ʾ
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
  
  // ����2���� (������)
  if (key2State != key2LastState && currentTime - lastKey2Time > 50) {
    lastKey2Time = currentTime;
    key2LastState = key2State;
    
    // �����ͷ�ʱִ�в���(�����ش���)
    if (key2State == 1) {
      // ����2���� - ��ʾ��һ����Ϣ
      // �˴��ɸ���ʵ��������ӹ���
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  Ӧ�ó�����ڵ�
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU����--------------------------------------------------------*/

  /* ��λ�������裬��ʼ��Flash�ӿں�Systick */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* ����ϵͳʱ�� */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* ��ʼ�����������õ����� */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  
  /* USER CODE BEGIN 2 */
  // ��ʼ��OLED��ʾ
  OLED_Init(); // ��ʼ��OLED��ʾ��
  OLED_Clear(); // �����ʾ
  
  // ��ʾ��������
  OLED_ShowStatus(0, 0); // ��ʾ����״̬
  HAL_Delay(1000); // ��ʱ1��
  
  // ��ʼ��DAC7311
  DAC7311_Init(); // ��ʼ��DAC7311
  
  // ��ʼ��������ͨ��
  OLED_ShowStatus(1, 0); // ��ʾԤ��״̬
  if (Sensor_Init() != 0) {
    // ��������ʼ��ʧ��
    OLED_ShowStatus(3, Sensor_GetLastError()); // ��ʾ����״̬
    HAL_Delay(2000); // ��ʾ2�������Ϣ
  }
  
  // ϵͳԤ��
  uint8_t warmupSeconds = 60; // Ԥ��ʱ��60��
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_ShowProgress(0, 7, warmupSeconds, i+1); // ��ʾԤ�Ƚ���
    HAL_Delay(1000); // ��ʱ1��
  }
  
  // Ԥ����ɣ�������������ģʽ
  OLED_ShowStatus(2, 0); // ��ʾ����״̬
  HAL_Delay(1000); // ��ʾ1��
  /* USER CODE END 2 */

  /* ����ѭ�� */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // ���´���������
    SensorData_t sensorData; // ���崫�������ݽṹ��
    
    // ��ȡ���´���������
    if (Sensor_UpdateAllData(&sensorData) == 0) {
      // ���³ɹ�����ʾ����
      OLED_ShowValue(
        sensorData.zeroPoint,     // ���ֵ
        sensorData.rangeValue,    // ����ֵ
        sensorData.concentration, // Ũ��ֵ
        sensorData.outputValue,   // ���ֵ
        sensorData.outputVoltage, // �����ѹ
        sensorData.outputCurrent  // �������
      );
    } else {
      // ����ʧ�ܣ���ʾ������Ϣ
      OLED_ShowStatus(3, Sensor_GetLastError()); // ��ʾ����״̬
    }
    
    // ��������ָʾ��
    HAL_GPIO_TogglePin(RunLed_GPIO_Port, RunLed_Pin); // ��ת����ָʾ��״̬
    
    // ��ʱ
    HAL_Delay(500); // 500����ˢ��һ��
  }
  /* USER CODE END 3 */
}

/**
  * @brief ϵͳʱ������
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** ����RCC_OscInitTypeDef�ṹ��ָ���Ĳ�����ʼ��RCC����
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

  /** ��ʼ��CPU��AHB��APB����ʱ��
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
  * @brief  ��������
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* �û���������Լ���ʵ��������HAL���󷵻�״̬ */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  ����Դ�ļ�����Դ�кţ����з���assert_param����
  * @param  file: ָ��Դ�ļ�����ָ��
  * @param  line: assert_param������Դ���
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* �û���������Լ���ʵ���������ļ������кţ�
     ���磺printf("��������ֵ���ļ�%s����%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
