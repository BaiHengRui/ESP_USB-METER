/*
*File Name:Display.cpp
显示函数

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
*Associated Files:"Display.h"

*Explanations:
    修改了TFT_eSPI库(../lib/TFT_eSPI/User_Setup.h)
*/
#include "Config.h"
#include "Library.h"
#include "Display.h"
#include "Variable.h"
//#include <math.h>

#include "Fonts/Font1_12.h"  //MiSans-Demibold(EN)
#include "Fonts/Font1_14.h"  //OPPOSans-B-2(EN)
#include "Fonts/Font1_18.h"  //OPPOSans-M-2(CN)
#include "Fonts/Font1_20.h"
#include "Fonts/Font1_28.h"
#include "Fonts/Font1_40.h"

Display DISP;
TFT_eSPI tft = TFT_eSPI(240,240);
TFT_eSprite spr = TFT_eSprite(&tft);
TFT_eSprite spr_win = TFT_eSprite(&tft);
// static TFT_eSprite spr(&tft);
// static TFT_eSprite spr_win(&tft);

#define VNUM_POINTS 160  //VBUS 与格子宽度一致
#define SNUM_POINTS 160 //信号
float VoltageData[VNUM_POINTS] = {0};
float CurrentData[VNUM_POINTS] = {0};
float PowerData[VNUM_POINTS] = {0};
float voltageMax = 10;    // 初始值10V
float voltageMin = 0;
float currentMax = 1;     // 初始值1A
float currentMin = 0;;
float powerMax = 50;
float powerMin = 0;

void Display::LCD_Init(){
    //初始化屏幕背光引脚黑屏解决开机花屏问题，需要先初始化引脚，最后setup恢复
    analogWrite(LCD_BL_PIN,0);
    spr.init();
    spr.invertDisplay(InvertDisplay);
    tft.setRotation(LCD_Rotation);
    
    int Light_temp = EEPROM.read(LCD_Light_addr);
    int Rotation_temp = EEPROM.read(LCD_Rotation_addr);
    LCD_Light = (Light_temp >=1 && Light_temp <=100) ? Light_temp : LCD_Light;
    LCD_Rotation = (Rotation_temp >=0 && Rotation_temp <= 3) ? Rotation_temp : LCD_Rotation;
    if (Light_temp >=1 && Light_temp <= 100)
    {
        EEPROM.write(LCD_Light_addr,LCD_Light);
        EEPROM.commit();
    }
    if (Rotation_temp >=0 && Rotation_temp <=3)
    {
        EEPROM.write(LCD_Rotation_addr,LCD_Rotation);
    }

}

void Display::LCD_Light_Update(int light, bool saved){
    light = constrain(light, 1, 100); //限制
    int light_pwm = 255 - ((100 - light) * 1.5);
    analogWrite(LCD_BL_PIN,light_pwm);
    if (saved ==1)
    {
        EEPROM.write(LCD_Light_addr,light);
        EEPROM.commit();
    }
    
}

void Display::LCD_Refresh_Screen(uint32_t color){
    spr.fillScreen(color);
    // spr.deleteSprite();
}

void Display::LCD_Rotation_Update(int rotation_angle, bool saved){
    LCD_Rotation = rotation_angle;
    tft.setRotation(LCD_Rotation);
    if (saved == 1)
    {
        EEPROM.write(LCD_Rotation_addr,LCD_Rotation);
        EEPROM.commit();
    } 
    // LCD_Refresh_Screen();
}

