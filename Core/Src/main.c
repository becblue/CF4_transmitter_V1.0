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
#define WARMUP_TIME              3000      // Ԥ��ʱ��(ms)
/* USER CODE END PD */

/* ˽�к� -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* ˽�б��� ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* ��Щ������ʱδʹ�ã�������Ҫȡ��ע��
static SensorData_t sensorData;            // ���������ݽṹ��
static uint32_t lastSensorUpdateTime = 0;  // �ϴδ��������ݸ���ʱ��
static uint32_t lastDisplayUpdateTime = 0;  // �ϴ���ʾ����ʱ��
static uint8_t systemError = 0;            // ϵͳ�����־
static uint8_t isWarmingUp = 1;            // Ԥ�ȱ�־
static uint32_t warmupStartTime = 0;       // Ԥ�ȿ�ʼʱ��
static uint8_t systemStatus = 0;           // ϵͳ״̬��0-������, 1-Ԥ����, 2-��������, 3-����
*/
/* USER CODE END PV */

/* ˽�к���ԭ�� -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* ˽���û����� ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  ������
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
  MX_USART1_UART_Init();  // ���ȳ�ʼ�����Դ���
  printf("\r\n\r\nϵͳ����...\r\n");  // �������������Ϣ
  printf("��ʼ����������...\r\n");
  
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  
  printf("�����ʼ�����\r\n");
  printf("��ǰϵͳʱ��Ƶ��: %d Hz\r\n", HAL_RCC_GetSysClockFreq());
  
  /* USER CODE BEGIN 2 */
  // ��ʼ��OLED��ʾ
  printf("��ʼ��OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {  // ��ʼ��OLED��ʾ��������I2C1���
    printf("OLED��ʼ��ʧ��!\r\n");
    Error_Handler();
  }
  printf("OLED��ʼ���ɹ�\r\n");
  
  OLED_Clear(); // �����ʾ
  OLED_ShowString(0, 0, "System Starting...");
  OLED_Refresh(); // ˢ����ʾ
  printf("OLED��ʾ������Ϣ\r\n");
  
  // ��ʼ��DAC7311
  if (DAC7311_Init(&hspi1, DAC1SYNC_GPIO_Port, DAC1SYNC_Pin) != 0) {
    // DAC��ʼ��ʧ��
    OLED_Clear();
    OLED_ShowString(0, 0, "DAC Init Failed!");
    OLED_Refresh();
    Error_Handler();
  }
  
  // ��ʼ��������ͨ��
  OLED_Clear();
  OLED_ShowString(0, 0, "Warming up...");
  OLED_Refresh();
  if (Sensor_Init() != 0) {
    // ��������ʼ��ʧ��
    OLED_Clear();
    OLED_ShowString(0, 0, "Sensor Error!");
    OLED_ShowString(0, 1, "Code: ");
    char errStr[8];
    sprintf(errStr, "%d", Sensor_GetLastError());
    OLED_ShowString(42, 1, errStr);
    OLED_Refresh();
    HAL_Delay(2000); // ��ʾ2�������Ϣ
  }
  
  // ϵͳԤ��
  uint8_t warmupSeconds = 3; // Ԥ��ʱ��3��
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "System Warming Up");
    char timeStr[16];
    sprintf(timeStr, "Wait %ds", warmupSeconds - i);  // ��ʾʣ������
    OLED_ShowString(0, 2, timeStr);
    OLED_Refresh();
    HAL_Delay(1000); // ��ʱ1��
  }
  
  // Ԥ����ɣ�������������ģʽ
  OLED_Clear();
  OLED_ShowString(0, 0, "System Ready");
  OLED_Refresh();
  HAL_Delay(1000); // ��ʾ1��
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE BEGIN 3 */
    // 1. ���������ݲɼ�����
    SensorData_t sensorData;  // �������������ݽṹ��
    if (Sensor_UpdateAllData(&sensorData) == 0)  // �������д���������
    {
      // 2. ���ݴ�������ʾ����
      // ��OLED����ʾ����
      OLED_Clear();  // ����
      
      // ��ʾŨ��ֵ
      char buf[32];
      sprintf(buf, "Ũ��: %d", sensorData.concentration);
      OLED_ShowString(0, 0, buf);
      
      // ��ʾ�����ѹ
      sprintf(buf, "��ѹ: %.2fV", sensorData.outputVoltage);
      OLED_ShowString(0, 2, buf);
      
      // ��ʾ�������
      sprintf(buf, "����: %.2fmA", sensorData.outputCurrent);
      OLED_ShowString(0, 4, buf);
      
      // 3. ������Ϣ���
      printf("���������ݸ��£�\r\n");
      printf("Ũ��: %d\r\n", sensorData.concentration);
      printf("���ֵ: %d\r\n", sensorData.outputValue);
      printf("��ѹ: %.2fV\r\n", sensorData.outputVoltage);
      printf("����: %.2fmA\r\n", sensorData.outputCurrent);
    }
    else
    {
      // ���ݸ���ʧ�ܣ���ʾ������Ϣ
      printf("����%s\r\n", Sensor_GetErrorString(Sensor_GetLastError()));
      
      // ��OLED����ʾ������Ϣ
      OLED_Clear();
      OLED_ShowString(0, 0, "���ݸ���ʧ��");
      OLED_ShowString(0, 2, Sensor_GetErrorString(Sensor_GetLastError()));
    }
    
    HAL_Delay(1000);  // ÿ�����һ������
    
    /* USER CODE END 3 */
  }
  /* USER CODE END WHILE */
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
