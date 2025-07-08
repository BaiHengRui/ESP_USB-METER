/*
*File Name:main.cpp
主函数

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
*Associated Files: /
*/
#include <Arduino.h>
#include "Variable.h"
#include "Library.h"
#include "Function.h"
#include "Display.h"
#include "Config.h"

extern Function FUNC;
extern Display DISP;

TaskHandle_t RTCTaskHandle;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;

void RTC_Time(void *pvParameters);
void sensorTask(void *pvParameters);
void displayTask(void *pvParameters);

//RTC运行秒，分，时
void RTC_Time(void *pvParameters) {
  for (;;)
  {
    if (LoadPower_W >= Record_Power)
    {
      RunS++;
      if (RunS == 60)
      {
        RunM++;
        RunS = 0;
      }if (RunM == 60)
      {
        RunH++;
        RunM = 0;
      }if (RunH == 1199 && RunM == 59 && RunS == 59)
      {
        Serial.print("The timers now max! Return to zero!\n");
        RunH = RunM = RunS = 0;     
      }
    }
    vTaskDelay(1000);
    //vTaskDelayUntil(1000);
  }  
}
// 传感器数据采集任务
void sensorTask(void *pvParameters) {
    for(;;) {
        // 传感器数据采集
        FUNC.INA22x_Run();
        FUNC.AdjustINA22xConfig();
        FUNC.GPIO_Run();
        FUNC.IMU_Run();
        FUNC.IMU_Attitude();
        FUNC.NTCTemperature_Run();
        FUNC.GetChargeProtocol();
        FUNC.UART_Debug(); 
        FUNC.FUSB_Run();
        FUNC.System_Run();
        vTaskDelay(pdMS_TO_TICKS(1)); // 1ms延迟
    }
}
// 显示更新任务
void displayTask(void *pvParameters) {
    unsigned long lastFpsUpdate = 0;
    
    for(;;) {
        // FPS计算
        frameCount++;
        unsigned long now = millis();
        if (now - lastFpsUpdate >= 1000) {
            fps = frameCount / ((now - lastFpsUpdate) / 1000.0);
            frameCount = 0;
            lastFpsUpdate = now;
        }
        
        // 显示更新
        FUNC.APP_Run();
        DISP.MsgUpdate();
        
        vTaskDelay(pdMS_TO_TICKS(1)); // 1ms延迟
    }
}
void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(240); //Max Freq 240MHz All Cores
  Serial.begin(115200);
  EEPROM.begin(1024);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  // Wire.setClock(1000000); // 1MHz高速模式
  FUNC.GPIO_Init();
  DISP.LCD_Init();
  // DISP.LCD_Refresh_Screen();// 初始化显示页面 抑制花屏
  DISP.Page1(); // 显示页面1
  FUNC.INA22x_Init();
  FUNC.IMU_Init();
  FUNC.System_Init();

  // DISP.Page1();
  DISP.LCD_Light_Update(EEPROM.read(LCD_Light_addr), 0);
  // Now_App =3;

  FUNC.FUSB_Init(); // 初始化FUSB302
  // 设置RTC任务
  xTaskCreatePinnedToCore(
      RTC_Time,
      "RTC_Task",
      2048,
      NULL,
      1,  // 优先级1
      &RTCTaskHandle,
      1
  );

  // 传感器任务
  xTaskCreatePinnedToCore(
      sensorTask,
      "SensorTask",
      4096,
      NULL,
      2,  // 优先级2
      &sensorTaskHandle,
      1
  );

  // 显示任务
  xTaskCreatePinnedToCore(
      displayTask,
      "DisplayTask",
      8192,
      NULL,
      3,  // 优先级3
      &displayTaskHandle,
      0
  );
  void RTC_Time(void *pvParameters);
  void sensorTask(void *pvParameters);
  void displayTask(void *pvParameters);
}

void loop() {      
  // put your main code here, to run repeatedly:
}