void Display::Page1(){
    VoltageNum = LoadVoltage_V * 11;//条的长度
    CurrentNum = LoadCurrent_A * 24;
    PowerNum = LoadPower_W / 2;

    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_40);

    spr.fillRoundRect(1,1,238,45,1,0x2444);//游标x,y 矩形w宽h高 圆角半径 填充颜色 0x2444
    spr.fillRoundRect(1,1,VoltageNum,45,1,0x0320);

    spr.fillRoundRect(1,47,238,45,1,0xFB56);
    spr.fillRoundRect(1,47,CurrentNum,45,1,0xFB56);

    spr.fillRoundRect(1,93,238,45,1,0x05FF);
    spr.fillRoundRect(1,93,PowerNum,45,1,0x0019);

    spr.setTextColor(TFT_WHITE);
    spr.setCursor(5,5);  
    spr.print(LoadVoltage_V, LoadVoltage_V < 10 ? 6 : 5);
    spr.setCursor(5,52); 
    spr.print(LoadCurrent_A, LoadCurrent_A < 10 ? 6 : 5);
    spr.setCursor(5,97); 
    spr.print(LoadPower_W, LoadPower_W < 10 ? 6 : (LoadPower_W < 100 ? 5 : 4));

    spr.setCursor(160,5);
    spr.print("V");
    spr.setCursor(160,52);
    spr.print("A");
    spr.setCursor(160,97);
    spr.print("W");    
    spr.unloadFont();

    //电流方向14号
    spr.loadFont(Font1_14);
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(220,50);
    if (CurrentDirection == 1)
    {
        spr.print(">");
    }else{spr.print("<");}

    //下方信息区 14号
    spr.setCursor(5,150);
    spr.setTextColor(0xffff);
    spr.print("Protocol：");

    spr.setCursor(145,150);
    spr.setTextColor(TFT_ORANGE);
    spr.print("D+:");
    spr.print(v_DP,3);
    spr.print("V");
    spr.setCursor(145,167);
    //spr.setTextColor(TFT_CYAN);
    spr.print("D-:");
    spr.print(v_DN,3);
    spr.print("V");
    //=========================
    spr.setTextColor(TFT_CYAN,0x0000);
    spr.setCursor(145,185);
    spr.print("CC1:");
    spr.print(v_CC1,3);
    spr.setCursor(145,202);
    spr.print("CC2:");
    spr.print(v_CC2,3);
    if (ccbus_used == 0)
    {
        spr.drawRoundRect(143, 184, 90, 17, 5, TFT_YELLOW);
    }else if (ccbus_used == 1)
    {
        spr.drawRoundRect(143, 201, 90, 17, 5, TFT_YELLOW);
    }

    spr.setCursor(200,2);
    spr.setTextColor(TFT_WHITE,0x0000);
    spr.print(fps, 1);

    spr.setCursor(5,197);
    //PD Position
    if (PD_Ready == 1)
    {
        if (PD_Option == 0)
        {
            spr.print("FIX:" + String(PD_Voltage*0.05,2) + "V " + String(PD_Current*0.01,2) + "A");
        }else if (PD_Option == 1)
        {
            spr.print("PPS:" + String(PD_Voltage*0.02,2) + "V " + String(PD_Current*0.05,2) + "A");
        }
    }
    
    spr.unloadFont();
    //RTC时间显示
    spr.loadFont(Font1_12);
    spr.setCursor(5,220);
    spr.setTextColor(TFT_WHITE, 0x0000);
    if (PD_Ready == 0)
    {
        spr.setCursor(5,220); spr.print(RunH < 10 ? "0" : ""); spr.print(RunH);
        spr.setCursor(25,219); spr.print(":");
        spr.setCursor(30,220); spr.print(RunM < 10 ? "0" : ""); spr.print(RunM);
        spr.setCursor(50,219); spr.print(":");
        spr.setCursor(55,220); spr.print(RunS < 10 ? "0" : ""); spr.print(RunS);
    }else if (PD_Ready == 1)
    {
        spr.print("SRC:" + String(PD_Src_Cap_Count));
        spr.print("-POS:" + String(PD_Position));
    }
    
    // spr.setCursor(5,220);  spr.print(RunH < 10 ? "0" : ""); spr.print(RunH);
    // spr.setCursor(25,219); spr.print(":");
    // spr.setCursor(30,220); spr.print(RunM < 10 ? "0" : ""); spr.print(RunM);
    // spr.setCursor(50,219); spr.print(":");
    // spr.setCursor(55,220); spr.print(RunS < 10 ? "0" : ""); spr.print(RunS);
    spr.unloadFont();
    //结束

    spr.loadFont(Font1_28);
    spr.setTextColor(TFT_WHITE);  
    spr.setCursor(5,168);
    spr.print(ChargeProtocol);
    spr.unloadFont();

    spr.loadFont(Font1_14);
    spr.setCursor(145,220);
    spr.print("TEMP:");
    if (NTC_Temperature >= 100) {
        spr.print(NTC_Temperature, 0);
    } else if (NTC_Temperature > -100 && NTC_Temperature < 100) {
        int decimalPlaces = (NTC_Temperature < 10 && NTC_Temperature > -10) ? 2 : 1;
        spr.print(NTC_Temperature, decimalPlaces);
    } else {
        spr.print("ERR");
    }
    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

