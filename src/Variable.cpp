/*
*File Name:Variable.cpp
全局变量

*Project Name: ESP32S3 USB-METER MINI
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
*Associated Files:"Veriable.h"
*/
#include "Variable.h"

int InvertDisplay =1;
int LCD_Light =50;
int LCD_Rotation =0;

float Sampling_ohm (0.005f);
float LoadVoltage_V (0);
float LoadCurrent_A (0);
float LoadPower_W (0);
bool CurrentDirection (0);
float mAh (0),mWh (0),Ah (0),Wh (0);
float Capacity (0),Energy (0);
float Record_Power (0);
float MaxVoltage (5),MaxCurrent (0),MaxPower (0);
float MinVoltage (5),MinCurrent (0),MinPower (0);
float Zerodrift (0);
String ChargeProtocol ="[N/A]";
float v_DN (0),v_DP (0);
float v_CC1 (0),v_CC2 (0);
bool PD_Option(0); //default 0 FIX/1 PPS
bool PD_Ready(0); //default 0 not ready/1 ready
float PD_Voltage(0),PD_Current(0);
int PD_Src_Cap_Count(0); //PD Source Capability Count
int PD_Position(0); //PD Position
int ccbus_used (0); //CC线状态 0/NULL 1/CC1 2/CC2

uint64_t SNID;
int32_t NowTime (0),LastTime (0);
float NTCv (0),NTCm (0),NTC_Temperature (0);
int OTA_Progress (0);
int Now_App (1);
int All_Apps (13);
unsigned char LastApp = 0;
int Menu_Key (0);
uint32_t RunS,RunM,RunH;
String System_Error = "NO ERROR";
float CPU_Temperature = 0.0f;
uint32_t Sketch_Size = 0;
uint32_t Free_Flash_Size = 0;

int LCD_Light_addr =1;
int LCD_Rotation_addr =2;
int App_addr =3;
int WiFi_addr =23;
int CalibrationData_addr =24;
int ThresholdPower_addr =25;
int ZeroDrift_addr = 26;

float AccX,AccY,AccZ;
float GyroX,GyroY,GyroZ;
float AccAngleX,AccAngleY;
float GyroAngleX,GyroAngleY,GyroAngleZ;
float AngleX,AngleY,AngleZ;
char buf[128];

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long frameCount = 0;
float fps = 0.0;