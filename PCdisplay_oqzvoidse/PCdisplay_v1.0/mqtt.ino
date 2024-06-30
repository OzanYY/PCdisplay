String id("PCdisplay-");
void connectMQTT() {
  // задаём случайный ID
  id += String(random(0xffffff), HEX);

  if (mqtt.connect(id.c_str())) {
    mqtt.subscribe(button_topic);
    mqtt.subscribe(slider_topic);
    mqtt.subscribe(manualFan_topic);
    mqtt.subscribe(control_topic);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (String(topic) == button_topic && (char)payload[0] == '1') {
    digitalWrite(RELE, 0);
    delay(1000);
    digitalWrite(RELE, 1);
  }
  if (String(topic) == slider_topic) {
    String messageTemp;
    for (int i = 0; i < length; i++) {
      messageTemp += (char)payload[i];
    }
    int sp = messageTemp.toInt();
    data[6] = sp;
  }
  if (String(topic) == manualFan_topic) {
    if ((char)payload[0] == '1') manualFan = true;
    else manualFan = false;
    changeParam = true;
  }
  if (String(topic) == control_topic) {
    if ((char)payload[0] == '1') controlFromPC = true;
    else controlFromPC = false;
    changeParam = true;
  }
}

unsigned long mqtt_timer = 0;

void mqttTick() {
  if (WiFi.status() != WL_CONNECTED) return;  // wifi не подключен
  if (!mqtt.connected() && mqtt_timer + 10000 < millis()) {
    mqtt_timer = millis();
    connectMQTT();
    reDraw = true;
  } else if (mqtt_timer + 5000 < millis()) {
    mqtt_timer = millis();
    mqtt.publish(connected_topic, "PCDisplay connected");
  }
  mqtt.loop();
}