void Display::Page2() {
    spr.createSprite(240, 240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);

    // 更新数据缓冲区
    for (int i = VNUM_POINTS - 1; i > 0; i--) {
        VoltageData[i] = VoltageData[i - 1];
        CurrentData[i] = CurrentData[i - 1];
    }
    VoltageData[0] = LoadVoltage_V * 10;
    CurrentData[0] = LoadCurrent_A * 10;

    // 安全获取极值函数
    auto safeExtremes = [](float* data, int len) -> std::pair<float, float> {
        if (len == 0) return {0, 0};
        float minVal = data[0], maxVal = data[0];
        for (int i = 1; i < len; ++i) {
            if (data[i] < minVal) minVal = data[i];
            if (data[i] > maxVal) maxVal = data[i];
        }
        return {minVal, maxVal};
    };

    // 获取极值并动态调整量程
    std::pair<float, float> vExtremes = safeExtremes(VoltageData, VNUM_POINTS);
    float vMin = vExtremes.first;
    float vMax = vExtremes.second;
    std::pair<float, float> cExtremes = safeExtremes(CurrentData, VNUM_POINTS);
    float cMin = cExtremes.first;
    float cMax = cExtremes.second;

    // 计算数据波动范围
    float vRange = vMax - vMin;
    float cRange = cMax - cMin;

    // 电压量程调整
    if (vRange < 1.0f) {  // 如果波动小于1.0
        float center = (vMax + vMin) / 2;  // 计算中心值
        voltageMin = center - 0.5f;        // 保持±0.5的范围
        voltageMax = center + 0.5f;
    } else {
        // 正常自动调整逻辑
        voltageMax = vMax + vRange * 0.1f;
        voltageMin = vMin - vRange * 0.1f;
    }
    voltageMin = std::max(voltageMin, 0.0f); // 电压不低于0
    // 电流量程调整

    if (cRange < 0.1f) {
        float center = (cMax + cMin) / 2;
        currentMin = center - 0.05f;
        currentMax = center + 0.05f;
    } else {
        currentMax = cMax + cRange * 0.1f;
        currentMin = cMin - cRange * 0.1f;
    }

    currentMin = std::max(currentMin, 0.0f);
    if (currentMax - currentMin < 0.1f) currentMax = currentMin + 0.1f;
    // 安全映射函数
    auto safeMap = [](float value, float inMin, float inMax, int outMin, int outMax) {
        if (inMin >= inMax) return (outMin + outMax) / 2; // 防除零保护
        return (int)((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
    };

    // 计算每个竖格（div）的值
    float voltsPerDivision = (voltageMax - voltageMin) / 3.0f;
    float ampsPerDivision = (currentMax - currentMin) / 3.0f;

    // 绘制区域参数
    const int graphX = 40;      // 左侧留空40px
    const int graphY = 40;      // 顶部留空40px
    const int graphWidth = 160; // 绘图区宽度
    const int graphHeight = 160;// 绘图区高度

    // 绘制网格系统
    spr.loadFont(Font1_12);
    spr.setTextColor(TFT_WHITE);

    // 左侧电压刻度（绿色）
    spr.setTextColor(TFT_GREEN);
    for (int i = 0; i <= 3; i++) {
        float val = voltageMax - (voltageMax - voltageMin) * i / 3;
        int y = safeMap(val, voltageMin, voltageMax, graphY + graphHeight, graphY);
        spr.drawString(String(val/10,2), graphX-20, y+2); // 左侧显示电压值
    }

    // 右侧电流刻度（黄色）
    spr.setTextColor(TFT_YELLOW);
    for (int i = 0; i <= 3; i++) {
        float val = currentMax - (currentMax - currentMin) * i / 3;
        int y = safeMap(val, currentMin, currentMax, graphY + graphHeight, graphY);
        spr.drawString(String(val/10,2), graphX + graphWidth + 15, y+2); // 右侧显示电流值
    }

    // 绘制网格线
    for (int i = 0; i <= 3; i++) {
        int y = safeMap(voltageMin + (voltageMax-voltageMin)*i/3, 
                      voltageMin, voltageMax, graphY+graphHeight, graphY);
        spr.drawLine(graphX, y, graphX+graphWidth, y, 0x7BCF);
    }
    //绘制四根竖向的网格线
    for (int i = 0; i <= 3; i++) {
        int x = graphX + (graphWidth * i) / 3;
        spr.drawLine(x, graphY, x, graphY + graphHeight, 0x7BCF);
    }
    // 绘制双曲线
    // 曲线绘制方向（从右向左）
    for (int i = 1; i < VNUM_POINTS; i++) {
        int reversedIdx1 = VNUM_POINTS - 1 - (i - 1);
        int reversedIdx2 = VNUM_POINTS - 1 - i;

        // 电压曲线（绿色）
        int x1 = safeMap(reversedIdx1, 0, VNUM_POINTS - 1, graphX, graphX + graphWidth);
        int y1 = safeMap(VoltageData[i - 1], voltageMin, voltageMax, graphY + graphHeight, graphY);
        int x2 = safeMap(reversedIdx2, 0, VNUM_POINTS - 1, graphX, graphX + graphWidth);
        int y2 = safeMap(VoltageData[i], voltageMin, voltageMax, graphY + graphHeight, graphY);
        spr.drawLine(x1, y1, x2, y2, TFT_GREEN);

        // 电流曲线（黄色）
        int cy1 = safeMap(CurrentData[i - 1], currentMin, currentMax, graphY + graphHeight, graphY);
        int cy2 = safeMap(CurrentData[i], currentMin, currentMax, graphY + graphHeight, graphY);
        spr.drawLine(x1, cy1, x2, cy2, TFT_YELLOW);
    }
    //实时电压电流
    spr.setTextColor(TFT_GREEN);
    spr.setTextDatum(TL_DATUM); // 左对齐
    spr.drawString("V:" + String(LoadVoltage_V,4), 30, 2);
    spr.drawString(String(voltsPerDivision/10, 2) + String("V/d"), 30, 20);

    spr.setTextColor(TFT_YELLOW);
    spr.setTextDatum(TR_DATUM); // 右对齐
    spr.drawString("A:" + String(LoadCurrent_A,4), 210, 2); 
    spr.drawString(String(ampsPerDivision/10, 2) + String("A/d"), 210, 20);

    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

const uint8_t MAX_LINES = 9;          // 最大显示行数 9*24 + 24
const uint8_t LINE_HEIGHT = 24;        // 每行像素高度（根据字体大小调整）
const uint8_t START_Y = 1;            // 起始显示行数
String displayBuffer[MAX_LINES];       // 显示缓冲区
void Display::Page3(){
    spr.createSprite(240, 240);
    spr.fillSprite(TFT_BLACK);
    spr.setTextDatum(TL_DATUM); // 左对齐
    spr.setColorDepth(8);

    // spr.setCursor(0, 0);
    spr.loadFont(Font1_18); // 加载字体
    spr.fillRect(0,0,240,22,TFT_GREEN); // 绘制顶部边框
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(5, 2); // 设置光标位置
    spr.print("PDO Log >"); // 显示标题
    spr.unloadFont();

    spr.loadFont(Font1_12);
    spr.setTextColor(TFT_WHITE);
    static bool firstRun = true; // 用于第一次加载时的特殊处理
    if (firstRun) {
        // 初始化显示缓冲区
        for (int i = 0; i < MAX_LINES; i++)
        {
            displayBuffer[i] = "";
            firstRun = false; // 只在第一次运行时执行
        } 
    }
    static long PDtimeMillis = millis(); // 用于记录PD时间
    if (millis() - PDtimeMillis >= 200)
    {
        PDtimeMillis = millis(); // 更新PD时间
        // 更新显示缓冲区
        for (int i = 0; i < MAX_LINES -1; i++)
        {
            displayBuffer[i] = displayBuffer[i + 1]; // 向上移动一行
        }
        // displayBuffer[MAX_LINES - 1] = String(buf); // 将最新的PD数据放在最后一行
    }
    displayBuffer[MAX_LINES - 1] = String(buf); // 将最新的PD数据放在最后一行
    spr.fillRect(0,24,spr.width(), LINE_HEIGHT * MAX_LINES, TFT_BLACK); // 清除显示区域
    for (int i = 0; i < MAX_LINES; i++)
    {
        // spr.drawString(displayBuffer[i], 0, START_Y + i * LINE_HEIGHT); // 绘制每一行
        spr.drawString(displayBuffer[i], 0, 24 + i * LINE_HEIGHT, 1); // 绘制每一行
    }

    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

void Display::Page4(){
    /**/
}

void Display::Sys_Info() {
    float Total_Flash_Size = (Sketch_Size + Free_Flash_Size)/2; // 总闪存大小KB OTA1
    float usageRatio = (Total_Flash_Size > 0) ? (Sketch_Size / Total_Flash_Size) : 0; // 计算使用比例
    uint16_t progressWidth = (usageRatio * 200); // 进度条宽度

    spr.createSprite(240, 240);
    spr.fillSprite(TFT_BLACK); // 黑色背景
    spr.setTextDatum(CC_DATUM);
    spr.setTextColor(TFT_WHITE);
    spr.loadFont(Font1_12);

    // ====================
    spr.setTextColor(TFT_CYAN);
    spr.drawFastHLine(0, 0, 240, TFT_DARKGREY); // 顶部装饰线
    spr.drawCentreString("SYSTEM INFORMATION", 120, 15, 2); // 主标题
    
    //设备信息区域
    spr.setTextColor(TFT_YELLOW);
    spr.drawFastHLine(10, 35, 220, TFT_DARKGREY);
    spr.drawString("DEVICE INFO", 120, 45, 2);
    
    spr.setTextColor(TFT_WHITE);
    spr.setTextDatum(TL_DATUM); // 左对齐
    spr.setCursor(15, 60);
    spr.print("SN: " + String(SNID, HEX));
    
    spr.setCursor(15, 75);
    spr.printf("Firmware: %s", FirmwareVer);
    
    //系统信息
    spr.setTextColor(TFT_YELLOW);
    spr.drawFastHLine(10, 90, 220, TFT_DARKGREY); // Y=90
    spr.drawString("SYSTEM STATUS", 80, 100, 2); // Y=100 (之前是95)
    
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(15, 115);
    spr.printf("Tick: %02d:%02d:%02d", 
              (millis() / 3600000) % 24,
              (millis() / 60000) % 60,
              (millis() / 1000) % 60);
    CPU_Temperature = temperatureRead();
    spr.setCursor(15, 130);
    spr.printf("CPU Temp: %.1f C", CPU_Temperature);
    
    // 错误状态显示
    spr.setCursor(15, 145);
    if (System_Error == 0) {
        spr.setTextColor(TFT_GREEN);
        spr.print("Status: OK");
    } else {
        spr.setTextColor(TFT_RED);
        spr.print("ERROR CODE: " + String(System_Error));
    }
    
    //固件信息区域
    spr.setTextColor(TFT_YELLOW);
    spr.drawFastHLine(10, 155, 220, TFT_DARKGREY); // Y=155
    spr.drawString("STORAGE USAGE", 80, 160, 2); // Y=160
    // 显示存储信息
    spr.setTextColor(TFT_WHITE);
    spr.setCursor(15, 175);
    spr.print(String(Sketch_Size) + "/" + String(Total_Flash_Size,0) + " KB-");
    spr.printf("%.1f%%", usageRatio * 100);
    // 进度条背景
    spr.fillRoundRect(15, 190, 200, 12, 3, TFT_DARKGREY); 
    
    // 进度条前景
    spr.fillRoundRect(15, 190, progressWidth, 12, 3, TFT_GREEN);
    

    spr.setTextColor(TFT_YELLOW);
    spr.drawFastHLine(10, 215, 220, TFT_DARKGREY);
    
    // 显示MD5校验值
    spr.setTextColor(TFT_WHITE);
    String md5 = ESP.getSketchMD5();
    // String shortMD5 = md5.substring(0, 8) + "..." + md5.substring(md5.length() - 8);
    // spr.drawCentreString("MD5: " + md5, 2, 225, 1);
    spr.setCursor(5,225);
    spr.print(md5);


    spr.drawFastHLine(0, 239, 240, TFT_DARKGREY); // 底部装饰线
    spr.unloadFont();
    // 推送显示
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

void Display::Menu() {
    spr.createSprite(240, 240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_20);

    spr.fillRoundRect(225, 30, 15, 198, 2, 0xffff);
    spr.fillRoundRect(225, 30, 15, (Menu_Key * 18), 2, 0x2444);

    spr.setTextColor(TFT_WHITE);
    spr.setCursor(100, 5);
    spr.print("菜单");
    if (Menu_Key < 9) {
        if (Menu_Key == 1) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 30);
        spr.print("1.设备信息");

        if (Menu_Key == 2) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 55);
        spr.print("2.屏幕亮度");

        if (Menu_Key == 3) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 80);
        spr.print("3.陀螺仪");

        if (Menu_Key == 4) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 105);
        spr.print("4.PD Trigger");

        if (Menu_Key == 5) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 130);
        spr.print("5.PDO LOG");

        if (Menu_Key == 6) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 155);
        spr.print("6.OTA");

        if (Menu_Key == 7) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 180);
        spr.print("7.NULL");

        if (Menu_Key == 8) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 205);
        spr.print("8.NULL");

        spr.setTextColor(0xFFFF);
        spr.setCursor(200, 205);
        spr.print("↓");
    } else if (Menu_Key >= 9) {
        if (Menu_Key == 9) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 30);
        spr.print("9.NULL");

        if (Menu_Key == 10) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 55);
        spr.print("10.NULL");

        if (Menu_Key == 11) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 80);
        spr.print("11.NULL");

        if (Menu_Key == 12) {
            spr.setTextColor(0x3666);
        } else {
            spr.setTextColor(0xFFE0);
        }
        spr.setCursor(5, 105);
        spr.print("12.NULL");

        spr.setTextColor(0xFFFF);
        spr.setCursor(200, 205);
        spr.print("↑");
    }
    //spr.setTextColor(0xFFFF);
    //spr.setCursor(200,210);
    //spr.print(MENU_KEY);
    spr.unloadFont();
    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

