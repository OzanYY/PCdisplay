void parsing()
{
  if (Serial.available())
  {
    char str[500];
    int amount = Serial.readBytesUntil('E', str, 500);
    str[amount] = '\0';

    int count = 0;
    char* offset = str;
    while (true)
    {
      PCdata[count++] = atoi(offset);
      offset = strchr(offset, ';');
      if (offset) offset++;
      else break;
    }

    timeout = millis();
    timeOut_flag = false;
  }

  if (!controlFromPC) {
    if (manualFan) PCdata[12] = 1;
    else PCdata[12] = 0;

    if (manualColor) PCdata[13] = 1;
    else PCdata[13] = 0;
  }
}

void buttonsTick() {
  if (displayMode == 50) return;
  if (btn3.isClick() && !inMenu)
  {
    displayMode = (displayMode + 1) % 3;
    reDraw = true;
  }
  if (btn1.isClick() && !inMenu)
  {
    displayMode--;
    if (displayMode < 0) displayMode = 2;
    reDraw = true;
  }
  if (btn2.isClick() && !inMenu && started)
  {
    wasDisplayMode = displayMode;
    displayMode = 50;
    changeMenu = true;
    changePointer = true;
    changeParam = true;
    pointer = 0;
    inMenu = true;
  }
}

void dutyCalculate() {
  if (PCdata[12] == 1)  // если стоит галочка ManualFAN
  {
    if (!manualFan) {
      duty = PCdata[14];  // скважность равна установленной ползунком
    }
    else duty = data[6];

  }
  else {                // если нет
    switch (PCdata[18]) {
      case 0:
        mainTemp = PCdata[0];  // взять опорную температуру как CPU
        break;
      case 1:
        mainTemp = PCdata[1];  // взять опорную температуру как GPU
        break;
      case 2:
        mainTemp = max(PCdata[0], PCdata[1]);  // взять опорную температуру как максимум CPU и GPU
        break;
    }
    duty = map(mainTemp, PCdata[11], PCdata[10], PCdata[9], PCdata[8]);
    duty = constrain(duty, PCdata[9], PCdata[8]);
  }
  if (timeOut_flag && controlFromPC) duty = ERROR_DUTY;  // если пропало соединение, поставить вентиляторы на ERROR_DUTY
  data[6] = duty;
}

void updateDisplay() {
  if (reDraw)
  {
    lcd.clear();
    switch (displayMode)
    {
      case 0:
        draw_labels_1();
        break;
      case 1:
        draw_labels_2();
        break;
      case 2:
        drawInfo();
        break;
    }
  }
  reDraw = false;
  
  switch (displayMode)
  {
    case 0:
      draw_stats_1();
      break;
    case 1:
      draw_stats_2();
      break;
    case 2:
      timeOutInfo();
      break;
    case 50:
      menu();
      break;
  }
}

void initBar3() {
  lcd.createChar(0, left_empty);
  lcd.createChar(1, center_empty);
  lcd.createChar(2, right_empty);
  lcd.createChar(3, bar2);
  lcd.createChar(4, bar3);
  lcd.createChar(5, bar4);
  lcd.createChar(6, rightbar1);
  lcd.createChar(7, rightbar3);
}

void fillBar3(byte start_pos, byte row, byte bar_length, byte fill_percent) {
  byte infill = bar_length * fill_percent / 10; // 10 * (0 - 100) / 10 Пример: 10 * 11 / 10 = 11
  byte fract = infill % 10; // 11 % 10 = 1
  infill /= 10; // 11 / 10 = 1

  // Отрисовываем первую клетку
  lcd.setCursor(start_pos, row);
  if (infill == 0) {
    if (fract >= 0 && fract < 2) lcd.write(0);
    else if (fract >= 2 && fract < 4) lcd.write(3);
    else if (fract >= 4 && fract < 6) lcd.write(4);
    else if (fract >= 6 && fract < 8) lcd.write(5);
    else if (fract >= 8) lcd.write(char(255));
  }

  for (int n = 0; n < bar_length - 1; n++) {
    if (n < infill) lcd.write(char(255));
    if (n == infill && n != 0) {
      if (fract >= 0 && fract < 2) lcd.write(1);
      else if (fract >= 2 && fract < 4) lcd.write(0);
      else if (fract >= 4 && fract < 6) lcd.write(3);
      else if (fract >= 6 && fract < 8) lcd.write(4);
      else if (fract >= 8) lcd.write(5);
    }
    if (n > infill) lcd.write(1);
  }
  if (infill == bar_length - 1) {
    if (fract >= 0 && fract < 2) lcd.write(2);
    else if (fract >= 2 && fract < 4) lcd.write(6);
    else if (fract >= 4 && fract < 6) lcd.write(6);
    else if (fract >= 6 && fract < 8) lcd.write(7);
    else if (fract >= 8) lcd.write(char(255));
  }
  else if (infill == bar_length) lcd.write(char(255));
  else lcd.write(2);
}

