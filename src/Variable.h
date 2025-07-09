/*
*File Name:Variable.h
全局变量

*Project Name: ESP32S3 USB-METER
*FramWork: Arduino
*Project Version: /
*Data: 2025/04/12
*Author Name:Bai Hengrui
*Copyright: Copyright (c) 2025 Bai Hengrui
*Open Source License: CC BY-NC-SA 4.0
*Last Edition: /
*Build IDE：GCC
*Important Build Version：/
*Update Explanation: /

*/

#ifndef VARIABLE_H
#define VARIABLE_H

#include <Arduino.h>
/*Display*/
extern int InvertDisplay;//屏幕反色
extern int LCD_Light;//屏幕亮度
extern int LCD_Rotation;//屏幕旋转角度

/*BUS*/
extern float Sampling_ohm;//采样电阻
extern float LoadVoltage_V;//负载电压
extern float LoadCurrent_A;//负载电流
extern float LoadPower_W;//负载功率
extern bool CurrentDirection;//电流方向 true:正向 false:反向
extern float mAh,mWh,Ah,Wh;//电量，能量
extern float Capacity,Energy;//电池容量，能量
extern float Record_Power;//记录功率
extern float MaxVoltage,MaxCurrent,MaxPower;//最大电压，电流，功率
extern float MinVoltage,MinCurrent,MinPower;//最小电压，电流，功率
extern float Zerodrift;//零点校准
extern String ChargeProtocol;//充电协议
extern float v_DN,v_DP;// D+ D- 电压
extern float v_CC1,v_CC2;// CC1 CC2 电压
extern bool PD_Option;// PD选项
extern bool PD_Ready;// PD就绪状态
extern float PD_Voltage,PD_Current;// PD电压，电流
extern int PD_Src_Cap_Count;// PD源能力计数
extern int PD_Position;// PD位置
extern int ccbus_used;// CC线状态 0/NULL 1/CC1 2/CC2

/*System*/
extern uint64_t SNID;//序列号
extern int32_t NowTime,LastTime;//当前时间，上一时间
extern float NTCv,NTCm,NTC_Temperature;//NTC电压，NTC电阻，NTC温度
extern int OTA_Progress;// OTA进度
extern int Now_App;//当前应用
extern int All_Apps;//所有应用数量
extern unsigned char LastApp;//上一个应用
extern int Menu_Key;//菜单键
extern uint32_t RunS,RunM,RunH;//RTC运行秒，分，时
extern String System_Error;//系统错误信息
extern float CPU_Temperature;//CPU温度
extern uint32_t Sketch_Size;//程序大小
extern uint32_t Free_Flash_Size;//剩余闪存大小

extern int LCD_Light_addr;//屏幕亮度地址
extern int LCD_Rotation_addr;//屏幕旋转地址
extern int App_addr;//应用地址
extern int WiFi_addr;// WiFi配置地址
extern int CalibrationData_addr;// 校准数据地址
extern int ThresholdPower_addr;// 阈值功率地址
extern float Zerodrift_addr;// 零点漂移地址

extern float AccX,AccY,AccZ;//加速度
extern float GyroX,GyroY,GyroZ;//角速度
extern float AccAngleX,AccAngleY;//加速度倾角
extern float GyroAngleX,GyroAngleY,GyroAngleZ;//角速度
extern float AngleX,AngleY,AngleZ;//陀螺仪倾角
extern char buf[128];//PD协议缓冲区

extern unsigned long previousMillis;// 上一次时间戳
extern unsigned long currentMillis;// 当前时间戳
extern unsigned long frameCount;// 帧计数
extern float fps;// 帧率
#endif