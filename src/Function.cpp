/*
*File Name:Function.cpp
运行函数

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
*Associated Files:"Function.h"

*Explanations:
    修改了MPU6050_tockn库(../lib/MPU6050_tockn/src/MPU6060_tockn.cpp/h)
*/

#include "Function.h"
#include "Config.h"
#include "Library.h"
#include "Display.h"
#include "Variable.h"
#include "OTA_HTML.h"

Function FUNC;
extern Display DISP;
INA226 ina(Wire);
MPU6050 mpu(Wire);
PD_UFP_Log_c PD_UFP(PD_LOG_LEVEL_VERBOSE);

Button2 button1;
Button2 button2;
Button2 button3;
Button2 button4;

long connecttimeout;
const char* host = "esp32";
  WebServer server(80);
  bool OTA_Status =0;
WiFiManager wm;
struct config_type
{
  char ssid[32];//配网WiFi的SSID
  char psw[64];//配网WiFi的Password
};
config_type wificonf ={{""},{""}};


void Key1LongClick(Button2&btn1){
    unsigned int time1 = btn1.wasPressedFor();
    unsigned int time1max = 1000;
    if (time1 <= 300)
    {
        if (Now_App !=6)
        {
            /**/
        }
        else if (Now_App ==6)
        {
            Menu_Key +=1;
        }else if (Now_App ==7)
        {
            LCD_Light +=1;
        }
        
        
    }
}
void Key2LongClick(Button2&btn2){
    unsigned int time2 = btn2.wasPressedFor();
    unsigned int time2max = 1000;
    if (time2 <= 300)
    {
        if (Now_App !=6)
        {
            if (Now_App ==1)
            {
                DISP.MsgWindows("曲线",0xAD55,TFT_WHITE,100,30,300);
                Now_App =2;
            }else if (Now_App ==2)
            {
                DISP.MsgWindows("主界面",0xAD55,TFT_WHITE,100,30,300);
                Now_App =1;
            }
        }else if (Now_App ==6)
        {
            Menu_Key -=1;
        }else if (Now_App ==7)
        {
            LCD_Light -=1;
        }    
    }
}
void Key3LongClick(Button2&btn3){
    unsigned int time3 = btn3.wasPressedFor();
    unsigned int time3max = 1000;
    if (time3 >= time3max)
    {
        Now_App =1;
        Menu_Key =0;
    }
    
    if (time3 <= 300)
    {
        if (Now_App ==6)
        {
            Now_App = LastApp;
        }
        
    }
}
void Key4LongClick(Button2&btn4){
    unsigned int time4 = btn4.wasPressedFor();
    unsigned int time4max = 1000;
    if (time4 <= 300)
    {
        if (Now_App ==1)
        {
            Now_App =6;
        }
        if (Now_App ==6)
        {
            if (Menu_Key ==1)
            {
                Now_App =5;//SysInfo
                LastApp =6;
                delay(20);
            }else if (Menu_Key ==2)
            {
                Now_App =7;//LCD Light
                LastApp =6;
                delay(20);
            }else if (Menu_Key ==3)
            {
                Now_App =8;//MPU Info
                LastApp =6;
                delay(20);
            }else if (Menu_Key ==4)
            {
                Now_App =9;//OTA
                LastApp =6;
                DISP.WiFi_Connect();
                delay(20);
            }else if (Menu_Key ==5)
            {
                Now_App =3;//PDO Log
                LastApp =6;
            } 
        }
    }
    if (time4 >= time4max)
    {
        Now_App =1;
    }

}
void Function::System_Init(){
    SNID = ESP.getEfuseMac();
    Free_Flash_Size = ESP.getFreeSketchSpace()/1024;//剩余闪存大小KB
    Sketch_Size = ESP.getSketchSize()/1024;//程序大小KB
    button1.begin(SW1);
    button2.begin(SW2);
    button3.begin(SW3);
    button4.begin(SW4);
    button1.setLongClickHandler(Key1LongClick);
    button2.setLongClickHandler(Key2LongClick);
    button3.setLongClickHandler(Key3LongClick);
    button4.setLongClickHandler(Key4LongClick);
    // Now_App = EEPROM.read(App_addr);
    Now_App =1;
    // esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  //WiFi电源优化管理
    Serial.print("\033[32m");//绿色
    Serial.println("===== ***ESP32-S3 USB METER*** =====");
    Serial.print("\033[33m");//黄色
    Serial.println("System Loading...");
    Serial.print("Firmware Version: ");
    Serial.println(FirmwareVer);
    Serial.print("Hardware Version: ");
    Serial.println(HardwareVer);
    Serial.print("SN:");
    Serial.println(SNID,HEX);
    Serial.print("Sketch MD5:");
    Serial.println(ESP.getSketchMD5());   
    Serial.print("Core Version:");
    Serial.println(ESP.getChipRevision()); 
    Serial.print("\033[32m");//绿色
    Serial.println("======== ***SYSTEM PASS*** ========");
    Serial.println("\033[0m");//清除
}

