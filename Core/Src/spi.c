/**
  ******************************************************************************
  * @file    spi.c
  * @brief   SPIͨ��Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�ʵ����SPIͨ����صĺ���
  * ������DAC7311���豸ͨ��
  *
  ******************************************************************************
  */

/* �����ļ� ------------------------------------------------------------------*/
#include "spi.h"

/* ˽�����Ͷ��� -----------------------------------------------------------*/
/* ˽�к궨�� ------------------------------------------------------------*/
/* ˽�б��� ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;  // SPI1���������DAC7311ͨ��

/* ����ʵ�� --------------------------------------------------------------*/

/**
  * @brief  SPI1 ��ʼ������
  * @retval ��
  */
void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;                       // ʹ��SPI1
  hspi1.Init.Mode = SPI_MODE_MASTER;           // ����ģʽ
  hspi1.Init.Direction = SPI_DIRECTION_2LINES; // ˫�ߵ���ģʽ
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;     // 8λ����֡
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;   // ʱ�Ӽ��ԣ�����״̬Ϊ�͵�ƽ
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;       // ��һ�����ز���
  hspi1.Init.NSS = SPI_NSS_SOFT;               // �������Ƭѡ�ź�
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // �����ʣ�PCLK2/16
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;      // ��λ����
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;      // ����TIģʽ
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // ����CRCУ��
  hspi1.Init.CRCPolynomial = 10;               // CRC����ʽ

  // ��ʼ��SPI
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();  // ��ʼ��ʧ�ܽ��������
  }
}

/**
  * @brief  SPI����ص�����
  * @param  hspi: SPI���ָ��
  * @retval ��
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  if(hspi->Instance == SPI1)
  {
    // ʹ��SPI1��GPIOAʱ��
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // ����SPI1��SCK����
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;     // �����������
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // ����
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // ����SPI1��MOSI����
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // ����DAC1��CS����(DAC1SYNC)
    GPIO_InitStruct.Pin = DAC1SYNC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // �������
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // ��������
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // ����
    HAL_GPIO_Init(DAC1SYNC_GPIO_Port, &GPIO_InitStruct);
    
    // ��ʼ״̬��CS�������ߣ�����DAC��
    HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);
  }
}

/**
  * @brief  SPI����ʼ������
  * @param  hspi: SPI���ָ��
  * @retval ��
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance == SPI1)
  {
    // ����SPI1ʱ��
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    // ȡ��SPI���ŵ�����
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_7);
    
    // ȡ��DAC1 CS���ŵ�����
    HAL_GPIO_DeInit(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin);
  }
} 