void Display::Change_LCD_Light(){
    //unsigned int lastlight;
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setColorDepth(8);
    spr.loadFont(Font1_20);
    spr.setTextColor(0xFFFF);
    spr.setCursor(5,5);
    spr.print("屏幕亮度调节");
    spr.setCursor(5,100);
    spr.print("当前亮度");
    spr.print(LCD_Light);
    LCD_Light_Update(LCD_Light,1);

    if(digitalRead(SW4) == LOW){
        delay(500);
        Now_App = LastApp;
    }
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::MPU_Info(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setColorDepth(8);
    spr.loadFont(Font1_20);
    spr.setTextColor(0xFFFF);
    spr.setCursor(5,5);
    spr.print("陀螺仪测试(MPU6050)");
    spr.unloadFont();

    spr.loadFont(Font1_14);
    spr.setTextColor(TFT_RED);
    spr.setCursor(5,30);
    spr.print("AccX:");
    spr.print(AccX,1);
    spr.setCursor(5,50);
    spr.print("AccY:");
    spr.print(AccY,1);
    spr.setCursor(5,70);
    spr.print("AccZ:");
    spr.print(AccZ,1);

    spr.setCursor(5,90);
    spr.setTextColor(TFT_GREEN);
    spr.print("GyroX:");
    spr.print(GyroX,1);
    spr.setCursor(5,110);
    spr.print("GyroY:");
    spr.print(GyroY,1);
    spr.setCursor(5,130);
    spr.print("GyroZ:");
    spr.print(GyroZ,1);

    spr.setCursor(5,150);
    spr.setTextColor(TFT_YELLOW);
    spr.print("AccAngleX:");
    spr.print(AccAngleX,1);
    spr.setCursor(5,170);
    spr.print("AccAngleY:");
    spr.print(AccAngleY,1);
  
    spr.setCursor(100,30);
    spr.setTextColor(TFT_ORANGE);
    spr.print("GyroAngleX:");
    spr.print("N/A");
    spr.setCursor(100,50);
    spr.print("GyroAngleY:");
    spr.print("N/A");
    spr.setCursor(100,70);
    spr.print("GyroAngleZ:");
    spr.print("N/A");  

    spr.setCursor(100,90);
    spr.setTextColor(TFT_CYAN);
    spr.print("AngleX:");
    spr.print(AngleX,1);
    spr.setCursor(100,110);
    spr.print("AngleY:");
    spr.print(GyroAngleY,1);
    spr.setCursor(100,130);
    spr.print("AngleZ:");
    spr.print(AngleZ,1);  


    //spr.loadFont(Font1_14);
    spr.setCursor(5,220);
    spr.setTextColor(TFT_WHITE);
    spr.print("SN:");
    spr.print(SNID,HEX);
    spr.print("  ");
    spr.print(HardwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::OTA_Status(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_18);
    spr.setCursor(2,2);
    spr.println("OTA-Status");
    spr.print("Progress: ");
    spr.print(OTA_Progress);
    spr.println("%");
    spr.print("版本号: ");
    spr.print(FirmwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::OTA_End(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_18);
    spr.setCursor(2,2);
    spr.println("OTA-End");
    spr.println("更新完成");
    spr.print("版本号: ");
    spr.print(FirmwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::OTA_Error(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_18);
    spr.setCursor(2,2);
    spr.println("OTA-Error");
    spr.println("更新失败 ");
    spr.print("版本号: ");
    spr.print(FirmwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::WiFi_Connect(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_18);
    spr.setCursor(2,2);
    spr.println("连接WiFi...");
    spr.println("Time out 15s...");
    spr.println("版本号: ");
    spr.print(FirmwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}

void Display::WiFi_Error(){
    spr.createSprite(240,240);
    spr.fillSprite(0x0000);
    spr.setTextDatum(CC_DATUM);
    spr.setColorDepth(8);
    spr.loadFont(Font1_18);
    spr.setCursor(2,2);
    spr.println("连接WiFi失败!");
    spr.println("HostAP : ESP32AP");
    spr.println("版本号: ");
    spr.print(FirmwareVer);
    spr.unloadFont();
    spr.pushSprite(0,0);
    spr.deleteSprite();
}
void Display::MsgWindows(const String& msg,uint16_t win_color,uint16_t text_color,int16_t win_width,int16_t win_height,uint32_t win_time) {
    InfoMessage newMsg = {
       .text = msg,
       .bgColor = win_color,
       .textColor = text_color,
       .win_width = win_width,
       .win_height = win_height,
       .time = win_time,
       .startTime = 0
    };
    msgQueue.push(newMsg);
}

void Display::MsgUpdate(){
    if(!msgQueue.empty() && !isShowingMsg) {
        msgQueue.front().startTime = millis();
        isShowingMsg = true;
    }

    if(isShowingMsg) {
        InfoMessage& currentMsg = msgQueue.front();

        // 超时检查
        if(millis() - currentMsg.startTime > currentMsg.time) {
            msgQueue.pop();
            isShowingMsg = false;
            LCD_Refresh_Screen(); // 清除残留
            return;
        }
        //在spr_win内绘制消息窗口
        spr_win.createSprite(240,240);
        spr_win.fillSprite(TFT_TRANSPARENT);
        // 绘制窗口
        const int width = currentMsg.win_width;
        const int height = currentMsg.win_height;
        const int x = (240 - width)/2;
        const int y = ((240 - height)/2)+20;

        spr_win.fillRoundRect(x, y, width, height, 5, currentMsg.bgColor);
        spr_win.drawRoundRect(x, y, width, height, 5, TFT_WHITE);

        spr_win.setTextDatum(CC_DATUM);
        spr_win.setTextColor(currentMsg.textColor, currentMsg.bgColor);
        spr_win.loadFont(Font1_18);

        // 自动换行处理
        String text = currentMsg.text;
        int lineHeight = 18;
        int maxLines = height / lineHeight;
        int currentY = y + 15;

        while(text.length() > 0 && maxLines-- > 0) {
            int spacePos = text.indexOf(' ', 20); // 每行最多20字符
            if(spacePos == -1) spacePos = text.length();

            String line = text.substring(0, spacePos);
            spr_win.drawString(line, x + width/2, currentY);

            currentY += lineHeight;
            text = text.substring(spacePos+1);
        }

        spr_win.unloadFont();
        // 透明方式推送（关键！）
        spr_win.pushSprite(0,0,TFT_TRANSPARENT);
        spr_win.deleteSprite();
    }
}