/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311����-ģ��ת��������ʵ���ļ�
  *          DAC7311��һ��12λ����ģת������ͨ��SPI�ӿڿ���
  ******************************************************************************
  */

/* ����ͷ�ļ� */
#include "dac7311.h"
#include "main.h"
#include "spi.h"

/**
  * @brief  ��ʼ��DAC7311
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_Init(void)
{
  /* ����DACΪ��������ģʽ */
  return DAC7311_SetPowerMode(DAC7311_POWER_DOWN_NORMAL);
}

/**
  * @brief  ����DAC7311���ֵ
  * @param  value: Ҫ���õ�DACֵ����Χ0-4095(12λ)
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
  uint16_t txData;
  HAL_StatusTypeDef status;
  
  /* ȷ��ֵ��0-4095��Χ�� */
  if (value > 4095)
  {
    value = 4095;
  }
  
  /* �������͵�����
     ��ʽ: [15:14]=00(��������ģʽ), [13:12]=00(����), [11:0]=DAC���� */
  txData = (DAC7311_POWER_DOWN_NORMAL << 12) | value;
  
  /* ʹ��SPI�������ݣ�ѡ��DAC��NSS���� */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_RESET); /* NSS���ͣ�ѡ��DAC */
  status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&txData, 2, 100);      /* ͨ��SPI�������� */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);   /* NSS���ߣ��ͷ�DAC */
  
  return (status == HAL_OK) ? 0 : 1; /* ����״̬��0�ɹ���1ʧ�� */
}

/**
  * @brief  ����DAC7311��Դģʽ
  * @param  mode: ��Դģʽ������������ֵ:
  *               DAC7311_POWER_DOWN_NORMAL - ��������ģʽ
  *               DAC7311_POWER_DOWN_1K - 1Kŷķ�ӵ�ģʽ
  *               DAC7311_POWER_DOWN_100K - 100Kŷķ�ӵ�ģʽ
  *               DAC7311_POWER_DOWN_HI_Z - ����ģʽ
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_SetPowerMode(uint16_t mode)
{
  uint16_t txData;
  HAL_StatusTypeDef status;
  
  /* �������͵����� */
  txData = (mode << 12); /* ��Դģʽ���ڸ���λ */
  
  /* ʹ��SPI�������� */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_RESET); /* NSS���ͣ�ѡ��DAC */
  status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&txData, 2, 100);      /* ͨ��SPI�������� */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);   /* NSS���ߣ��ͷ�DAC */
  
  return (status == HAL_OK) ? 0 : 1; /* ����״̬��0�ɹ���1ʧ�� */
}