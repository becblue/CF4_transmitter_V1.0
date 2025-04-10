/**
  ******************************************************************************
  * @file    oled.c
  * @brief   OLED��ʾ����Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�ʵ����OLED��ʾ������������
  * ʹ��I2C1ͨ�ţ�֧���������»��ƣ��Ż�ˢ��Ч��
  * SSD1306�������������ֱ���128x64
  *
  ******************************************************************************
  */

/* �����ļ� ------------------------------------------------------------------*/
#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oledfont.h"  // ����ⶨ��

/* �������� ------------------------------------------------------------------*/
// 8x16�������ݣ�ASCII���ַ������ӿո�(0x20)��ʼ��
extern const unsigned char F8X16[];

// 6x8�������� 
extern const unsigned char F6x8[][6];

/* ˽�����Ͷ��� --------------------------------------------------------------*/

/* ˽�к궨�� ----------------------------------------------------------------*/

/* ˽�б��� ------------------------------------------------------------------*/
static OLED_DirtyFlag_t oledDirtyFlag;  // �������
static I2C_HandleTypeDef *oled_hi2c = &hi2c1;  // I2C���

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static void OLED_WriteCmd(uint8_t cmd);
static void OLED_WriteData(uint8_t data);
static void OLED_SetPosition(uint8_t x, uint8_t y);
static void OLED_MarkDirty(uint8_t page);
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr, uint8_t size);

/* ����ʵ�� ------------------------------------------------------------------*/

/**
  * @brief  OLED��ʼ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t OLED_Init(void)
{
  HAL_Delay(100);  // �ȴ�OLED�ϵ��ȶ�
  
  // ��ʼ����ʾ
  OLED_WriteCmd(0xAE);   // �ر���ʾ
  OLED_WriteCmd(0x20);   // �����ڴ�Ѱַģʽ
  OLED_WriteCmd(0x10);   // 00:ˮƽѰַģʽ, 01:��ֱѰַģʽ, 10:ҳѰַģʽ, 11:��Ч
  OLED_WriteCmd(0xB0);   // ����ҳ��ʼ��ַ,0-7
  OLED_WriteCmd(0xC8);   // ����COM�������0xC0:����/0xC8:��ת��
  OLED_WriteCmd(0x00);   // ���õ�����ʼ��ַ
  OLED_WriteCmd(0x10);   // ���ø�����ʼ��ַ
  OLED_WriteCmd(0x40);   // ������ʾ��ʼ�� [5:0]
  OLED_WriteCmd(0x81);   // ���öԱȶȿ���
  OLED_WriteCmd(0xCF);   // �Աȶ�ֵ,0-255
  OLED_WriteCmd(0xA1);   // ���ö���ӳ�� (0xA0:0->SEG0/0xA1:127->SEG0)
  OLED_WriteCmd(0xA6);   // ��������/���� (0xA6:����/0xA7:����)
  OLED_WriteCmd(0xA8);   // ���ö�·������ (16 to 63)
  OLED_WriteCmd(0x3F);   // 1/64 duty
  OLED_WriteCmd(0xD3);   // ������ʾƫ��
  OLED_WriteCmd(0x00);   // ��ƫ��
  OLED_WriteCmd(0xD5);   // ����ʱ�ӷ�Ƶ/����Ƶ��
  OLED_WriteCmd(0x80);   // ���÷�Ƶϵ��
  OLED_WriteCmd(0xD9);   // ����Ԥ�������
  OLED_WriteCmd(0xF1);   // ����Ԥ���
  OLED_WriteCmd(0xDA);   // ����COM����Ӳ������
  OLED_WriteCmd(0x12);   // [5:4]����COM����
  OLED_WriteCmd(0xDB);   // ����VCOMH��ƽ
  OLED_WriteCmd(0x40);   // ����VCOMȡ��ѡ�񼶱�
  OLED_WriteCmd(0x8D);   // ���ó���
  OLED_WriteCmd(0x14);   // ʹ�ܳ���(0x10:����/0x14:����)
  OLED_WriteCmd(0xAF);   // ������ʾ
  
  // ��ʼ���������
  memset(&oledDirtyFlag, 0, sizeof(oledDirtyFlag));
  
  // ����
  OLED_Clear();
  
  return 0;  // ��ʼ���ɹ�
}

/**
  * @brief  OLED����
  * @retval ��
  */