void Function::System_Run(){
    if (OTA_Status ==1)
    {
        server.handleClient();
    }
    CPU_Temperature = temperatureRead();
    button1.loop();
    button2.loop();
    button3.loop();
    button4.loop();
    // DISP.MsgUpdate();
}

void Function::GPIO_Init(){
    analogReadResolution(12);
    analogSetClockDiv(1);
    analogSetAttenuation(ADC_11db);

    pinMode(LCD_BL_PIN, OUTPUT);
    pinMode(DP_PIN, INPUT);
    pinMode(DN_PIN, INPUT);
    pinMode(NTC_PIN, INPUT);
    pinMode(SW1, INPUT);
    pinMode(SW2, INPUT);
    pinMode(SW3, INPUT);
    pinMode(SW4, INPUT);
}

void Function::GPIO_Run(){
    int ADC_DP = analogRead(DP_PIN);
    int ADC_DN = analogRead(DN_PIN);
    int ADC_CC1 = analogRead(CC1_PIN);
    int ADC_CC2 = analogRead(CC2_PIN);
    v_DP = (float)ADC_DP * 3.3 /4095.0;
    v_DN = (float)ADC_DN * 3.3 /4095.0;
    v_CC1 = (float)ADC_CC1 * 3.3 /4095.0;
    v_CC2 = (float)ADC_CC2 * 3.3 /4095.0;
}

void Function::INA22x_Init(){
    ina.begin(0x40);
    if (!ina.begin(0x40))
    {
        while (!ina.begin())
        {
            Serial.print("\033[31m");//串口文本红色
            Serial.println("INA22x Init Error!");
            delay(100);
            System_Error ="INA22x Init Error";
            Serial.println("[DEV]Again Init I2C BUS At SDA(36),SCL(35)!");
            Serial.println("\033[0m");//清除串口文本颜色
            Wire.end();//结束I2C总线
            delay(100);
            Wire.begin(36,35);
            ina.begin(0x40);
            DISP.LCD_Light_Update(LCD_Light,0);
            DISP.MsgWindows("Dev INA Mode",TFT_RED,0xfff,200,40);
            System_Error ="NULL";
            break;
        }
    }
    ina.calibrate(Sampling_ohm,10);
    ina.configure(INA226_AVERAGES_64,INA226_BUS_CONV_TIME_332US,INA226_SHUNT_CONV_TIME_332US,INA226_MODE_SHUNT_BUS_CONT);//~20sps 48,896μs
}

void Function::IMU_Init(){
    mpu.begin();
    if (!mpu.begin())
    {
        Serial.print("\033[31m");//串口文本红色
        Serial.println("MPU6050 Init Error!");
        Serial.println("\033[0m");//清除串口文本颜色
        DISP.LCD_Light_Update(100,0);
        DISP.MsgWindows("MPU ERROR",TFT_RED,0xfff,160,40);
        return;
    }
    mpu.setGyroOffsets(-4.51,1.58,-1.2);
}

