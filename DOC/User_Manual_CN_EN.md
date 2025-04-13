# CF4气体浓度检测系统使用说明书
# CF4 Gas Concentration Detection System User Manual

## 一、产品概述 / I. Product Overview

### 1.1 产品简介 / Product Introduction
本产品是一款专业的CF4气体浓度检测系统，用于工业环境中四氟化碳(CF4)气体的实时监测。
This product is a professional CF4 gas concentration detection system for real-time monitoring of tetrafluoromethane (CF4) gas in industrial environments.

### 1.2 主要特点 / Main Features
- 实时检测CF4气体浓度 / Real-time CF4 gas concentration detection
- 高精度12位DAC输出（0-3.3V） / High-precision 12-bit DAC output (0-3.3V)
- LED状态指示功能 / LED status indication
- OLED实时数据显示 / OLED real-time data display
- 故障检测与报警输出 / Fault detection and alarm output

### 1.3 技术参数 / Technical Parameters
- 工作电压 / Operating voltage: 5V DC
- 输出信号 / Output signal: 0-3.3V
- 显示方式 / Display: OLED
- 响应时间 / Response time: ≤500ms
- 工作温度 / Operating temperature: -10℃～50℃
- 工作湿度 / Operating humidity: ≤95%RH（无凝露 / No condensation）

## 二、安装说明 / II. Installation Instructions

### 2.1 开箱检查 / Unpacking Inspection
请检查以下物品 / Please check the following items:
- 主机设备 / Main device × 1
- 使用说明书 / User manual × 1
- 合格证 / Certificate × 1
- 电源适配器 / Power adapter × 1（如有订购 / If ordered）

### 2.2 接线说明 / Wiring Instructions
设备接口说明 / Device interface description:
1. 电源接口 / Power interface:
   - VCC: 5V电源正极 / 5V power positive
   - GND: 电源地 / Power ground

2. 信号输出 / Signal output:
   - VOUT: 模拟量输出 / Analog output (0-3.3V)
   - GND: 信号地 / Signal ground

## 三、使用说明 / III. Operating Instructions

### 3.1 开机流程 / Start-up Process
1. 初始化阶段（1秒）/ Initialization phase (1 second):
   - LED1闪烁（100ms间隔）/ LED1 flashing (100ms interval)
   - LED2熄灭 / LED2 off
   - OLED显示初始化信息 / OLED displays initialization information

2. 预热阶段（3秒）/ Warm-up phase (3 seconds):
   - LED1常亮 / LED1 steady on
   - LED2闪烁（100ms间隔）/ LED2 flashing (100ms interval)
   - OLED显示预热倒计时 / OLED displays warm-up countdown

3. 工作阶段 / Working phase:
   - LED1常亮 / LED1 steady on
   - LED2随数据更新闪烁 / LED2 flashes with data updates
   - OLED显示实时数据 / OLED displays real-time data

### 3.2 显示说明 / Display Instructions
OLED显示内容 / OLED display content:
- 气体浓度 / Gas concentration (PPM)
- 量程值 / Range value
- 零点值 / Zero point value
- 输出值 / Output value (0-4095)

### 3.3 LED指示说明 / LED Indicator Instructions
LED1（系统状态 / System status）:
- 初始化 / Initialization: 闪烁 / Flashing (100ms)
- 预热 / Warm-up: 常亮 / Steady on
- 工作 / Working: 常亮 / Steady on

LED2（工作状态 / Working status）:
- 初始化 / Initialization: 熄灭 / Off
- 预热 / Warm-up: 闪烁 / Flashing (100ms)
- 工作 / Working: 数据更新后亮200ms / On for 200ms after data update

## 四、注意事项 / IV. Precautions

### 4.1 安装注意事项 / Installation Precautions
1. 安装在通风处 / Install in ventilated area
2. 避免高温高湿 / Avoid high temperature and humidity
3. 远离强电磁干扰 / Keep away from strong electromagnetic interference
4. 正确接线，防止反接 / Correct wiring, prevent reverse connection

### 4.2 使用注意事项 / Operation Precautions
1. 等待预热完成 / Wait for warm-up completion
2. 定期检查接线 / Regular wiring check
3. 保持通风口清洁 / Keep ventilation clean
4. 定期检查数值 / Regular value check

## 五、故障排除 / V. Troubleshooting

### 5.1 常见故障 / Common Issues
1. 无法开机 / Cannot power on:
   - 检查电源连接 / Check power connection
   - 检查电源电压 / Check power voltage

2. 显示异常 / Display abnormal:
   - 检查OLED连接 / Check OLED connection
   - 重启设备 / Restart device

3. 输出异常 / Output abnormal:
   - 检查接线 / Check wiring
   - 检查输出端 / Check output terminal

### 5.2 错误状态 / Error States
当出现以下状态时 / When the following states occur:
1. LED同时闪烁 / LEDs flash simultaneously: 系统错误 / System error
2. OLED显示错误代码 / OLED shows error code: 记录并联系支持 / Record and contact support

## 六、联系方式 / VI. Contact Information

技术支持 / Technical Support:
- 热线 / Hotline: [电话号码 / Phone number]
- 邮箱 / Email: [邮箱地址 / Email address]
- 网址 / Website: [网址 / Website]

## 七、保修说明 / VII. Warranty Information

保修期 / Warranty period: 12个月 / 12 months
非保修范围 / Not covered:
- 人为损坏 / Human damage
- 使用不当 / Improper use
- 自行拆修 / Unauthorized repair

---
本说明书最终解释权归[公司名称]所有
The final interpretation right of this manual belongs to [Company name]