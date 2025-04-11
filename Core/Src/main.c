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
  * �����ʹ�õ����������ڱ��������ĸ�Ŀ¼�е�LICENSE�ļ����ҵ�����
  * ���û���汾����ṩLICENSE�ļ�����"ԭ��"�ṩ��
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
#define SENSOR_UPDATE_INTERVAL   500       // ���������ݸ��¼��?(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // ��ʾ���¼��?(ms)
#define WARMUP_TIME              3000      // Ԥ��ʱ��(ms)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* ��Щ������ʱδʹ�ã�������Ҫȡ��ע��
static SensorData_t sensorData;            // ���������ݽṹ��
static uint32_t lastSensorUpdateTime = 0;  // �ϴδ��������ݸ���ʱ��
static uint32_t lastDisplayUpdateTime = 0;  // �ϴ���ʾ����ʱ��
static uint8_t systemError = 0;            // ϵͳ������?
static uint8_t isWarmingUp = 1;            // Ԥ�ȱ�־
static uint32_t warmupStartTime = 0;       // Ԥ�ȿ�ʼʱ��
static uint8_t systemStatus = 0;           // ϵͳ״̬��0-������, 1-Ԥ����, 2-��������, 3-����
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
  // ��ʼ��OLED��ʾ
  printf("��ʼ��OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {
    printf("OLED��ʼ��ʧ��!\r\n");
    Error_Handler();
  }
  printf("OLED��ʼ���ɹ�\r\n");
  
  OLED_Clear();
  OLED_ShowString(0, 0, "System Starting...");
  OLED_Refresh();
  printf("OLED��ʾ������Ϣ\r\n");
  
  // ��ʼ��DAC7311
  if (DAC7311_Init() != 0) {
    // DAC��ʼ��ʧ��
    OLED_Clear();
    OLED_ShowString(0, 0, "DAC Init Failed!");
    OLED_Refresh();
    Error_Handler();
  }
  
  // ��ʼ��������
  OLED_Clear();
  OLED_ShowString(0, 0, "Warming up...");
  OLED_Refresh();
  if (Sensor_Init() != 0) {
    // ��ʼ��ʧ��
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
  uint8_t warmupSeconds = 3; // ԤԤ��ʱ��3��
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "System Warming Up");
    char timeStr[16];
    sprintf(timeStr, "Wait %ds", warmupSeconds - i);  // ��ʾʣ��ʱ��
    OLED_ShowString(0, 2, timeStr);
    OLED_Refresh();
    HAL_Delay(1000); // �ȴ�1��
  }
  
  // Ԥ�����
  OLED_Clear();
  OLED_ShowString(0, 0, "System Ready");
  OLED_Refresh();
  HAL_Delay(1000); // ��ʾ1��
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 1. ���������ݲɼ�����
    SensorData_t sensorData;  // ���崫�������ݽṹ��
    if (Sensor_UpdateAllData(&sensorData) == 0)  // ���Ը������д���������
    {
      // 2. ���ݴ������ʾ����
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
      printf("\r\n[%lu ms] ���������ݸ��£�\r\n", HAL_GetTick());
      printf("Ũ��: %d\r\n", sensorData.concentration);
      printf("���ֵ: %d\r\n", sensorData.outputValue);
      printf("��ѹ: %.2fV\r\n", sensorData.outputVoltage);
      printf("����: %.2fmA\r\n", sensorData.outputCurrent);
    }
    else
    {
      // ���ݸ���ʧ�ܣ���ʾ������Ϣ
      printf("\r\n[%lu ms] ����%s\r\n", HAL_GetTick(), Sensor_GetErrorString(Sensor_GetLastError()));
      
      // ��OLED����ʾ������Ϣ
      OLED_Clear();
      OLED_ShowString(0, 0, "���ݸ���ʧ��");
      OLED_ShowString(0, 2, Sensor_GetErrorString(Sensor_GetLastError()));
    }
    
    HAL_Delay(1000);  // ÿ�����һ������
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
  /* �û���������Լ���ʵ��������HAL���󷵻�״̬ */
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
  /* �û���������Լ���ʵ���������ļ������кţ�?
     ���磺printf("��������ֵ���ļ�%s����%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
