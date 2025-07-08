void Display::Page2() {
    if (!spr.created()) { spr.createSprite(240, 135); }
    spr.fillSprite(TFT_BLACK);
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
    auto [vMin, vMax] = safeExtremes(VoltageData, VNUM_POINTS);
    auto [cMin, cMax] = safeExtremes(CurrentData, VNUM_POINTS);

    // 电压量程调整（保证最小量程为1.0）
    if (vMax <= vMin) {
        voltageMax = vMax + 0.5f;
        voltageMin = vMax - 0.5f;
    } else {
        voltageMax = vMax + (vMax - vMin) * 0.1f;
        voltageMin = vMin - (vMax - vMin) * 0.1f;
    }
    voltageMin = std::max(voltageMin, 0.0f); // 电压不低于0
    if (voltageMax - voltageMin < 1.0f) voltageMax = voltageMin + 1.0f;

    // 电流量程调整
    if (cMax <= cMin) {
        currentMax = cMax + 0.5f;
        currentMin = cMax - 0.5f;
    } else {
        currentMax = cMax + (cMax - cMin) * 0.1f;
        currentMin = cMin - (cMax - cMin) * 0.1f;
    }
    currentMin = std::max(currentMin, 0.0f);
    if (currentMax - currentMin < 0.1f) currentMax = currentMin + 0.1f;

    // 安全映射函数
    auto safeMap = [](float value, float inMin, float inMax, int outMin, int outMax) {
        if (inMin >= inMax) return (outMin + outMax) / 2; // 防除零保护
        return (int)((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
    };

    // 绘制区域参数
    const int graphX = 30;
    const int graphWidth = 210;
    const int voltY = 10;
    const int currY = 80;
    const int graphHeight = 60;

    // 绘制网格系统
    spr.loadFont(Font1_12);
    spr.setTextColor(TFT_WHITE);

    // 电压网格（水平）
    for (int i = 0; i <= 5; i++) {
        float val = voltageMax - (voltageMax - voltageMin) * i / 5;
        int y = safeMap(val, voltageMin, voltageMax, voltY + graphHeight, voltY);
        spr.drawLine(graphX, y, graphX + graphWidth, y, 0x7BCF); // 浅灰色网格
        spr.drawString(String(val/10,1), graphX-25, y-6); // 左侧标注
    }

    // 电流网格（水平）
    for (int i = 0; i <= 5; i++) {
        float val = currentMax - (currentMax - currentMin) * i / 5;
        int y = safeMap(val, currentMin, currentMax, currY + graphHeight, currY);
        spr.drawLine(graphX, y, graphX + graphWidth, y, 0x7BCF);
        spr.drawString(String(val/10,1), graphX-25, y-6);
    }

    // 垂直网格（时间轴）
    for (int x = graphX; x <= graphX + graphWidth; x += 50) {
        spr.drawLine(x, voltY, x, voltY + graphHeight, 0x7BCF);
        spr.drawLine(x, currY, x, currY + graphHeight, 0x7BCF);
    }

    // 绘制电压曲线（抗锯齿效果）
    for (int i = 1; i < VNUM_POINTS; i++) {
        int x1 = safeMap(i-1, 0, VNUM_POINTS-1, graphX, graphX+graphWidth);
        int y1 = safeMap(VoltageData[i-1], voltageMin, voltageMax, voltY+graphHeight, voltY);
        int x2 = safeMap(i, 0, VNUM_POINTS-1, graphX, graphX+graphWidth);
        int y2 = safeMap(VoltageData[i], voltageMin, voltageMax, voltY+graphHeight, voltY);
        
        // 使用抗锯齿线段
        spr.drawLine(x1, y1, x2, y2, TFT_GREEN);
        if(abs(y2 - y1) > 2) { // 增强高梯度区域的可见性
            spr.drawLine(x1, y1+1, x2, y2+1, TFT_GREEN);
        }
    }

    // 绘制电流曲线（渐变效果）
    uint16_t color = TFT_YELLOW;
    for (int i = 1; i < VNUM_POINTS; i++) {
        int x1 = safeMap(i-1, 0, VNUM_POINTS-1, graphX, graphX+graphWidth);
        int y1 = safeMap(CurrentData[i-1], currentMin, currentMax, currY+graphHeight, currY);
        int x2 = safeMap(i, 0, VNUM_POINTS-1, graphX, graphX+graphWidth);
        int y2 = safeMap(CurrentData[i], currentMin, currentMax, currY+graphHeight, currY);
        
        // 根据斜率动态调整颜色深度
        float slope = abs(y2 - y1);
        uint16_t lineColor = (slope > 5) ? color : spr.color565(255,255,0); // 高变化用深黄色
        spr.drawLine(x1, y1, x2, y2, lineColor);
    }

    // 绘制量程指示器
    spr.fillRect(graphX, voltY-10, 40, 10, TFT_BLACK);
    spr.drawString("V-Range:", graphX, voltY-10);
    spr.drawString(String(voltageMin/10,1)+"-"+String(voltageMax/10,1), graphX, voltY-10);

    spr.fillRect(graphX, currY+graphHeight+2, 40, 10, TFT_BLACK);
    spr.drawString("A-Range:", graphX, currY+graphHeight+2);
    spr.drawString(String(currentMin/10,1)+"-"+String(currentMax/10,1), graphX, currY+graphHeight+2);

    spr.unloadFont();
    spr.pushSprite(0, 0);
}