void OLED_Clear(void)
{
  // �����ʾ������
  memset(oledDirtyFlag.displayBuffer, 0, sizeof(oledDirtyFlag.displayBuffer));
  
  // ������ҳ���Ϊ����
  for (uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
    oledDirtyFlag.isDirty[i] = 1;  // ���Ϊ����
  }
  
  // ������ʾ
  OLED_Refresh();
}

/**
  * @brief  OLEDˢ����ʾ(����������)
  * @retval ��
  */
void OLED_Refresh(void)
{
  // ��������ҳ������������
  for (uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
    if (oledDirtyFlag.isDirty[i]) {
      // ����ҳ��ַ
      OLED_SetPosition(0, i);
      
      // д���ҳ������
      for (uint8_t j = 0; j < 128; j++) {
        OLED_WriteData(oledDirtyFlag.displayBuffer[i][j]);
      }
      
      // �������
      oledDirtyFlag.isDirty[i] = 0;
    }
  }
}

/**
  * @brief  OLED��ʾ�ַ���
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  str: Ҫ��ʾ���ַ���
  * @retval ��
  */
void OLED_ShowString(uint8_t x, uint8_t y, char *str)
{
  uint8_t j = 0;
  
  // ������
  if (y > 7) return;
  
  // ҳ���Ϊ����
  OLED_MarkDirty(y);
  
  // �����16���ظ����壬������һҳΪ����
  if (y < 7) {
    OLED_MarkDirty(y + 1);
  }
  
  // �����ʾ�ַ�
  while (str[j] != '\0') {
    OLED_DrawChar(x, y, str[j], 16);  // ʹ��8x16����
    x += 8;  // ��һ���ַ�λ��
    
    // ���������ʾ��Χ��������ʾ
    if (x > 120) {
      x = 0;
      y++;
      if (y > 7) {
        y = 0;  // �ص���һҳ
      }
      
      // ����µ�����
      OLED_MarkDirty(y);
      if (y < 7) {
        OLED_MarkDirty(y + 1);
      }
    }
    j++;
  }
}

