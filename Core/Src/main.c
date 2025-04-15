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
#include "oled.h"  // OLED��ʾͷ�ļ�
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
#define WARMUP_TIME              3000      // Ԥ��ʱ��(ms)
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
static uint32_t lastDisplayUpdateTime = 0;   // �ϴ���ʾ����ʱ��

// ϵͳ״̬��ر���
static uint32_t warmupStartTime = 0;        // Ԥ�ȿ�ʼʱ��
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
// ��������
static void UpdateDisplay(SensorData_t *data);  // ��ʾ���º�������
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
  MX_GPIO_Init();  // ��ʼ������GPIO
  MX_I2C1_Init();  // ��ʼ��I2C1������OLED��ʾ
  MX_USART1_UART_Init();  // ��ʼ��USART1�����ڵ������
  MX_USART3_UART_Init();  // ��ʼ��USART3�����ڴ�����ͨ��
  /* USER CODE BEGIN 2 */
  // ��ʼ��LED
  LED_Init();  // ��ʼ��LED����ģ��
  LED_UpdateState(SYSTEM_INIT);  // ����LEDΪ��ʼ��״̬��LED1��˸��LED2Ϩ��
  
  // ��¼Ԥ�ȿ�ʼʱ��
  warmupStartTime = HAL_GetTick();  // ��¼Ԥ�ȿ�ʼʱ��
  
  // ��ʼ��OLED��ʾ
  printf("��ʼ��OLED...\r\n");
  if(OLED_Init(&hi2c1) != 0) {
    printf("OLED��ʼ��ʧ��!\r\n");
    LED_UpdateState(SYSTEM_ERROR);  // ����LEDΪ����״̬
    while(1) {  // ͣ���ڴ���״̬
      LED_UpdateState(SYSTEM_ERROR);  // ����LED����״̬��ʾ
      HAL_Delay(10);  // С��ʱ������CPUռ�ù���
    }
  }
  printf("OLED��ʼ�����\r\n");
  
  // ��ʼ��DAC7311
  printf("��ʼ��DAC7311...\r\n");  // ��ӡ��ʼ����ʼ��Ϣ
  if (DAC7311_Init() != 0) {  // ��ʼ��DAC7311
    printf("DAC7311��ʼ��ʧ��!\r\n");  // ��ӡ������Ϣ
    LED_UpdateState(SYSTEM_ERROR);  // ����LEDΪ����״̬
    OLED_Clear();  // ���OLED��ʾ
    OLED_ShowString(0, 0, "DAC Init Failed!");  // ��ʾ������Ϣ
    OLED_Refresh();  // ˢ��OLED��ʾ
    while(1) {  // ͣ���ڴ���״̬
      LED_UpdateState(SYSTEM_ERROR);  // ����LED����״̬��ʾ
      HAL_Delay(10);  // С��ʱ������CPUռ�ù���
    }
  }
  printf("DAC7311��ʼ�����\r\n");  // ��ӡ��ʼ�������Ϣ
  
  // ��ʼ��ʱ����DAC���Ϊ0
  DAC7311_SetValue(0);  // ����DAC���Ϊ0
  
  // ��ʼ��������
  printf("��ʼ��������...\r\n");
  if (Sensor_Init() != 0) {
    printf("��������ʼ��ʧ��! �������: %d\r\n", Sensor_GetLastError());
    LED_UpdateState(SYSTEM_ERROR);  // ����LEDΪ����״̬
    OLED_Clear();
    OLED_ShowString(0, 0, "Sensor Error!");
    OLED_ShowString(0, 1, "Code: ");
    char errStr[8];
    sprintf(errStr, "%d", Sensor_GetLastError());
    OLED_ShowString(42, 1, errStr);
    OLED_Refresh();
    while(1) {  // ͣ���ڴ���״̬
      LED_UpdateState(SYSTEM_ERROR);  // ����LED����״̬��ʾ
      HAL_Delay(10);  // С��ʱ������CPUռ�ù���
    }
  }
  printf("��������ʼ�����\r\n");
  
  // �ڳ�ʼ���׶α���LED״̬����
  uint32_t currentTime = HAL_GetTick();
  printf("��ʼ��ʼ���׶μ��...\r\n");
  while(currentTime - warmupStartTime < 1000)  // ��ʼ���׶γ���1��
  {
    // ֻ��鴫����ͨ��״̬��ʹ��05ָ��
    printf("��鴫����ͨ��...\r\n");
    if (Sensor_CheckConnection() != 0)  // ֻ���ͨ��״̬
    {
      printf("������ͨ��ʧ��!\r\n");
      LED_UpdateState(SYSTEM_ERROR);  // ����LEDΪ����״̬
      OLED_Clear();
      OLED_ShowString(0, 0, "Sensor Error!");
      OLED_ShowString(0, 2, "Init Failed!");
      OLED_Refresh();
      while(1) {  // ͣ���ڴ���״̬
        LED_UpdateState(SYSTEM_ERROR);  // ����LED����״̬��ʾ
        HAL_Delay(10);  // С��ʱ������CPUռ�ù���
      }
    }
    printf("������ͨ������\r\n");
    LED_UpdateState(SYSTEM_INIT);  // LED1��˸��LED2Ϩ��
    currentTime = HAL_GetTick();
    HAL_Delay(100);  // �����ʱ���������Ƶ����ͨ��
  }
  printf("��ʼ���׶μ�����\r\n");
  
  // ����Ԥ�Ƚ׶�
  LED_UpdateState(SYSTEM_WARMUP);  // �л���Ԥ��״̬��LED1������LED2��˸
  
  // ϵͳԤ��
  uint8_t warmupSeconds = 3; // Ԥ��ʱ��3��
  for (uint8_t i = 0; i < warmupSeconds; i++) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Warming Up...");
    char timeStr[16];
    sprintf(timeStr, "Wait %ds", warmupSeconds - i);  // ��ʾʣ��ʱ��
    OLED_ShowString(0, 2, timeStr);
    OLED_Refresh();
    
    // ��ÿ�뿪ʼʱֻ��鴫����ͨ��״̬
    if (Sensor_CheckConnection() != 0)  // ֻ���ͨ��״̬
    {
        LED_UpdateState(SYSTEM_ERROR);  // ����LEDΪ����״̬
        OLED_Clear();
        OLED_ShowString(0, 0, "Sensor Error!");
        OLED_ShowString(0, 2, "Warmup Failed!");
        OLED_Refresh();
        while(1) {  // ͣ���ڴ���״̬
            LED_UpdateState(SYSTEM_ERROR);  // ����LED����״̬��ʾ
            HAL_Delay(10);  // С��ʱ������CPUռ�ù���
        }
    }
    
    // ����һ����ֻ����LED״̬���������κδ�������ѯ
    uint32_t startTick = HAL_GetTick();
    while(HAL_GetTick() - startTick < 1000)  // ����1��
    {
        LED_UpdateState(SYSTEM_WARMUP);  // LED1������LED2��˸
        HAL_Delay(100);  // ÿ100ms����һ��LED״̬
    }
    DAC7311_SetValue(0);  // ȷ��Ԥ�Ƚ׶�DAC�������Ϊ0
  }
  
  // Ԥ�Ƚ��������빤��״̬
  LED_UpdateState(SYSTEM_WORKING);  // �л�������״̬��LED1������LED2��DAC���¿���
  
  // Ԥ����ɺ�ĵ�һ�����ݻ�ȡ
  HAL_Delay(100);  // ������ʱ��ȷ����������ȫ����
  
  // ��ȡ��ʼ����������
  if (Sensor_UpdateAllData(&sensorData) == 0)
  {
    OLED_Clear();
    char buf[32];
    
    // ��ʾŨ��ֵ
    sprintf(buf, "PPM:%d", sensorData.concentration);
    OLED_ShowString(0, 0, buf);
    
    // ��ʾ����ֵ
    sprintf(buf, "Range:%d", sensorData.rangeValue);
    OLED_ShowString(0, 2, buf);
    
    // ��ʾ���ֵ
    sprintf(buf, "Zero:%d", SENSOR_ZERO_POINT);
    OLED_ShowString(0, 3, buf);
    
    // ��ʾDAC���ֵ
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
    static uint8_t errorCount = 0;  // ���������
    static uint8_t isInErrorState = 0;  // ����״̬��־
    
    // ����Ƿ���Ҫ���´���������
    if (currentTime - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL)
    {
      if (Sensor_UpdateAllData(&sensorData) == 0)  // ������ͨ�ųɹ�
      {
        errorCount = 0;  // ����������
        if (isInErrorState)  // ���֮ǰ���ڴ���״̬
        {
          isInErrorState = 0;  // �������״̬
          LED_UpdateState(SYSTEM_WORKING);  // �ָ�������״̬
          
          // �ָ�������ʾ
          OLED_Clear();
          UpdateDisplay(&sensorData);
        }
        
        // ����DAC���Ϊʵ��ֵ
        DAC7311_SetVoltage((float)sensorData.outputValue * DAC_VREF / 4095.0f, DAC_VREF);
        if (!isInErrorState)  // ֻ�ڷǴ���״̬�¸���LED2
        {
          LED_HandleDAC7311Update();  // ����LED2״̬
        }
      }
      else  // ������ͨ��ʧ��
      {
        errorCount++;  // ���Ӵ������
        if (errorCount >= 3)  // ����3��ͨ��ʧ��
        {
          if (!isInErrorState)  // �����δ���ڴ���״̬
          {
            isInErrorState = 1;  // ���ô���״̬��־
            LED_UpdateState(SYSTEM_ERROR);  // �л�������״̬
            
            // ��ʾ������Ϣ��DAC����仯��Ϣ
            printf("[����] ������ͨ��ʧ�ܣ�DAC�����������������\r\n");
            printf("[DAC] ��ǰ���ֵ: %d��Ŀ��ֵ: %d\r\n", sensorData.outputValue, DAC7311_FULL_SCALE);
            
            // ׼����ʾ����
            char dacStr[32];
            OLED_Clear();  // ����
            
            // ��һ����ʾ������Ϣ
            OLED_ShowString(0, 0, "Sensor Error!");
            
            // �ڶ�����ʾ���������ʾ
            OLED_ShowString(0, 2, "Check Connection");
            
            // ��������ʾ��ǰDACֵ
            sprintf(dacStr, "DAC:%d", sensorData.outputValue);
            OLED_ShowString(0, 4, dacStr);
            
            // ��������ʾĿ��ֵ
            sprintf(dacStr, "Target:%d", DAC7311_FULL_SCALE);
            OLED_ShowString(0, 6, dacStr);
            
            // һ����ˢ����ʾ
            OLED_Refresh();
            
            // ����DAC�����������
            DAC7311_RampToValue(DAC7311_FULL_SCALE);
            
            // ������ʾ����DACֵ
            OLED_Clear();  // �ٴ�����
            OLED_ShowString(0, 0, "Sensor Error!");
            OLED_ShowString(0, 2, "Check Connection");
            sprintf(dacStr, "DAC Output:");
            OLED_ShowString(0, 4, dacStr);
            sprintf(dacStr, "Set to %d", DAC7311_FULL_SCALE);
            OLED_ShowString(0, 6, dacStr);
            OLED_Refresh();  // ˢ����ʾ
            
            printf("[DAC] ���������Ϊ������: %d\r\n", DAC7311_FULL_SCALE);
          }
          LED_UpdateState(SYSTEM_ERROR);  // ���ִ���״̬��LED��ʾ
        }
      }
      lastSensorUpdateTime = currentTime;
    }
    
    // ֻ�ڷǴ���״̬�¸�����ʾ��LED״̬
    if (!isInErrorState)
    {
      // ������ʾ
      if (currentTime - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL)
      {
        UpdateDisplay(&sensorData);
        lastDisplayUpdateTime = currentTime;
      }
      
      // ����LED״̬
      LED_UpdateState(SYSTEM_WORKING);
    }
    else
    {
      // �ڴ���״̬�±���LED������ʾ
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

/**
  * @brief  ������ʾ����
  * @param  data: ���������ݽṹ��ָ��
  * @retval None
  */
static void UpdateDisplay(SensorData_t *data)
{
    if (data == NULL) {  // ��������Ч��
        return;
    }
    
    OLED_Clear();  // �����ʾ
    char buf[32];   // ��ʾ������
    
    // ��ʾŨ��ֵ
    sprintf(buf, "PPM:%d", data->concentration);
    OLED_ShowString(0, 0, buf);
    
    // ��ʾ����ֵ
    sprintf(buf, "Range:%d", data->rangeValue);
    OLED_ShowString(0, 2, buf);
    
    // ��ʾ���ֵ
    sprintf(buf, "Zero:%d", SENSOR_ZERO_POINT);
    OLED_ShowString(0, 3, buf);
    
    // ��ʾDAC���ֵ
    sprintf(buf, "DAC:%d/4095", data->outputValue);
    OLED_ShowString(0, 6, buf);
    
    OLED_Refresh();  // ˢ����ʾ
}