void Function::INA22x_Run(){
    NowTime = millis();
    ShuntVoltage_mV = ina.readShuntVoltage();
    BusVoltage = ina.readBusVoltage();
    ShuntCurrent = ina.readShuntCurrent();

    LoadVoltage_V = fabs(BusVoltage + (ShuntVoltage_mV/1000));
    LoadCurrent_A = fabs(ina.readShuntCurrent());
    LoadPower_W = fabs(ina.readBusPower());
    // LoadPower_W = fabs(LoadVoltage_V * LoadCurrent_A);

    CurrentDirection = (ShuntCurrent < 0) ? 1 : 0;
    
    mAh = mAh + (LoadCurrent_A * (NowTime - LastTime))/3600;
    // mWh = mWh + (LoadVoltage_V * ((LoadCurrent_A * (NowTime - LastTime))/3600));
    mWh = mWh + (LoadPower_W * ((NowTime - LastTime)/3600));
    Ah = mAh/1000;
    Wh = mWh/1000;
    Capacity = (1 - (Wh / (4.4 * Ah))) * 0.95;
    Energy = mWh;

    LastTime = NowTime;
}

void Function::IMU_Run(){
  if (!mpu.update())
  {
    return; //如果MPU6050没有更新数据，则返回
  }
  mpu.update();
  if (millis() - imutick > 100){

    AccX = mpu.getAccX();
    AccY = mpu.getAccY();
    AccZ = mpu.getAccZ();

    GyroX = mpu.getGyroX();
    GyroY = mpu.getGyroY();
    GyroZ = mpu.getGyroZ();

    AccAngleX = mpu.getAccAngleX();
    AccAngleY = mpu.getAccAngleY();

    //GyroAngleX = mpu.getGyroAngleX();
    //GyroAngleY = mpu.getGyroAngleY();
    //GyroAngleZ = mpu.getGyroAngleZ();

    AngleX = mpu.getAngleX();
    AngleY = mpu.getAngleY();
    AngleZ = mpu.getAngleZ();
    
    imutick = millis();
  }
}

void Function::FUSB_Init(){
    PD_UFP.set_listen_mode(true); //设置监听模式
    // PD_UFP.init(FUSB_INT_PIN, PD_POWER_OPTION_MAX_12V);
    // PD_UFP.init_PPS(FUSB_INT_PIN, PPS_V(8.6),PPS_A(2.1));
    // PD_UFP.init_PPS(FUSB_INT_PIN,PPS_V(PD_POWER_OPTION_MAX_VOLTAGE),PPS_A(PD_POWER_OPTION_MAX_CURRENT),PD_POWER_OPTION_MAX_POWER);
    PD_UFP.Listen_init_PPS(FUSB_INT_PIN, 0, 0);
    // PD_UFP.set_emarker_info(0x05AC, 0x5678, 0x00); //设置E-Marker信息
    Serial.print("FUSB302 PD Sink Init!");
}

void Function::FUSB_Run(){
    // PD_UFP.run();
    PD_UFP.Listen_run();
    static long PDtimeMillis = millis();
    if (millis()-PDtimeMillis >= 1* 200) //200ms
    {
        PDtimeMillis = millis();
        PD_UFP.status_log_readline(buf, sizeof(buf) -1);
        Serial.printf(buf);
    }
    PD_Voltage = PD_UFP.get_voltage();
    PD_Current = PD_UFP.get_current();
    status_power_t status = PD_UFP.get_ps_status();
    if (status == STATUS_POWER_TYP) {
        PD_Ready =1;// 电源就绪 (STATUS_POWER_TYP)
        PD_Option =0;// 当前为固定电压模式 (TYP)
    } else if (status == STATUS_POWER_PPS) {
        PD_Ready =1;// 电源就绪 (STATUS_POWER_PPS)
        PD_Option =1;// 当前为可编程电源模式 (PPS)
    } else if (status == STATUS_POWER_NA)
    {
        PD_Ready =0;// 电源未就绪 (STATUS_POWER_NA)
        PD_Voltage = 0;
        PD_Current = 0;
        // PD_Option =0;// 当前为固定电压模式 (TYP)
    }else{
        PD_Ready =0;// 电源未就绪 (STATUS_POWER_NA)
        PD_Voltage = 0;
        PD_Current = 0;
    }  
    PD_Src_Cap_Count = PD_UFP.get_src_cap_count(); //获取源能力计数
    PD_Position = PD_UFP.get_selected_position(); //获取PD位置
    ccbus_used = PD_UFP.get_cc_pin(); //获取CC线状态，0/NULL 1/CC1 2/CC2
    // PD_UFP.set_power_option(PD_POWER_OPTION_MAX_5V); //set power option
}

