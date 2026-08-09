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
#include "menu.h"

LCD_1602_RUS lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);
Alash_DS1302 rtc = Alash_DS1302(RTC_CLK, RTC_DAT, RTC_RST);

void lcd_init() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.clear();
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
  }
  delay(5000);
  lcd.clear();
  menu_init();
}

void loop() {
  wifi_tick();
  webui_tick();
  draw_menu();
}