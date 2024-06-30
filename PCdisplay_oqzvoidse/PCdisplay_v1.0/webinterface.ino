void build() {
  GP.BUILD_BEGIN();   // начать построение, ширина колонки 350px - GP.BUILD_BEGIN(ширина);
  GP.THEME(GP_DARK);  // из памяти прошивки [доступные темы: GP_LIGHT, GP_DARK]

  GP.TITLE("PCdisplay");
  GP.HR();

  GP.FORM_BEGIN("/save");

  GP_MAKE_BLOCK_TAB(
    "WiFi",
    GP_MAKE_BOX(GP.LABEL("WIFI Name"); GP.TEXT("ssid", "SSID", ssid););
    GP_MAKE_BOX(GP.LABEL("Password"); GP.PASS_EYE("pass", "Password", pass););
  );

  GP_MAKE_BLOCK_TAB(
    "MQTT",
    GP_MAKE_BOX(GP.LABEL("MQTT"); GP.TEXT("host", "MQTT-Server", mqtt_server););
    GP_MAKE_BOX(GP.LABEL("Port"); GP.NUMBER("port", "Port", mqtt_port););
  );

  GP_MAKE_BLOCK_TAB(
    "TOPICS",
    GP.LABEL("Управление с ПК");
    GP.TEXT(String("control"), "Topic", control_topic);
    GP.LABEL("Ручное управление кулерами");
    GP.TEXT(String("manualFan"), "Topic", manualFan_topic);
    GP.LABEL("Информационный канал");
    GP.TEXT(String("info"), "Topic", info_topic);
    GP.LABEL("Вкл/Выкл ПК");
    GP.TEXT(String("button"), "Topic", button_topic);
    GP.LABEL("Работает ли устройство");
    GP.TEXT(String("connected"), "Topic", connected_topic);
    GP.LABEL("Управление кулерами");
    GP.TEXT(String("slider"), "Topic", slider_topic);
  );

  GP.SUBMIT("Сохранить");

  GP.FORM_END();

  GP.FORM_BEGIN("/restart");
  GP.SUBMIT("Перезагрузить");
  GP.FORM_END();

  GP.BUILD_END();
}
