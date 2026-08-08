#pragma once

#include <Arduino.h>

// Инициализация датчиков
void sensor_init();

// BME280
int get_BME280_temperature();
int get_BME280_humidity();
int get_BME280_pressure();

// DS18B20
int get_DS18B20_temperature();