/**
  ******************************************************************************
  * @file    oled.h
  * @brief   OLED��ʾ����ͷ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�������OLED��ʾ���������ӿں����ݽṹ
  * ʹ��I2Cͨ�ţ�֧���������»��ƣ��Ż�ˢ��Ч��
  * 
  ******************************************************************************
  */

/* ��ֹ�ݹ���� -------------------------------------*/
#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C" {
#endif

/* �����ļ� ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* ��ʾ�������� -----------------------------------------------------------*/
#define OLED_WIDTH      128     // OLED�������
#define OLED_HEIGHT     64      // OLED�߶�����
#define OLED_PAGE_NUM   8       // OLEDҳ��(ÿҳ8������)

/* OLED��ַ������� */
#define OLED_ADDR       0x78    // OLED�ӻ���ַ(0x3C<<1)
#define OLED_CMD        0x00    // д����
#define OLED_DATA       0x40    // д����

/* OLED�������Ŷ��� */
#define OLED_DC_GPIO_Port  GPIOB   // DC���Ŷ˿ڣ���������/����ѡ��
#define OLED_DC_Pin        GPIO_PIN_0   // DC���ű��
#define OLED_CS_GPIO_Port  GPIOB   // CSƬѡ���Ŷ˿�
#define OLED_CS_Pin        GPIO_PIN_1   // CSƬѡ���ű��
#define OLED_RST_GPIO_Port GPIOB   // RST��λ���Ŷ˿�
#define OLED_RST_Pin       GPIO_PIN_2   // RST��λ���ű��

/* ������ض��� */
#define DIRTY_BLOCK_NUM  8      // ����������(ÿҳһ��)
#define DIRTY_BLOCK_SIZE 128    // �������С(ÿҳ128���ֽ�)

/* OLED����� */
#define OLED_CMD_DISPLAY_ON     0xAF    // ������ʾ
#define OLED_CMD_DISPLAY_OFF    0xAE    // �ر���ʾ
#define OLED_CMD_SET_CONTRAST   0x81    // ���öԱȶ�����

/* ���ݽṹ���� -----------------------------------------------------------*/
/**
  * @brief  ������ǽṹ��
  */
typedef struct {
    uint8_t isDirty[DIRTY_BLOCK_NUM];          // ҳ����
    uint8_t displayBuffer[OLED_PAGE_NUM][128]; // ��ʾ������
} OLED_DirtyFlag_t;

/* �������� ------------------------------------------------------------------*/
/**
  * @brief  OLED��ʼ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t OLED_Init(I2C_HandleTypeDef *hi2c);

/**
  * @brief  OLED����
  * @retval ��
  */
void OLED_Clear(void);

/**
  * @brief  OLEDˢ����ʾ(����������)
  * @retval ��
  */
void OLED_Refresh(void);

/**
  * @brief  OLED��ʾ�ַ���
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  str: Ҫ��ʾ���ַ���
  * @retval ��
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

/**
  * @brief  OLED��ʾ�����ַ�
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  chr: Ҫ��ʾ���ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);

/**
  * @brief  OLED��ʾ����
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  num: Ҫ��ʾ������
  * @param  len: ���ֳ���
  * @retval ��
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

/**
  * @brief  OLED��ʾ������
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  num: Ҫ��ʾ�ĸ�����
  * @param  intLen: �������ֳ���
  * @param  fracLen: С�����ֳ���
  * @param  size: �����С
  * @retval ��
  */
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t intLen, uint8_t fracLen, uint8_t size);

/**
  * @brief  OLED��ʾ����������
  * @param  zeroPoint: ���ֵ
  * @param  rangeValue: ����ֵ
  * @param  concentration: Ũ��ֵ
  * @param  outputValue: ���ֵ(0-4095)
  * @param  outputVoltage: �����ѹ(0-3.3V)
  * @param  outputCurrent: �������(4-20mA)
  * @retval ��
  */
void OLED_ShowValue(uint16_t zeroPoint, uint16_t rangeValue, uint16_t concentration, 
                    uint16_t outputValue, float outputVoltage, float outputCurrent);

/**
  * @brief  ��ʾϵͳ����״̬
  * @param  status: ״̬��
  * @param  errorCode: ������
  * @retval ��
  */
void OLED_ShowStatus(uint8_t status, uint8_t errorCode);

/**
  * @brief  OLED��ʾ������
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  max_value: ���ֵ
  * @param  value: ��ǰֵ
  * @retval ��
  */
void OLED_ShowProgress(uint8_t x, uint8_t y, uint8_t max_value, uint8_t value);

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_SetPosition(uint8_t x, uint8_t y);
void OLED_SetContrast(uint8_t contrast);
void OLED_RefreshGram(void);

uint8_t OLED_WriteCmd(uint8_t command);  // д�������0��ʾ�ɹ���1��ʾʧ��
uint8_t OLED_WriteData(uint8_t data);    // д���ݣ�����0��ʾ�ɹ���1��ʾʧ��

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H__ */ 

