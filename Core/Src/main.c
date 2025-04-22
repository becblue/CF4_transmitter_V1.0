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
  * �����ʹ�õ����������ڱ��������ĸ�Ŀ¼�е�LICENSE�ļ����ҵ�������
  * ���û���汾����ṩLICENSE�ļ�����"ԭ��"�ṩ��
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
#include "led.h"  // LED����ͷ�ļ�
//#include "oled.h"  // OLED��ʾͷ�ļ�����ʱ���ã�
#include "sensor.h"  // ���������ͷ�ļ�
#include "dac7311.h"  // DAC����ͷ�ļ�
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_UPDATE_INTERVAL   500       // ���������ݸ��¼��(ms)
#define DISPLAY_UPDATE_INTERVAL  1000      // ��ʾ���¼��(ms)
#define WARMUP_TIME              10000     // Ԥ��ʱ��(ms)���޸�Ϊ10��
#define DAC_VREF                 3.3f      // DAC�ο���ѹ��3.3V��
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// ��������ر���
static SensorData_t sensorData = {0};       // ���������ݽṹ�壬��ʼ��Ϊ0
static uint32_t lastSensorUpdateTime = 0;   // �ϴδ��������ݸ���ʱ��
//static uint32_t lastDisplayUpdateTime = 0;   // �ϴ���ʾ����ʱ�䣨��ʱ���ã�

// ϵͳ״̬��ر���
static uint32_t warmupStartTime = 0;        // Ԥ�ȿ�ʼʱ��
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
// ��������
//static void UpdateDisplay(SensorData_t *data);  // ��ʾ���º�����������ʱ���ã�
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
  MX_GPIO_Init();  // ��ʼ������GPIO
  MX_I2C1_Init();  // ��ʼ��I2C1
  MX_USART1_UART_Init();  // ��ʼ��USART1�����ڵ������
  MX_USART3_UART_Init();  // ��ʼ��USART3�����ڴ�����ͨ��

  /* USER CODE BEGIN 2 */
  // ��ʼ��LED
  LED_Init();  // ��ʼ��LED����ģ��
  LED_UpdateState(SYSTEM_INIT);  // ����LEDΪ��ʼ��״̬��LED1��˸��LED2Ϩ��
  
  // ��¼Ԥ�ȿ�ʼʱ��
  warmupStartTime = HAL_GetTick();  // ��¼Ԥ�ȿ�ʼʱ��
  
  /* ע�͵�OLED��ʼ����ش��� */
  
  // ��ʼ��DAC7311
  printf("��ʼ��DAC7311...\r\n");
  if (DAC7311_Init() != 0) {
    printf("DAC7311��ʼ��ʧ��!\r\n");
    LED_UpdateState(SYSTEM_ERROR);  // ����״̬��LED1��˸��LED2Ϩ��
    while(1) {
      LED_UpdateState(SYSTEM_ERROR);
      HAL_Delay(10);
    }
  }
  printf("DAC7311��ʼ�����\r\n");
  
  // ��ʼ��ʱ����DAC���Ϊ0
  DAC7311_SetValue(0);
  
  // ��ʼ��������
  printf("��ʼ��������...\r\n");
  if (Sensor_Init() != 0) {
    printf("��������ʼ��ʧ��! �������: %d\r\n", Sensor_GetLastError());
    LED_UpdateState(SYSTEM_ERROR);  // ����״̬��LED1��˸��LED2Ϩ��
    while(1) {
      LED_UpdateState(SYSTEM_ERROR);
      HAL_Delay(10);
    }
  }
  printf("��������ʼ�����\r\n");
  
  // �ڳ�ʼ���׶α���LED״̬����
  uint32_t currentTime = HAL_GetTick();
  printf("��ʼ��ʼ���׶μ��...\r\n");
  while(currentTime - warmupStartTime < 1000)
  {
    printf("��鴫����ͨ��...\r\n");
    if (Sensor_CheckConnection() != 0)
    {
      printf("������ͨ��ʧ��!\r\n");
      LED_UpdateState(SYSTEM_ERROR);  // ����״̬��LED1��˸��LED2Ϩ��
      while(1) {
        LED_UpdateState(SYSTEM_ERROR);
        HAL_Delay(10);
      }
    }
    printf("������ͨ������\r\n");
    LED_UpdateState(SYSTEM_INIT);  // ��ʼ��״̬��LED1��˸��LED2Ϩ��
    currentTime = HAL_GetTick();
    HAL_Delay(100);
  }
  printf("��ʼ���׶μ�����\r\n");
  
  // ����Ԥ�Ƚ׶�
  LED_UpdateState(SYSTEM_WARMUP);  // Ԥ��״̬��LED1������LED2��˸
  
  // �ȴ�Ԥ�����
  while(HAL_GetTick() - warmupStartTime < WARMUP_TIME) {
    LED_UpdateState(SYSTEM_WARMUP);  // ���ָ���LED״̬
    HAL_Delay(100);
  }
  
  // Ԥ����ɣ�������������״̬
  LED_UpdateState(SYSTEM_WORKING);  // ����״̬��LED1������LED2��DAC���¿���
  printf("ϵͳԤ����ɣ���ʼ��������\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    currentTime = HAL_GetTick();
    
    // ���´���������
    if(currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL)
    {
      // �������д���������
      if(Sensor_UpdateAllData(&sensorData) != 0)
      {
        printf("���������ݸ���ʧ��!\r\n");
        LED_UpdateState(SYSTEM_ERROR);  // ����״̬��LED1��˸��LED2Ϩ��
      }
      else
      {
        LED_UpdateState(SYSTEM_WORKING);  // ����״̬��LED1������LED2��DAC���¿���
        printf("[DAC] ��ǰ���ֵ: %d\r\n", sensorData.outputValue);
      }
      
      lastSensorUpdateTime = currentTime;
    }
    
    HAL_Delay(10);  // ������ʱ������CPUռ�ù���
  }
  /* USER CODE END 3 */
}

/* ע�͵�UpdateDisplay����ʵ��
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
  /* �û���������Լ���ʵ���������ļ������кţ�??
     ���磺printf("��������ֵ���ļ�%s����%d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
