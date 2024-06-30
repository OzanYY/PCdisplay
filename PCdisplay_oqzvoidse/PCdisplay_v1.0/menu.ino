void menu() {

  /* Кнопки */
  // Первая кнопка
  if (btn1.isClick() or btn1.isHold()) {  // Если кнопку нажали или удерживают
    if (menuFlag) {  // Если флаг установлен
      lcd.setCursor(0, pointer % 4);
      lcd.print(" ");
      pointer = constrain(pointer - 1, 0, MENU_ITEMS - 1);  // Двигаем указатель в пределах дисплея
    } else {                                                // Иначе
      data[pointer]--;                                      // Увеличиваем параметр на 1
      lcd.setCursor(15, pointer % 4);       // Ставим курсор на позицию 14го сивола и номер строки
      lcd.print("      ");
      changeParam = true;
    }
    changePointer = true;
  }

  // Третья кнопка
  if (btn3.isClick() or btn3.isHold()) {
    if (menuFlag) {  // Если флаг установлен
      lcd.setCursor(0, pointer % 4);
      lcd.print(" ");
      pointer = constrain(pointer + 1, 0, MENU_ITEMS - 1);  // Двигаем указатель в пределах дисплея
    } else {                                                // Иначе
      data[pointer]++;                                      // Уменьшаем параметр на 1
      lcd.setCursor(15, pointer % 4);       // Ставим курсор на позицию 14го сивола и номер строки
      lcd.print("      ");
      changeParam = true;
    }
    changePointer = true;
  }

  // Вторая кнопка
  if (btn2.isClick()) {  // Нажатие на ОК - переключение цели параметр/значение параметра
    lcd.setCursor(19, pointer % 4);
    lcd.print(" ");

    switch (pointer) {  // По номеру указателей располагаем вложенные функции (можно вложенные меню)
      case 0:
        controlFromPC = !controlFromPC;
        menuFlag = !menuFlag;  // Инвертируем флаг
        break;
      case 1:
        backlight = !backlight;
        menuFlag = !menuFlag;  // Инвертируем флаг
        break;
      case 2:
        ambientLight = !ambientLight;
        menuFlag = !menuFlag;  // Инвертируем флаг
        break;
      case 3:
        manualFan = !manualFan;
        menuFlag = !menuFlag;  // Инвертируем флаг
        break;
      case 4:
        manualColor = !manualColor;
        menuFlag = !menuFlag;  // Инвертируем флаг
        break;
      case 5: break;
      case 6: break;
      case 7:
        inMenu = false;
        lcd.clear();
        reDraw = true;
        displayMode = wasDisplayMode;
        menuFlag = !menuFlag;
        break;
      case 8:
        ESP.reset(); // Перезагружаем esp
        break;
    }
    menuFlag = !menuFlag;  // Инвертируем флаг
    changeParam = true;
    changePointer = true;
  }

  // Определение страницы
  if (changePointer) {
    if (pointer >= 4 && menuMode == 0) {
      menuMode = 1;
      changeMenu = true;
      changeParam = true;
    }
    if (pointer >= 8 && menuMode == 1) {
      menuMode = 2;
      changeMenu = true;
      changeParam = true;
    }
    else if (pointer < 4 && menuMode == 1) {
      menuMode = 0;
      changeMenu = true;
      changeParam = true;
    }
    else if (pointer < 8 && menuMode == 2) {
      menuMode = 1;
      changeMenu = true;
      changeParam = true;
    }
  }

  if (changeMenu) {
    /* меню */
    lcd.clear();  // Очищаем буфер
    if (pointer < 4) {
      lcd.print("  ControlFromPC:");
      lcd.setCursor(0, 1);
      lcd.print("  Backlight:  ");
      lcd.setCursor(0, 2);
      lcd.print("  AmbientLight:");
      lcd.setCursor(0, 3);
      lcd.print("  ManualFan");
    } else if (pointer < 8) {
      lcd.print("  ManualLColor:");
      lcd.setCursor(0, 1);
      lcd.print("  RGB LedStrip:");
      lcd.setCursor(0, 2);
      lcd.print("  FanSpeed:    ");
      lcd.setCursor(0, 3);
      lcd.print("  Exit");
    }
    else if (pointer < 12) {
      lcd.print("  Restart");
    }
    changeMenu = false;
  }

  if (changeParam) {
    // 1
    lcd.setCursor(17, 0);
    if (controlFromPC && menuMode == 0) {
      lcd.write(255);
    }
    else if (menuMode == 0) {
      lcd.write(219);
    }

    // 2
    lcd.setCursor(17, 1);
    if (backlight && menuMode == 0) {
      lcd.backlight();
      lcd.write(255);
    }
    else if (menuMode == 0) {
      lcd.noBacklight();
      lcd.write(219);
    }

    // 3
    lcd.setCursor(17, 2);
    if (ambientLight && menuMode == 0) {
      lcd.write(255);
    }
    else if (menuMode == 0) {
      lcd.write(219);
    }

    // 4
    lcd.setCursor(17, 3);
    if (manualFan && menuMode == 0) {
      lcd.write(255);
    }
    else if (menuMode == 0) {
      lcd.write(219);
    }

    // 5
    lcd.setCursor(17, 0);
    if (manualColor && menuMode == 1) {
      lcd.write(255);
    }
    else if (menuMode == 1) {
      lcd.write(219);
    }
    // 6

    // 7
    if (menuMode == 1) {
      lcd.setCursor(15, 2);
      if (data[6] > 100) {
        duty = 100;
        data[6] = 100;
      }
      if (data[6] < 2) {
        duty = 1;
        data[6] = 1;
      }
      lcd.print(data[6]);
    }

    // 8

    //9

    changeParam = false;
  }

  if (changePointer) {
    printPointer(pointer);  // Вывод указателя
  }
  changePointer = false;
}

void printPointer(uint8_t pointer) {
  if (menuFlag && pointer == 5) {                   // Если флаг установлен
    lcd.setCursor(0, pointer % 4);  // Указываем на параметр
    lcd.write(126);
  }
  else if (menuFlag) {                   // Если флаг установлен
    lcd.setCursor(0, pointer % 4);  // Указываем на параметр
    lcd.print(">");
  }
  else {                           // Иначе
    lcd.setCursor(19, pointer % 4);  // Указываем на значение параметра
    lcd.print("<");
  }
}
