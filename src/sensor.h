#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include "config.h"

// Инициализация датчиков
void sensor_init();

// BME280
int get_BME280_temperature();
int get_BME280_humidity();
int get_BME280_pressure();

int* get_HORS_BME280_temperature();
int* get_HORS_BME280_humidity();
int* get_HORS_BME280_pressure();

int* get_DAY_BME280_temperature();
int* get_DAY_BME280_humidity(); 
int* get_DAY_BME280_pressure();

// DS18B20
int get_DS18B20_temperature();

int* get_HORS_DS18B20_temperature();

int* get_DAY_DS18B20_temperature();