/**
  * @brief  OLED��ʾ����
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  num: Ҫ��ʾ������
  * @param  len: ���ֳ���
  * @param  size: �����С(16/8)
  * @retval ��
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
  uint8_t t, temp;
  uint8_t enshow = 0;
  
  // ������
  if (y > 7) return;
  
  // ҳ���Ϊ����
  OLED_MarkDirty(y);
  
  // �����16���ظ����壬������һҳΪ����
  if (size == 16 && y < 7) {
    OLED_MarkDirty(y + 1);
  }
  
  // ��λ��ʾ����
  for (t = 0; t < len; t++) {
    temp = (num / pow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1)) {
      if (temp == 0) {
        OLED_DrawChar(x + (size / 2) * t, y, ' ', size);
        continue;
      } else {
        enshow = 1;
      }
    }
    OLED_DrawChar(x + (size / 2) * t, y, temp + '0', size);
  }
}

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
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t intLen, uint8_t fracLen, uint8_t size)
{
  uint32_t intPart;  // ��������
  uint32_t fracPart; // С������
  
  // ������
  if (y > 7) return;
  
  // ҳ���Ϊ����
  OLED_MarkDirty(y);
  
  // �����16���ظ����壬������һҳΪ����
  if (size == 16 && y < 7) {
    OLED_MarkDirty(y + 1);
  }
  
  // ������
  if (num < 0) {
    OLED_DrawChar(x, y, '-', size);
    x += size / 2;
    num = -num;
  }
  
  // ����������С������
  intPart = (uint32_t)num;
  fracPart = (uint32_t)((num - intPart) * pow(10, fracLen));
  
  // ��ʾ��������
  OLED_ShowNum(x, y, intPart, intLen, size);
  
  // ��ʾС����
  OLED_DrawChar(x + intLen * (size / 2), y, '.', size);
  
  // ��ʾС������
  OLED_ShowNum(x + (intLen + 1) * (size / 2), y, fracPart, fracLen, size);
}

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
                   uint16_t outputValue, float outputVoltage, float outputCurrent)
{
  char buffer[20]; // �ַ���������
  static uint8_t currentPage = 0; // ��ǰ��ʾҳ�棬0-����������1-�������
  static uint32_t lastToggleTime = 0; // �ϴ��л�ʱ��
  uint32_t currentTime = HAL_GetTick(); // ��ȡ��ǰʱ��
  
  // ÿ3���Զ��л�ҳ��
  if(currentTime - lastToggleTime > 3000) {
    currentPage = (currentPage + 1) % 2; // ѭ���л�ҳ��
    lastToggleTime = currentTime; // �����л�ʱ��
    
    // �л�ҳ��ʱ����
    for (uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
      memset(oledDirtyFlag.displayBuffer[i], 0, 128); // �����ʾ������
      oledDirtyFlag.isDirty[i] = 1; // ���Ϊ����
    }
  }
  
  // ��ʾ�������������̶���ʾ��
  OLED_ShowString(0, 0, "CF4 Sensor System"); // ��ʾϵͳ����
  
  // ��ʾ���߷ָ���
  for(uint8_t i = 0; i < 128; i++) {
    oledDirtyFlag.displayBuffer[1][i] = 0x80; // ����ˮƽ��
  }
  oledDirtyFlag.isDirty[1] = 1; // ���Ϊ����
  
  // ���ݵ�ǰҳ����ʾ��ͬ����
  if(currentPage == 0) { // ��һҳ����������
    // ��ʾҳ�����
    OLED_ShowString(0, 1, "Basic Parameters");
    
    // Ũ��ֵ����������ʾ��
    OLED_ShowString(0, 2, "Concentration:"); // ��ʾŨ�ȱ���
    sprintf(buffer, "%5d", concentration); // ��ʽ��Ũ��ֵ
    OLED_ShowString(0, 3, buffer); // ��ʾŨ����ֵ
    
    // ��ʾ�淶ֵָʾ
    if (concentration >= rangeValue) {
      OLED_ShowString(64, 3, "[MAX]"); // ����������ʾ���ֵ��־
    } else if (concentration <= zeroPoint) {
      OLED_ShowString(64, 3, "[MIN]"); // ���������ʾ��Сֵ��־
    } else {
      // ����Ũ�Ȱٷֱ�
      uint8_t percent = 0;
      if (rangeValue > zeroPoint) {
        percent = (uint8_t)(((float)(concentration - zeroPoint) / (rangeValue - zeroPoint)) * 100);
      }
      sprintf(buffer, "[%d%%]", percent); // ��ʽ���ٷֱ�
      OLED_ShowString(64, 3, buffer); // ��ʾ�ٷֱ�
    }
    
    // ��ʾ��������
    OLED_ShowString(0, 4, "Zero Point:"); // ��ʾ������
    sprintf(buffer, "%d", zeroPoint); // ��ʽ�����ֵ
    OLED_ShowString(72, 4, buffer); // ��ʾ�����ֵ
    
    OLED_ShowString(0, 5, "Range Value:"); // ��ʾ���̱���
    sprintf(buffer, "%d", rangeValue); // ��ʽ������ֵ
    OLED_ShowString(72, 5, buffer); // ��ʾ������ֵ
    
    // ��ʾ���ֵ
    OLED_ShowString(0, 6, "Output Value:"); // ��ʾ���ֵ����
    sprintf(buffer, "%d", outputValue); // ��ʽ�����ֵ
    OLED_ShowString(72, 6, buffer); // ��ʾ�����ֵ
    
    // ��ʾ������
    uint8_t progressPercent = 0;
    if (outputValue > 4095) {
      progressPercent = 100; // ������Χ��ʾ100%
    } else {
      progressPercent = (uint8_t)((float)outputValue * 100 / 4095); // ������Ȱٷֱ�
    }
    OLED_ShowProgress(0, 7, 100, progressPercent); // ��ʾ���ֵ������
  } 
  else { // �ڶ�ҳ���������
    // ��ʾҳ�����
    OLED_ShowString(0, 1, "Output Details");
    
    // ��ʾ�������ֵ
    OLED_ShowString(0, 2, "Digital Value:"); // ��ʾ�����������
    sprintf(buffer, "%d", outputValue); // ��ʽ���������ֵ
    OLED_ShowString(72, 2, buffer); // ��ʾ���������ֵ
    
    // ��ͼ�η�ʽ��ʾ��ǰ���ֵ(��������ͼ)
    OLED_ShowString(0, 3, "Output Level:"); // ��ʾ�����ƽ����
    uint8_t barWidth = (outputValue > 4095) ? 100 : (uint8_t)((float)outputValue * 100 / 4095); // �������ο��
    
    for(uint8_t i = 0; i < 100; i++) {
      if(i < barWidth) {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0xFF; // ���Ĳ���
      } else {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0x81; // δ���Ĳ���(�߿�)
      }
    }
    oledDirtyFlag.isDirty[4] = 1; // ���Ϊ����
    
    // ��ʾ��ѹ���
    OLED_ShowString(0, 5, "Voltage Output:"); // ��ʾ��ѹ�������
    sprintf(buffer, "%.2f V", outputVoltage); // ��ʽ����ѹֵ
    OLED_ShowString(72, 5, buffer); // ��ʾ��ѹ��ֵ
    
    // ��ʾ�������
    OLED_ShowString(0, 6, "Current Output:"); // ��ʾ�����������
    sprintf(buffer, "%.2f mA", outputCurrent); // ��ʽ������ֵ
    OLED_ShowString(72, 6, buffer); // ��ʾ������ֵ
    
    // ��ʾ״ָ̬ʾ
    OLED_ShowString(0, 7, "Status:"); // ��ʾ״̬����
    if (outputCurrent < 4.0f) {
      OLED_ShowString(48, 7, "Under Range"); // ���ڷ�Χ
    } else if (outputCurrent > 20.0f) {
      OLED_ShowString(48, 7, "Over Range"); // ������Χ
    } else {
      OLED_ShowString(48, 7, "Normal"); // ������Χ
    }
  }
  
  // ˢ����ʾ
  OLED_Refresh(); // ������Ļ��ʾ
}

/**
  * @brief  ��ʾϵͳ����״̬
  * @param  status: ״̬��
  * @param  errorCode: ������
  * @retval ��
  */