void Function::IMU_Attitude(){
    if (mpu.hasError()){return;}
    if (fabs(AccAngleX) > 60 && fabs(AccAngleY) < 60 && AccY > 0.2)
    {
        DISP.LCD_Rotation_Update(0,0);
    }else if (fabs(AccAngleX) < 60 && fabs(AccAngleY) > 60 && AccX < -0.2)
    {
        DISP.LCD_Rotation_Update(1,0);
    }else if (fabs(AccAngleX) > 60 && fabs(AccAngleY) < 60 && AccY < -0.2)
    {
        DISP.LCD_Rotation_Update(2,0);
    }else if (fabs(AccAngleX) < 60 && fabs(AccAngleY) > 60 && AccX > 0.2)
    {
        DISP.LCD_Rotation_Update(3,0);
    }
}

void Function::AdjustINA22xConfig(){
    /*根据电流动态调整精度*/
    // 滞后阈值 ±0.5A，避免振荡 
    // 去抖延迟：至少 500ms 后才允许再次切换       
    static float lastCalibratedCurrent = 0;
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime < 500) return;

    if (LoadCurrent_A < 1.0 && lastCalibratedCurrent != 2) {
        ina.calibrate(Sampling_ohm, 2);
        lastCalibratedCurrent = 2;
        lastChangeTime = millis();
    } else if (LoadCurrent_A >= 1.0 && LoadCurrent_A < 5.0 && lastCalibratedCurrent != 6) {
        ina.calibrate(Sampling_ohm, 6);
        lastCalibratedCurrent = 6;
        lastChangeTime = millis();
    } else if (LoadCurrent_A >= 5.0 && lastCalibratedCurrent != 10) {
        ina.calibrate(Sampling_ohm, 10);
        lastCalibratedCurrent = 10;
        lastChangeTime = millis();
    }
}

void Function::NTCTemperature_Run(){
    NTCv = map(analogRead(NTC_PIN),0,4095,0,3300);
    NTCm = (3300 - NTCv) * 10000 / NTCv;
    NTC_Temperature = (3450 * 298.15)/(3450 + (298.15 * log(NTCm/10000))) - 273.15;
   /*
   *R=10K ohm
    Beta:3450K
    Kelvins:298.15 = 0(*C)
    Kelvins:273.15 = 25(*C)
   */
}

void Function::APP_Run(){
    switch (Now_App)
    {
    case 1:
        DISP.Page1();
        break;
    case 2:
        DISP.Page2();
        break;
    case 3:
        DISP.Page3();
        break;
    case 4:
        DISP.Page4();
        break;
    case 5:
        DISP.Sys_Info();
        break;
    case 6:
        DISP.Menu();
        break;
    case 7:
        DISP.Change_LCD_Light();
        break;
    case 8:
        DISP.MPU_Info();
        break;
    case 9:
        while (OTA_Status ==0)
        {
            WebUpdate();
            DISP.OTA_Status();
            OTA_Status =1;
        }
        break;
    case 10:
        DISP.OTA_End();
        break;
    case 11:
        DISP.OTA_Error();
        break;
    case 12:
        DISP.WiFi_Connect();
        break;
    case 13:
        DISP.WiFi_Error();
        break;
    default:
        break;
    }
}

void Function::SaveWiFiConfig(){
    uint8_t *p = (uint8_t*)(&wificonf);
    for (int i = 0; i < sizeof(wificonf); i++)
    {
      EEPROM.write(i + WiFi_addr, *(p + i)); //在闪存内模拟写入
    }
    delay(10);
    EEPROM.commit();//执行写入ROM
    delay(10);
}

void Function::ReadWiFiConfig(){
    uint8_t *p = (uint8_t*)(&wificonf);
    for (int i = 0; i < sizeof(wificonf); i++)
    {
      *(p + i) = EEPROM.read(i + WiFi_addr);
    }
}

void Function::DeleteWiFiConfig(){
    config_type deletewifi = {{""}, {""}};
    uint8_t *p = (uint8_t*)(&deletewifi);
    for (int i = 0; i < sizeof(deletewifi); i++)
    {
      EEPROM.write(i + WiFi_addr, *(p + i)); //在闪存内模拟写入
    }
    delay(10);
    EEPROM.commit();//执行写入ROM
    delay(10);
}