void draw_stats_1() {
  lcd.setCursor(4, 0); lcd.print(PCdata[0]); lcd.write(223); // CPU-temp
  lcd.setCursor(17, 0); lcd.print(PCdata[4]); // CPU_load
  if (PCdata[4] < 10) perc = "% ";
  else if (PCdata[4] < 100) perc = "%";
  else perc = "";  lcd.print(perc);
  lcd.setCursor(4, 1); lcd.print(PCdata[1]); lcd.write(223); // GPU_temp
  lcd.setCursor(17, 1); lcd.print(PCdata[5]); // GPU_load
  if (PCdata[5] < 10) perc = "% ";
  else if (PCdata[5] < 100) perc = "%";
  else perc = "";  lcd.print(perc);
  lcd.setCursor(17, 2); lcd.print(PCdata[7]); // GPU_mem
  if (PCdata[7] < 10) perc = "% ";
  else if (PCdata[7] < 100) perc = "%";
  else perc = "";  lcd.print(perc);
  lcd.setCursor(17, 3); lcd.print(PCdata[6]); // RAM_use
  if (PCdata[6] < 10) perc = "% ";
  else if (PCdata[6] < 100) perc = "%";
  else perc = "";  lcd.print(perc);

  //fillBar3 принимает аргументы (столбец, строка, длина полосы, значение в % (0 - 100) )
  fillBar3(7, 0, 10, PCdata[4]);
  fillBar3(7, 1, 10, PCdata[5]);
  fillBar3(7, 2, 10, PCdata[7]);
  fillBar3(7, 3, 10, PCdata[6]);

  reDraw = false;
}

void draw_stats_2() {
  lcd.setCursor(16, 0); lcd.print(duty);
  if ((duty) < 10) perc = "%  ";
  else if ((duty) < 100) perc = "% ";
  else perc = "%";  lcd.print(perc);

  lcd.setCursor(4, 2); lcd.print(PCdata[2]); lcd.write(223); // MB_temp
  lcd.setCursor(16, 2); lcd.print(PCdata[3]); lcd.write(223); // HDD_Max_temp

  lcd.setCursor(9, 3);
  sec = (long)(millis() - uptime_timer) / 1000;
  hrs = floor((sec / 3600));
  mins = floor(sec - (hrs * 3600)) / 60;
  sec = sec - (hrs * 3600 + mins * 60);
  if (hrs < 10) lcd.print(0);
  lcd.print(hrs);
  lcd.print(":");
  if (mins < 10) lcd.print(0);
  lcd.print(mins);
  lcd.print(":");
  if (sec < 10) lcd.print(0);
  lcd.print(sec);

  //fillBar3 принимает аргументы (столбец, строка, длина полосы, значение в % (0 - 100) )
  fillBar3(6, 0, 10, duty);

  reDraw = false;
}

void draw_labels_1() {
  lcd.setCursor(0, 0);
  lcd.print("CPU:");
  lcd.setCursor(0, 1);
  lcd.print("GPU:");
  lcd.setCursor(0, 2);
  lcd.print("GPUmem:");
  lcd.setCursor(0, 3);
  lcd.print("RAMuse:");
}

void draw_labels_2() {
  lcd.setCursor(0, 0);
  lcd.print("FANsp:");
  lcd.setCursor(0, 2);
  lcd.print("MB:");
  lcd.setCursor(9, 2);
  lcd.print("HDDmax:");
  lcd.setCursor(0, 3);
  lcd.print("Working:");
}

void drawInfo()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.print("   WiFi connected");
    lcd.setCursor(0, 1);
    lcd.print(" IP: ");
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 2);
  }
  else
  {
    lcd.print("  WiFi disconnected");
    lcd.setCursor(0, 1);
  }
  connectMQTT();
  if (mqtt.connected())
  {
    lcd.print("   MQTT connected");
  }
  else
  {
    if (WiFi.status() == WL_CONNECTED)
      lcd.setCursor(0, 2);
    lcd.print("    MQTT failed");
  }
}

void timeOutTick()
{
  if ((millis() - timeout > 10300))
  {
    if (!timeOut_flag) changeParam = true;
    timeOut_flag = true;
    if (!ERROR_BACKLIGHT) lcd.noBacklight();
  }
  else
  {
    timeOut_flag = false;
  }
}

void timeOutInfo() {
  lcd.setCursor(0, 3);
  if (timeOut_flag)
  {
    lcd.print(" Restore connection");
  }
  else
  {
    lcd.print("                    ");
  }
}
