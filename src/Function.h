/*
*File Name:Function.h
运行函数

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
#ifndef FUNCTION_H
#define FUNCTION_H


class Function
{
private:
    /* data */
    float ShuntVoltage_mV;
    float BusVoltage;
    float ShuntCurrent;

    long imutick;
public:

    void System_Init();
    void System_Run();
    void GPIO_Init();
    void GPIO_Run();
    void INA22x_Init();
    void IMU_Init();
    void INA22x_Run();
    void IMU_Run();
    void FUSB_Init();
    void FUSB_Run();
    void IMU_Attitude();
    void AdjustINA22xConfig();
    void NTCTemperature_Run();
    void APP_Run();
    void SaveWiFiConfig();
    void ReadWiFiConfig();
    void DeleteWiFiConfig();
    void WebUpdate();
    void GetChargeProtocol();

    void UART_Debug();
};


extern Function FUNC;

#endif