void Function::WebUpdate(){
    connecttimeout = millis();
    Serial.print("Web OTA");
    ReadWiFiConfig();
    WiFi.begin(wificonf.ssid,wificonf.psw);
    //esp_wifi_set_ps(WIFI_PS_NONE);
    DISP.WiFi_Connect();
    Serial.print("WiFi Mode");
    Serial.println(WiFi.getMode());
    while (WiFi.status() != WL_CONNECTED)//WiFi连接失败，进入配网
    {
        if (millis() - connecttimeout >= 15000)//15s
        {
            DISP.LCD_Refresh_Screen();
            DISP.WiFi_Error();
            std::vector<const char *> menu = {"wifi","restart"};//自定义菜单
            wm.setMenu(menu);

            bool res;
            res = wm.autoConnect("ESP32AP","");//AP Name,Password
            if (!res)
            {
                Serial.println("WiFi连接失败！");
                while(res);
            }else
            {
                Serial.println("WiFi连接成功！");
            }
        } 
    }
    if (WiFi.status() == WL_CONNECTED)//WiFi连接成功
    {
        Serial.print("WiFi名称:");
        Serial.println(WiFi.SSID().c_str());
        Serial.print("WiFi密码:");
        Serial.println(WiFi.psk().c_str());
        strcpy(wificonf.ssid,WiFi.SSID().c_str());
        stpcpy(wificonf.psw,WiFi.psk().c_str());
        SaveWiFiConfig();
        ReadWiFiConfig();
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
    }
    /*配网部分结束*/

    /*OTA服务部分*/
    if (!MDNS.begin(host))//host = 80(http)
    {
        Serial.println("mDNS服务启动失败！");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS服务启动成功！");
    Serial.print("局域网OTA更新网址: ");
    Serial.println("http://esp32.local");
    //网页服务
    server.on("/",HTTP_GET,[](){
        server.sendHeader("Connection","close");
        server.send(200,"text/html",serverIndex);
    });
    //上传固件
    server.on("/update", HTTP_POST,[](){
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    },[](){
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
            Serial.printf("文件名称: %s\n", upload.filename.c_str());
            Serial.printf("当前写入大小(Byte):%u\n",upload.currentSize);
            Serial.print("开始写入...");
            if (!Update.begin(UPDATE_SIZE_UNKNOWN))
            {
                Update.printError(Serial);
            }  
        }else if (upload.status == UPLOAD_FILE_WRITE)
        {
            //写入固件
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            {
                Update.printError(Serial);
            }
            static int LastProgress = 0;//上一次进度
            if (upload.totalSize >0)// 避免除零错误
            {
                OTA_Progress = 100 - ((upload.currentSize *100) / upload.totalSize);//计算当前上传进度
                DISP.OTA_Status();
                if (OTA_Progress != LastProgress) //更新进度
                {
                    LastProgress = OTA_Progress;
                }
            } 
        }else if (upload.status == UPLOAD_FILE_END)
        {
            if (Update.end(true))
            {
                DISP.OTA_End();
                OTA_Progress = 0;
                OTA_Status = 0;
                Serial.printf("更新完成大小(Byte): %u\n", upload.totalSize);
                Serial.print("服务关闭,即将重启");
                delay(5000);//5s重启
            }else
            {
                DISP.OTA_Error();
                Update.printError(Serial);
            }
        }
    });
    server.begin();
}

    String QC2Protocol(float dp, float dn, float voltage) {
        // 定义常量阈值
        const float LOW_THRESHOLD = 0.3f;
        const float MID_THRESHOLD = 1.7f;
        const float HIGH_THRESHOLD = 2.8f;
        const float APPLE_THRESHOLD_MIN = 2.4f;
        const float APPLE_THRESHOLD_MAX = 3.0f;
        const float NO_PROTOCOL_THRESHOLD = 0.21f;
    
        // 检查无协议情况
        if (dn < NO_PROTOCOL_THRESHOLD && dp < NO_PROTOCOL_THRESHOLD  && PD_Ready == 0) {
            return "[N/A]";
        }
    
        // Apple 2.4A 协议 2.7/2.7 DP/DN
        if (dn > APPLE_THRESHOLD_MIN && dn < APPLE_THRESHOLD_MAX &&
            dp > APPLE_THRESHOLD_MIN && dp < APPLE_THRESHOLD_MAX &&
            voltage > 4.8f && voltage < 5.2f && PD_Ready == 0) {
            return "APP2.4";
        }
    
        // QC5V 协议 0/0.6 DP/DN
        if (dn <= LOW_THRESHOLD && 0.38f <= dp && dp < MID_THRESHOLD && 
            voltage >= 4.3f && voltage <= 5.3f && PD_Ready == 0) {
            return "QC5V";
        }
    
        // QC9V 协议 3.3/0.6 DP/DN
        if (0.38f <= dn && dn < MID_THRESHOLD && 
            HIGH_THRESHOLD <= dp && 
            voltage >= 8.7f && voltage <= 9.3f && PD_Ready == 0) {
            return "QC9V";
        }
    
        // QC12V 协议 0.6/0.6 DP/DN
        if (0.38f <= dn && dn < MID_THRESHOLD && 
            0.38f <= dp && dp < MID_THRESHOLD && voltage >= 11.8f && voltage <= 12.2f && PD_Ready == 0) {
            return "QC12V";
        }
        
        //QC3.0 协议 0.6/3.3 DP/DNz
        if (0.38f <= dp && dp < MID_THRESHOLD && 
            HIGH_THRESHOLD <= dn && PD_Ready == 0)
        {
            return "QC3.0";
        }
        
        // QC20V 协议
        if (HIGH_THRESHOLD <= dn && HIGH_THRESHOLD <= dp && 
            voltage >= 19.8f && voltage <= 20.2f && PD_Ready == 0) {
            return "QC20V";
        }
        
        if(PD_Ready ==0){
            return "[N/A]"; // 如果没有PD协议，返回未知协议
        }
        if (PD_Ready == 1 && PD_Option == 0) // PD Sink 固定电压模式
        {
            float PDPower = (PD_Voltage*0.05) * (PD_Current*0.01); // PD功率计算
            return "PD" + String(PDPower,0) + "W"; // 返回PD功率
        }else if (PD_Ready == 1 && PD_Option == 1)
        {
            float PDPower = (PD_Voltage*0.02) * (PD_Current*0.05); // PD功率计算
            return "PD" + String(PDPower,1) + "W"; // 返回PPS功率
        }
        
        // 默认返回未知协议
        return "[N/A]";
    }

