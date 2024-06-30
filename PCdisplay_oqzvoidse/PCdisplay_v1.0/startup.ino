void startup() {
  uptime_timer = millis();
  Serial.begin(9600);  // запускаем сериал для отладки
  Serial.setTimeout(5);
  Wire.setClock(WIRE_CLOCK);  // Разгоняем шину I2C
  Wire.begin(SDA, SCL);
  analogWriteFreq(40000); //Может понизить производительность
  pinMode(RELE, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(RELE, 1);
  btn1.setTickMode(AUTO);  // Устанавливаем автоматический опрос кнопки
  btn2.setTickMode(AUTO);  // Устанавливаем автоматический опрос кнопки
  btn3.setTickMode(AUTO);  // Устанавливаем автоматический опрос кнопки
  EEPROM.begin(530);       // выделяем Flash память под  ssid и pass
  lcd.init();              // Инициализируем дисплей
  lcd.backlight();         // Включаем подсветку дисплея
  ui.attachBuild(build);   // подключаем веб-интерфейс
  ui.attach(action);       // Прикрепляем к интрфейсу действия

  /*EEPROM.get(0, ssid);          // "Вспоминаем" логин
  EEPROM.get(32, pass);         // "Вспоминаем" пароль
  EEPROM.get(64, mqtt_server);  // "Вспоминаем" сервер
  EEPROM.get(128, mqtt_port);   // "Вспоминаем" порт*/

  EEPROM.get(0, ssid);
  EEPROM.get(32, pass);
  EEPROM.get(64, mqtt_server);
  EEPROM.get(128, button_topic);
  EEPROM.get(192, info_topic);
  EEPROM.get(256, connected_topic);
  EEPROM.get(320, slider_topic);
  EEPROM.get(384, manualFan_topic);
  EEPROM.get(448, control_topic);
  EEPROM.get(512, mqtt_port);

  delay(100);   // на 0.1 секунды
  lcd.clear();  // очистить дисплей
  // Задаем настройки по умолчанию
  PCdata[8] = speedMAX;
  PCdata[9] = speedMIN;
  PCdata[10] = tempMAX;
  PCdata[11] = tempMIN;

  unsigned long timer = millis();  // Создаем таймер
  lcd.print("      Launch...");

  WiFi.mode(WIFI_STA);     // Переводим esp в режим подключения
  WiFi.begin(ssid, pass);  // Подключаемся к wifi

  initBar3();
  delay(2000);

  // юзер не кликнул, пытаемся подключиться к точке
  lcd.setCursor(0, 0);
  lcd.print(" Connecting to WiFi");

  timer = millis();                      // Обновляем таймер
  while (WiFi.status() != WL_CONNECTED)  //если соединения нет
  {
    delay(1);  // Техническая задержка
    // если вышел таймаут
    if (timer + 15000 < millis() || btn2.isHolded()) {
ap:
      WiFi.disconnect();  // отключаемся

      lcd.clear();
      lcd.print(" Failed connection");
      lcd.setCursor(0, 1);
      lcd.print("Create AP: PCDisplay");

      WiFi.mode(WIFI_AP);        // Переключем esp на точку доступа
      WiFi.softAP("PCDisplay");  // Даем название точке доступа

      ui.start();  // Запускаем веб-интерфейс

      while (ui.tick()) {
        delay(1);
        if (saveFlag || btn2.isHolded())  // Пока веб-интрфейс запущен отслеживаем нажатие кнопки сохранить
        {
          ui.stop();
          lcd.clear();
          lcd.print("      Saving...");
          delay(2000);
          ESP.reset();  // Перезагружаем esp
        }
        if (btn2.isDouble())
          goto start;
      }
    }
  }
start:
  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("   WiFi connected");
    lcd.setCursor(0, 1);
    lcd.print(" IP: ");
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 2);
  } else {
    lcd.print("  WiFi disconnected");
    lcd.setCursor(0, 1);
  }

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);

  if (mqtt.connect(id.c_str())) {
    lcd.print("   MQTT connected");
  } else {
    if (WiFi.status() == WL_CONNECTED)
      lcd.setCursor(0, 2);
    lcd.print("    MQTT failed");
  }

  ui.start();
  connectMQTT();

  timer = millis();  // Обновляем таймер
  //while(millis() < timer + 2000) {}
  delay(2000);

  timeout = millis();
}
