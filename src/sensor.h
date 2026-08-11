#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include "config.h"

#define HORS_BUFFER_SIZE 16
#define DAY_BUFFER_SIZE 16
#define MINUTE_BUFFER_SIZE 30

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

// Количество записанных значений
int get_HORS_count();
int get_DAY_count();

// Счётчики записей (для отслеживания обновлений)
int get_HORS_records();
int get_DAY_records();

// Данные за последние минуты (запись каждые SENSOR_UPDATE_MS)
int* get_MIN_BME280_temperature();
int* get_MIN_BME280_humidity();
int* get_MIN_BME280_pressure();
int* get_MIN_DS18B20_temperature();
int get_MIN_count();