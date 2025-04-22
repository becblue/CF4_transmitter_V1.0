/**
  ******************************************************************************
  * @file    led.c
  * @brief   LED控制模块源文件
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "led.h"
#include "main.h"

/* 私有变量 ------------------------------------------------------------------*/
static uint32_t lastLED1ToggleTime = 0;  // LED1最后一次翻转的时间，用于控制LED1的闪烁
static uint32_t lastLED2ToggleTime = 0;  // LED2最后一次翻转的时间，用于控制LED2的闪烁
static uint32_t lastLED2OnTime = 0;      // LED2最后一次点亮的时间，用于DAC更新指示
static uint8_t isLED2UpdateActive = 0;   // LED2是否处于DAC更新指示状态
static SystemState_t currentState = SYSTEM_INIT;  // 当前系统状态

/**
  * @brief  LED初始化
  * @param  无
  * @retval 无
  */
void LED_Init(void)
{
    // 初始化时两个LED都熄灭
    LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_OFF);  // LED1初始熄灭
    LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2初始熄灭
    
    // 初始化时间戳
    lastLED1ToggleTime = HAL_GetTick();  // 初始化LED1翻转时间戳
    lastLED2ToggleTime = HAL_GetTick();  // 初始化LED2翻转时间戳
    lastLED2OnTime = HAL_GetTick();      // 初始化LED2点亮时间戳
}

/**
  * @brief  设置LED状态
  * @param  GPIOx: LED所在的GPIO端口
  * @param  GPIO_Pin: LED对应的GPIO引脚
  * @param  state: LED状态（LED_ON或LED_OFF）
  * @retval 无
  */
void LED_SetState(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);  // 设置LED状态
}

/**
  * @brief  翻转LED状态
  * @param  GPIOx: LED所在的GPIO端口
  * @param  GPIO_Pin: LED对应的GPIO引脚
  * @retval 无
  */
void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);  // 翻转指定LED的状态
}

/**
  * @brief  更新LED状态
  * @param  state: 系统状态
  * @retval 无
  */
void LED_UpdateState(SystemState_t state)
{
    uint32_t currentTime = HAL_GetTick();  // 获取当前系统时间
    currentState = state;  // 更新当前系统状态

    switch (state)
    {
        case SYSTEM_INIT:  // 系统初始化状态
            // LED1闪烁（间隔100ms），LED2熄灭
            if (currentTime - lastLED1ToggleTime >= 100)  // 每100ms切换一次状态
            {
                LED_Toggle(LED1_GPIO_PORT, LED1_PIN);  // 切换LED1状态
                lastLED1ToggleTime = currentTime;  // 更新最后切换时间
            }
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2保持熄灭
            break;

        case SYSTEM_WARMUP:  // 系统预热状态
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_ON);  // LED1常亮
            // LED2闪烁（间隔100ms）
            if (currentTime - lastLED2ToggleTime >= 100)  // 每100ms切换一次状态
            {
                LED_Toggle(LED2_GPIO_PORT, LED2_PIN);  // 切换LED2状态
                lastLED2ToggleTime = currentTime;  // 更新最后切换时间
            }
            break;

        case SYSTEM_WORKING:  // 系统工作状态
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_ON);  // LED1常亮
            
            // LED2由DAC7311更新控制，在LED_HandleDAC7311Update中处理
            // 检查是否需要关闭LED2（点亮超过200ms）
            if (isLED2UpdateActive && (currentTime - lastLED2OnTime >= 200))
            {
                LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // 关闭LED2
                isLED2UpdateActive = 0;  // 清除更新标志
            }
            break;

        case SYSTEM_ERROR:  // 系统错误状态
            // LED1闪烁（间隔200ms）
            if (currentTime - lastLED1ToggleTime >= 200)  // 每200ms切换一次状态
            {
                LED_Toggle(LED1_GPIO_PORT, LED1_PIN);  // 切换LED1状态
                lastLED1ToggleTime = currentTime;  // 更新最后切换时间
            }
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // LED2保持熄灭
            break;

        default:
            LED_SetState(LED1_GPIO_PORT, LED1_PIN, LED_OFF);  // 默认状态LED1熄灭
            LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_OFF);  // 默认状态LED2熄灭
            break;
    }
}

/**
  * @brief  处理DAC7311更新后的LED显示
  * @retval 无
  */
void LED_HandleDAC7311Update(void)
{
    if (currentState != SYSTEM_WORKING) {  // 只在工作状态处理DAC更新指示
        return;
    }

    /* DAC7311更新后，LED2点亮200ms */
    LED_SetState(LED2_GPIO_PORT, LED2_PIN, LED_ON);  // 点亮LED2
    lastLED2OnTime = HAL_GetTick();         // 记录LED2点亮时间
    isLED2UpdateActive = 1;               // 设置更新活动标志
} 


