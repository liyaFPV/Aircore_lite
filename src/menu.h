#pragma once

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


void menu_init();
void draw_menu_by_index(int index);
void draw_menu();
void error(const char* msg);