void Function::GetChargeProtocol(){
    String GetProtocol = QC2Protocol(v_DP, v_DN, LoadVoltage_V);
    ChargeProtocol = GetProtocol;
    
}


String ReadUART = ""; //0x00 无信息
void Function::UART_Debug(){
    String Uart_Data = "";
    if (Serial.available() >0)//串口收到数据
    {
        while (Serial.available() >0)
        {
            Uart_Data += char(Serial.read());//逐字节写入
            delay(2);//缓存
        }
        //响应信息
        //UART Debug/Setting 01
        //LCD_LIGHT_SET
        if (ReadUART == "LCD_LIGHT_SET")
        {
            int setLight = atoi(Uart_Data.c_str());//String转int
            if (setLight >=1 && setLight <=100)
            {
                LCD_Light = setLight;
                DISP.LCD_Light_Update(LCD_Light, 1);
                Serial.println("成功并保存！");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("LIGHT SET: "+String(LCD_Light),0xAD55,TFT_WHITE,180,30,2000);
            }else{Serial.print("错误的输入！(1-100)，返回！");ReadUART="";
                DISP.MsgWindows("Rejected!",0xAD55,TFT_WHITE,150,30,2000);}
        }
        //LCD_ROTATION_SET
        if (ReadUART == "LCD_ROTATION_SET")
        {
            int setRotation = atoi(Uart_Data.c_str());
            if (setRotation >=0 && setRotation <=4)
            {
                LCD_Rotation = setRotation;
                DISP.LCD_Rotation_Update(LCD_Rotation, 1);
                Serial.println("成功并保存！");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("ROTATION SET: "+String(LCD_Rotation),0xAD55,TFT_WHITE,180,30,2000);          
            }else{Serial.println("错误的输入！(0-3)，返回！");ReadUART="";
                DISP.MsgWindows("Rejected!",0xAD55,TFT_WHITE,150,30,2000);} 
        }
        //SYSTEM_APP_SET
        if (ReadUART == "SYSTEM_APP_SET")
        {
            int setAPP = atoi(Uart_Data.c_str());
            if (setAPP >=1 && setAPP <=11)
            {
                Now_App = setAPP;
                Serial.println("成功！");
                delay(5);
                ReadUART = "";
                // DISP.MsgWindows("APP SET: "+String(Now_App),0xAD55,TFT_WHITE,150,30,2000);
            }else{Serial.println("错误的输入！返回！");ReadUART="";
                DISP.MsgWindows("Rejected!",0xAD55,TFT_WHITE,150,30,2000);}
        }
        //SYSTEM_WIFI_RESET
        if (ReadUART == "SYSTEM_WIFI_RESET")
        {
            String resetWiFi = Uart_Data.c_str();
            if (resetWiFi == "YES")
            {
                DeleteWiFiConfig();
                Serial.println("有效的WiFi配置已删除!");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("WIFI RESET",0xAD55,TFT_WHITE,150,30,2000);
            }else if (resetWiFi == "NO")
            {
                Serial.println("未做出任何更改!");
                DISP.MsgWindows("Rejected!",0xAD55,TFT_WHITE,150,30,2000);
                delay(5);
                ReadUART = "";   
            }else{Serial.println("错误的输入！(YES)/(NO)");ReadUART="";
                DISP.MsgWindows("Rejected!",0xAD55,TFT_WHITE,150,30,2000);}
        }
        //CPU_CLOCK_SET
        if (ReadUART == "SYSTEM_CPU_CLOCK_SET")
        {
            String setClock = Uart_Data.c_str();
            if (setClock == "PS_NONE")
            {
                setCpuFrequencyMhz(240);  //Freq 160MHz ~fps:60
                DISP.LCD_Light_Update(LCD_Light,0); //恢复屏幕亮度
                Serial.println("时钟频率电源：" + String(setClock));
                Serial.println("成功！");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PS_NONE",0xAD55,TFT_WHITE,100,30,2000);
            }else if (setClock == "PS_MIN")
            {
                setCpuFrequencyMhz(160);  //Freq 80MHz ~fps:40
                Serial.println("时钟频率电源：" + String(setClock));
                Serial.println("成功！");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PS_MIN",0xAD55,TFT_WHITE,100,30,2000);
            }else if (setClock == "PS_LOW")
            {
                setCpuFrequencyMhz(40);  //Freq 40MHz ~fps:20
                DISP.LCD_Light_Update(30,0); //降低屏幕亮度
                Serial.println("时钟频率电源：" + String(setClock));
                Serial.println("成功！");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PS_LOW",0xAD55,TFT_WHITE,100,30,2000);
            }else{Serial.println("错误的输入！返回！");ReadUART="";
                DISP.MsgWindows("Rejected",0xAD55,TFT_WHITE,100,30,2000);}
        }
        //
        if (ReadUART == "INA_CONFIG_SET")
        {
            String setINA = Uart_Data.c_str();
            if (setINA == "FAST")
            {
                ina.configure(INA226_AVERAGES_1,INA226_BUS_CONV_TIME_140US,INA226_SHUNT_CONV_TIME_140US,INA226_MODE_SHUNT_BUS_CONT);
                Serial.println("传感器采样设置：" + String(setINA));
                Serial.println("accepted!");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("FAST MODE",0xAD55,TFT_WHITE,150,30,2000);
            }else if (setINA == "MEDIUM")
            {
                ina.configure(INA226_AVERAGES_64,INA226_BUS_CONV_TIME_332US,INA226_SHUNT_CONV_TIME_332US,INA226_MODE_SHUNT_BUS_CONT);
                Serial.println("传感器采样设置： " + String(setINA));
                Serial.println("accepted!");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("MEDIUM MODE",0xAD55,TFT_WHITE,150,30,2000);
            }else if (setINA == "SLOW")
            {
                ina.configure(INA226_AVERAGES_128,INA226_BUS_CONV_TIME_1100US,INA226_SHUNT_CONV_TIME_1100US,INA226_MODE_SHUNT_BUS_CONT);
                Serial.println("传感器采样设置：" + String(setINA));
                Serial.println("accepted!");
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("SLOW MODE",0xAD55,TFT_WHITE,150,30,2000);
            }else{Serial.println("错误的输入！返回！");ReadUART="";
                DISP.MsgWindows("Rejected",0xAD55,TFT_WHITE,100,30,2000);}
        }
        //
        if (ReadUART == "DEV_PD_POWER_SET")
        {
            String setPD = Uart_Data.c_str();
            if (setPD == "FIX5V"){
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_5V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD FIX 5V",0xAD55,TFT_WHITE,150,30,2000);
            }else if (setPD == "FIX9V"){
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_9V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD FIX 9V",0xAD55,TFT_WHITE,150,30,2000);
                
            }else if (setPD == "FIX12V"){
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_12V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD FIX 12V",0xAD55,TFT_WHITE,150,30,2000);
            }else if (setPD == "FIX15V")
            {
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_15V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD FIX 15V",0xAD55,TFT_WHITE,150,30,2000);
            }else if (setPD == "FIX20V")
            {
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_20V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD FIX 20V",0xAD55,TFT_WHITE,150,30,2000);
            }
            else if (setPD.startsWith("PPS") && setPD.indexOf('V') >0 && setPD.indexOf('A') >0)
            {
                int pps_v = setPD.indexOf('V');
                int pps_a = setPD.indexOf('A');
                String pps_voltage = setPD.substring(3,pps_v);
                String pps_current = setPD.substring(pps_v+1,pps_a);
                float voltage = pps_voltage.toFloat();
                float current = pps_current.toFloat();
                PD_UFP.set_PPS(PPS_V(voltage), PPS_A(current));
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";  
                DISP.MsgWindows("PPS:"+pps_voltage+"V"+pps_current+"A",0xAD55,TFT_WHITE,150,30,2000);
            }
            
            else if (setPD == "OFF_PD")
            {
                PD_UFP.set_power_option(PD_POWER_OPTION_MAX_5V);
                Serial.println("PD Power Option Set: " + String(setPD));
                delay(5);
                ReadUART = "";
                DISP.MsgWindows("PD OFF",0xAD55,TFT_WHITE,150,30,2000);
            }
        }

        //
        else
        {
            ReadUART = Uart_Data;
            delay(2);
            if (ReadUART == "LCD_LIGHT_SET")
            {
                Serial.println("请输入亮度调整值(1-100)");
            }else if (ReadUART == "LCD_ROTATION_SET")
            {
                Serial.println("请输入方向调整值(0-3)");
            }else if (ReadUART == "SYSTEM_APP_SET")
            {
                Serial.println("请输入应用代号");
            }else if (ReadUART == "SYSTEM_WIFI_RESET")
            {
                Serial.println("确定吗？(YES or NO)");
            }else if (ReadUART == "SYSTEM_CPU_CLOCK_SET")
            {
                Serial.println("选项: PS_NONE/PS_MIN/PS_LOW");
            }else if (ReadUART == "INA_CONFIG_SET")
            {
                Serial.println("选项: FAST/MEDIUM/SLOW");
            }else if (ReadUART == "DEV_PD_POWER_SET")
            {
                Serial.println("选项: FIX5/FIX9/FIX12/PPS20V3A/OFF_PD");
            }
            
            else
            {
                Serial.println("");
                Serial.println("System Debug/Setting Command");
                Serial.println("LCD_LIGHT_SET       --update lcd light(1-100)");
                Serial.println("LCD_ROTATION_SET    --update lcd rotation(0-3)");
                Serial.println("SYSTEM_APP_SET      --update system apps(1-11)");
                Serial.println("SYSTEM_WIFI_RESET   --reset wifi config(YES or NO)");
                Serial.println("SYSTEM_CPU_CLOCK_SET--set cpu clock power");
                Serial.println("INA_CONFIG_SET      --set ina config");
                Serial.println("DEV_PD_POWER_SET    --set PD power option");
                Serial.println("");
            }
        }
    }
}


extern Function FUNC;