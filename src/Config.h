/*
*File Name:Config.h
存放配置文件

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
*Associated Files: /
*/
#ifndef CONFIG_H
#define CONFIG_H

//版本号
//Major;Minor;Patch;Language
#define FirmwareVer "V1.1.33bCN"
#define HardwareVer "HW101"

//IO引脚
#define I2C_SDA_PIN 46  //I2C PIN 46
#define I2C_SCL_PIN 45  //I2C PIN 45
#define FUSB_INT_PIN 38  //FUSB INT PIN

#define LCD_BL_PIN 9  //LCD BLK PIN
#define DP_PIN 4  //ADC1 USB D+
#define DN_PIN 7  //ADC1 USB D-
#define CC1_PIN 5  //ADC1 CC1
#define CC2_PIN 6  //ADC1 CC2
#define VBUS_PIN 2  //ADC1 V-BUS

#define NTC_PIN 8 //NTC PIN

#define SW1 17  //Up
#define SW2 18  //Down
#define SW3 21  //Back
#define SW4 0  //Enter

#endif