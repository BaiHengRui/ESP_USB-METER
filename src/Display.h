/*
*File Name:Display.h
显示函数

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

*/
#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <queue>

class Display
{
private:
    /* data */
    int VoltageNum =0;
    int CurrentNum =0;
    int PowerNum =0;
    int OTA_ProgressNum =0;
    
    struct InfoMessage {
        String text;
        uint16_t bgColor;
        uint16_t textColor;
        int16_t win_width;
        int16_t win_height;
        uint32_t time;
        uint32_t startTime;
    };
    std::queue<InfoMessage> msgQueue;
    bool isShowingMsg = false;


public:
    void LCD_Init();
    void LCD_Light_Update(int light, bool saved);
    void LCD_Refresh_Screen(uint32_t color=0x0000);
    void LCD_Rotation_Update(int rotation_angle, bool saved);
    void Page1();
    void Page2();
    void Page3();
    void Page4();
    void Sys_Info();
    void Menu();
    void Change_LCD_Light();
    void MPU_Info();
    void OTA_Status();
    void OTA_End();
    void OTA_Error();
    void WiFi_Connect();
    void WiFi_Error();

    void MsgWindows(const String& msg,uint16_t win_color,uint16_t text_color,int16_t win_width,int16_t win_height,uint32_t win_time=2000);
    void MsgUpdate();
};

extern Display DISP;

#endif