void OLED_ShowStatus(uint8_t status, uint8_t errorCode)
{
  // ��յ�0ҳ�͵�1ҳ
  memset(oledDirtyFlag.displayBuffer[0], 0, 128);
  memset(oledDirtyFlag.displayBuffer[1], 0, 128);
  oledDirtyFlag.isDirty[0] = 1;
  oledDirtyFlag.isDirty[1] = 1;
  
  // ��ʾϵͳ״̬
  OLED_ShowString(0, 0, "System Status:");
  
  // ����״̬����ʾ��ͬ״̬
  switch (status) {
    case 0:  // ������
      OLED_ShowString(0, 1, "Starting...");
      break;
    case 1:  // Ԥ����
      OLED_ShowString(0, 1, "Warming up...");
      break;
    case 2:  // ��������
      OLED_ShowString(0, 1, "Running");
      break;
    case 3:  // ����״̬
      OLED_ShowString(0, 1, "Error:");
      // ��ʾ�������
      char errStr[8];
      sprintf(errStr, "0x%02X", errorCode);
      OLED_ShowString(64, 1, errStr);
      break;
    default:
      OLED_ShowString(0, 1, "Unknown");
      break;
  }
  
  // ˢ����ʾ
  OLED_Refresh();
}

/**
 * @brief  ��ʾ������
 * @param  x: ��ʼ������
 * @param  y: ��ʼ������
 * @param  max_value: ���ֵ
 * @param  value: ��ǰֵ(0-100)
 * @retval ��
 */
