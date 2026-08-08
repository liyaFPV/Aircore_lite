#define _LCD_TYPE 1
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "meteo.h"
#include "wifi.h"
#include <LiquidCrystal_I2C.h>
#include <LCD_1602_RUS_ALL.h>
#include <Alash_DS1302.h>
#include "webui.h"
#include "sensor.h"

LCD_1602_RUS lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);
Alash_DS1302 rtc = Alash_DS1302(RTC_CLK, RTC_DAT, RTC_RST);

void lcd_init() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.clear();
}

void error(const char* msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ошибка:");
  lcd.setCursor(0, 1);
  lcd.print(msg);
  Serial.println("Ошибка: ");
  Serial.print(msg);
  while (true) {
    delay(1000);
  }
}

void print_load(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aircore Lite");
  lcd.setCursor(0, 1);
  lcd.print("Загрузка...");
  lcd.setCursor(0, 3);
  lcd.print("by LiyaFPV");
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  lcd_init();
  print_load();
  sensor_init();
  wifi_init();
  webui_begin();
  while (!rtc.begin()) {
      error("RTC не найден!");
  }
  if(wifi_connected()){
    getMeteo();
  }
  if(wifi_is_ap()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AP: ");
    lcd.setCursor(4, 0);
    lcd.print(wifi_get_ssid());
    lcd.setCursor(0, 1);
    lcd.print("IP: ");
    lcd.setCursor(4, 1);
    lcd.print(wifi_get_ip());
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP: ");
    lcd.setCursor(4, 0);
    lcd.print(wifi_get_ip());
    delay(5000);
    lcd.clear();
  }
}

void loop() {
  wifi_tick();
  webui_tick();

  static uint32_t lastLcd = 0;
  if (millis() - lastLcd < DISPLAY_UPDATE_MS) return;
  lastLcd = millis();

  uint8_t hh, mm, ss, mday, mon, wday;
  uint16_t yyear;

  if (!rtc.getDateTime(&hh, &mm, &ss, &mday, &mon, &yyear, &wday)) {
    error("Ошибка чтения даты/времени");
    return;
  }

  uint8_t row = wifi_is_ap() ? 2 : 0;

  char buf[21];
  snprintf(buf, sizeof(buf), "%02d:%02d", hh, mm);
  lcd.setCursor(0, row);
  lcd.print(buf);
  snprintf(buf, sizeof(buf), "%02d.%02d.%04d", mday, mon, yyear);
  lcd.setCursor(0, row + 1);
  lcd.print(buf);
}