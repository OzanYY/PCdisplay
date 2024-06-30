//******************************** Version 1.0 ********************************//

// ------------------------ НАСТРОЙКИ ----------------------------
// настройки пределов скорости и температуры по умолчанию (на случай отсутствия связи)
byte speedMIN = 10, speedMAX = 90, tempMIN = 30, tempMAX = 70;
byte COLOR_ALGORITM = 0;    // 0 или 1 - разные алгоритмы изменения цвета (строка 222)
byte ERROR_DUTY = 90;       // скорость вентиляторов при потере связи
#define ERROR_BACKLIGHT 1   // 0 - гасить подсветку при потере сигнала, 1 - не гасить
#define MENU_ITEMS 9 // Количество пунктов иеню
// ------------------------ НАСТРОЙКИ ----------------------------

//---------------------- Порты --------------------------------------
#define WIRE_CLOCK 400e3//Разгоняем шину I2C в 4 раза до 400 кГц (только софтварная так может, хардварная 100e3)
#define SDA D4
#define SCL D3
#define BTN_PIN1 D5 // пин 1 кнопки
#define BTN_PIN2 D6 // пин 2 кнопки
#define BTN_PIN3 D7 // пин 3 кнопки
#define FAN_PIN D2 // пин куллеров
#define RELE D1 // пин реле
//---------------------- Порты --------------------------------------

//---------------------- Библиотеки ---------------------------------
#include <ESP8266WiFi.h> // Библиотека работы с esp
#include <PubSubClient.h>// Библиотека для работы с mqtt 
#include <GyverPortal.h> // Библиотека для создание web интерфейса
#include <GyverButton.h> // Библиотека для работы с кнопками
#include <ESP_EEPROM.h>  // Библиотека для работы с Flash памятью Esp
#include <LiquidCrystal_I2C.h> // Библиотека для работы с жк дисплеем
#include <Wire.h>
//---------------------- Библиотеки ---------------------------------

//---------------------- Подключаемые файлы -------------------------
#define printByte(args) write(args);
// значок градуса!!!! lcd.write(223);
// правый край полосы загрузки
byte right_empty[] = { B11111, B00001, B00001, B00001, B00001, B00001, B00001, B11111 };
// левый край полосы загрузки
byte left_empty[] = { B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111 };
// центр полосы загрузки
byte center_empty[] = { B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111 };
// блоки для построения графиков
byte bar2[] = { B11111, B11000, B11000, B11000, B11000, B11000, B11000, B11111 };
byte bar3[] = { B11111,  B11100,  B11100,  B11100,  B11100,  B11100,  B11100,  B11111 };
byte bar4[] = { B11111,  B11110,  B11110,  B11110,  B11110,  B11110,  B11110,  B11111 };
byte rightbar1[] = { B11111,  B10001,  B10001,  B10001,  B10001,  B10001,  B10001,  B11111 };
//byte rightbar2[] = {  B11111,  B11001,  B11001,  B11001,  B11001,  B11001,  B11001,  B11111}; //жертва оптимизации
byte rightbar3[] = { B11111,  B11101,  B11101,  B11101,  B11101,  B11101,  B11101,  B11111 };
//---------------------- Подключаемые файлы -------------------------

//---------------------- Конфигурация библиотек ---------------------
GyverPortal ui;
GButton btn1(BTN_PIN1, HIGH_PULL);
GButton btn2(BTN_PIN2, HIGH_PULL);
GButton btn3(BTN_PIN3, HIGH_PULL);
WiFiClient wclient;
PubSubClient mqtt(wclient);
LiquidCrystal_I2C lcd(0x27, 20, 4);
//---------------------- Конфигурация библиотек ---------------------

//---------------------- Глобальные переменные ----------------------
char ssid[32] = "";  // название точки достпа
char pass[32] = "";  // Пароль от точки доступа
char mqtt_server[64] = ""; // адрес сервера MQTT
uint16_t mqtt_port; // Порт для подключения к серверу MQTT
char button_topic[64] = "";
char info_topic[64] = "";
char connected_topic[64] = "";
char slider_topic[64] = "";
char manualFan_topic[64] = "";
char control_topic[64] = "";
int PCdata[20] = {0}; // Массив распарсеных данных
int displayMode = 0, wasDisplayMode = 0;
int duty;
int mainTemp;
uint8_t data[MENU_ITEMS]; // Массив для параметров меню
bool menuFlag = true, inMenu = false, started = false;    // Флаг выбора меню
uint8_t menuMode = 0;
static int8_t pointer = 0; // Переменная указатель
bool changeMenu = true, changePointer = true, changeParam = true;
unsigned long timeout = 0, uptime_timer = 0;
unsigned long sec, mins, hrs;
String perc;
byte lines[] = { 4, 5, 7, 6 };
bool backlight = true, ambientLight = true, controlFromPC = true, manualFan = false, manualColor = false;
bool saveFlag = false, timeOutLCDClear = false, restoreConnectToPC = false, timeOut_flag = false, reDraw = true;
//---------------------- Глобальные переменные ----------------------

void setup() {
  startup();
  lcd.clear();
}

void loop() {
  delay(1);
  ui.tick();
  parsing();
  dutyCalculate(); // посчитать скважность для вентиляторов
  analogWrite(FAN_PIN, map(duty, 0, 100, 1, 255));
  buttonsTick();
  updateDisplay();
  mqttTick();
  timeOutTick();
  if (!started) started = true;
}