void OLED_ShowProgress(uint8_t x, uint8_t y, uint8_t max_value, uint8_t value)
{
    uint8_t i;
    uint8_t length = 100;
    uint8_t progress;
    
    // �������ֵ
    if (value > max_value) {
        progress = length;
    } else {
        progress = (value * length) / max_value;
    }
    
    // ���ƽ������߿�
    OLED_ShowChar(x, y, '[');
    OLED_ShowChar(x + length/6 + 1, y, ']');
    
    // ���ƽ������ڲ����
    for (i = 0; i < length/6; i++) {
        if (i < progress/6) {
            OLED_ShowChar(x + i + 1, y, '=');
        } else {
            OLED_ShowChar(x + i + 1, y, ' ');
        }
    }
    
    // ��ʾ�ٷֱ�
    char buffer[10];
    sprintf(buffer, "%3d%%", value);
    OLED_ShowString(x + length/6 + 3, y, buffer);
    
    // ���ø���Ϊ������
    oledDirtyFlag.isDirty[y] = 1;
}

/**
  * @brief  OLED��ʾ�����ַ�
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  chr: Ҫ��ʾ���ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr)
{
  // ����DrawChar����ʵ����ʾ
  OLED_DrawChar(x, y, chr, 16);
  
  // ���ҳΪ����
  OLED_MarkDirty(y);
  
  // �����16���ظ����壬������һҳΪ����
  if (y < 7) {
    OLED_MarkDirty(y + 1);
  }
}

/**
  * @brief  д����
  * @param  cmd: Ҫд�������
  * @retval ��
  */
static void OLED_WriteCmd(uint8_t cmd)
{
  uint8_t buf[2];
  buf[0] = OLED_CMD;  // ����
  buf[1] = cmd;
  
  HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, buf, 2, 100);
}

/**
  * @brief  д����
  * @param  data: Ҫд�������
  * @retval ��
  */
static void OLED_WriteData(uint8_t data)
{
  uint8_t buf[2];
  buf[0] = OLED_DATA;  // ����
  buf[1] = data;
  
  HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, buf, 2, 100);
}

/**
  * @brief  ������ʾλ��
  * @param  x: �е�ַ(0-127)
  * @param  y: ҳ��ַ(0-7)
  * @retval ��
  */
static void OLED_SetPosition(uint8_t x, uint8_t y)
{
  OLED_WriteCmd(0xB0 + y);                   // ����ҳ��ַ
  OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);   // �����е�ַ��4λ
  OLED_WriteCmd(x & 0x0F);                   // �����е�ַ��4λ
}

/**
  * @brief  ���ҳΪ����
  * @param  page: ҳ��(0-7)
  * @retval ��
  */
static void OLED_MarkDirty(uint8_t page)
{
  if (page < DIRTY_BLOCK_NUM) {
    oledDirtyFlag.isDirty[page] = 1;
  }
}

/**
  * @brief  �����ַ�
  * @param  x: �е�ַ(0-127)
  * @param  y: ҳ��ַ(0-7)
  * @param  chr: Ҫ��ʾ���ַ�
  * @param  size: �����С(16/8)
  * @retval ��
  */
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr, uint8_t size)
{
  uint8_t c = 0, i = 0;
  
  // ���ַ�ASCII��תΪ�ֿ�����
  c = chr - ' ';
  
  // ��ͬ�����С����
  if (size == 16) {
    // 8x16����
    for (i = 0; i < 8; i++) {
      oledDirtyFlag.displayBuffer[y][x + i] = F8X16[c * 16 + i];
      oledDirtyFlag.displayBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8];
    }
  } else if (size == 8) {
    // 6x8����
    for (i = 0; i < 6; i++) {
      oledDirtyFlag.displayBuffer[y][x + i] = F6x8[c][i];
    }
  }
} 