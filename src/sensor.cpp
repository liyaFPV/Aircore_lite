#include "sensor.h"

void sensor_init() {
  // Инициализация датчиков
  // Например, настройка пинов, инициализация библиотек и т.д.
}

int get_BME280_temperature(){
    return random(-50, 120);
}

int get_BME280_humidity(){
    return random(0, 100);
}

int get_BME280_pressure(){
    return random(225, 825);
}

int get_DS18B20_temperature(){
    return random(-50, 120);
}
