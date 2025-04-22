/**
  ******************************************************************************
  * @file    led.c
  * @brief   LED����ģ��Դ�ļ�
  ******************************************************************************
  */

/* �����ļ� ------------------------------------------------------------------*/
#include "led.h"
#include "main.h"

/* ˽�б��� ------------------------------------------------------------------*/
static uint32_t lastLED1ToggleTime = 0;  // LED1���һ�η�ת��ʱ�䣬���ڿ���LED1����˸
static uint32_t lastLED2ToggleTime = 0;  // LED2���һ�η�ת��ʱ�䣬���ڿ���LED2����˸
static uint32_t lastLED2OnTime = 0;      // LED2���һ�ε�����ʱ�䣬����DAC����ָʾ
static uint8_t isLED2UpdateActive = 0;   // LED2�Ƿ���DAC����ָʾ״̬
static SystemState_t currentState = SYSTEM_INIT;  // ��ǰϵͳ״̬

/**
  * @brief  LED��ʼ��
  * @param  ��
  * @retval ��
  */
void LED_Init(void)
{
    // ��ʼ��ʱ����LED��Ϩ��
    LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_OFF);  // LED1��ʼϨ��
    LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2��ʼϨ��
    
    // ��ʼ��ʱ���
    lastLED1ToggleTime = HAL_GetTick();  // ��ʼ��LED1��תʱ���
    lastLED2ToggleTime = HAL_GetTick();  // ��ʼ��LED2��תʱ���
    lastLED2OnTime = HAL_GetTick();      // ��ʼ��LED2����ʱ���
}

/**
  * @brief  ����LED״̬
  * @param  GPIOx: LED���ڵ�GPIO�˿�
  * @param  GPIO_Pin: LED��Ӧ��GPIO����
  * @param  state: LED״̬��LED_ON��LED_OFF��
  * @retval ��
  */
void LED_SetState(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);  // ����LED״̬
}

/**
  * @brief  ��תLED״̬
  * @param  GPIOx: LED���ڵ�GPIO�˿�
  * @param  GPIO_Pin: LED��Ӧ��GPIO����
  * @retval ��
  */
void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);  // ��תָ��LED��״̬
}

/**
  * @brief  ����LED״̬
  * @param  state: ϵͳ״̬
  * @retval ��
  */
void LED_UpdateState(SystemState_t state)
{
    uint32_t currentTime = HAL_GetTick();  // ��ȡ��ǰϵͳʱ��
    currentState = state;  // ���µ�ǰϵͳ״̬

    switch (state)
    {
        case SYSTEM_INIT:  // ϵͳ��ʼ��״̬
            // LED1��˸�����100ms����LED2Ϩ��
            if (currentTime - lastLED1ToggleTime >= 100)  // ÿ100ms�л�һ��״̬
            {
                LED_Toggle(LED1_GPIO_PORT, LED1_PIN);  // �л�LED1״̬
                lastLED1ToggleTime = currentTime;  // ��������л�ʱ��
            }
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2����Ϩ��
            break;

        case SYSTEM_WARMUP:  // ϵͳԤ��״̬
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_ON);  // LED1����
            // LED2��˸�����100ms��
            if (currentTime - lastLED2ToggleTime >= 100)  // ÿ100ms�л�һ��״̬
            {
                LED_Toggle(LED2_GPIO_PORT, LED2_PIN);  // �л�LED2״̬
                lastLED2ToggleTime = currentTime;  // ��������л�ʱ��
            }
            break;

        case SYSTEM_WORKING:  // ϵͳ����״̬
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_ON);  // LED1����
            
            // LED2��DAC7311���¿��ƣ���LED_HandleDAC7311Update�д���
            // ����Ƿ���Ҫ�ر�LED2����������200ms��
            if (isLED2UpdateActive && (currentTime - lastLED2OnTime >= 200))
            {
                LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // �ر�LED2
                isLED2UpdateActive = 0;  // ������±�־
            }
            break;

        case SYSTEM_ERROR:  // ϵͳ����״̬
            // LED1��˸�����200ms��
            if (currentTime - lastLED1ToggleTime >= 200)  // ÿ200ms�л�һ��״̬
            {
                LED_Toggle(LED1_GPIO_PORT, LED1_PIN);  // �л�LED1״̬
                lastLED1ToggleTime = currentTime;  // ��������л�ʱ��
            }
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2����Ϩ��
            break;

        default:
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_OFF);  // Ĭ��״̬LED1Ϩ��
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // Ĭ��״̬LED2Ϩ��
            break;
    }
}

/**
  * @brief  ����DAC7311���º��LED��ʾ
  * @retval ��
  */
void LED_HandleDAC7311Update(void)
{
    if (currentState != SYSTEM_WORKING) {  // ֻ�ڹ���״̬����DAC����ָʾ
        return;
    }

    /* DAC7311���º�LED2����200ms */
    LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_ON);  // ����LED2
    lastLED2OnTime = HAL_GetTick();         // ��¼LED2����ʱ��
    isLED2UpdateActive = 1;               // ���ø��»��־
} 


