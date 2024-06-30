void action() {
  if (ui.form()) {
    if (ui.form("/save")) {

      ui.copyStr("ssid", ssid);
      ui.copyStr("pass", pass);
      ui.copyStr("host", mqtt_server);
      ui.copyInt("port", mqtt_port);
      ui.copyStr("button", button_topic);
      ui.copyStr("info", info_topic);
      ui.copyStr("connected", connected_topic);
      ui.copyStr("slider", slider_topic);
      ui.copyStr("manualFan", manualFan_topic);
      ui.copyStr("control", control_topic);

      EEPROM.put(0, ssid);   // Запоминаем новый логин
      EEPROM.put(32, pass);  // и пароль
      EEPROM.put(64, mqtt_server);
      EEPROM.put(128, button_topic);
      EEPROM.put(192, info_topic);
      EEPROM.put(256, connected_topic);
      EEPROM.put(320, slider_topic);
      EEPROM.put(384, manualFan_topic);
      EEPROM.put(448, control_topic);
      EEPROM.put(512, mqtt_port);
      EEPROM.commit();

      saveFlag = true;
    }
    if (ui.form("/restart")) {
      ESP.restart();
    }
  }
}
