#include "sensor.h"

int BME280_temperature_HORS[16];
int BME280_humidity_HORS[16];
int BME280_pressure_HORS[16];
int DS18B20_temperature_HORS[16];

int BME280_temperature_DAY[16];
int BME280_humidity_DAY[16];
int BME280_pressure_DAY[16];
int DS18B20_temperature_DAY[16];

int rec_HORS_old=0;

int rec_DAY_old=0;

Ticker timerHORS;
Ticker timerDAY;

void addValue(int *array, int size, int value, int &index) {
    if (index >= size) {
        memset(array, 0, size * sizeof(array[0]));
        index = 0;
    }

    array[index++] = value;
}

void record_hors_data(){
    addValue(BME280_temperature_HORS, 16, get_BME280_temperature(), rec_HORS_old);
    addValue(BME280_humidity_HORS, 16, get_BME280_humidity(), rec_HORS_old);
    addValue(BME280_pressure_HORS, 16, get_BME280_pressure(), rec_HORS_old);
    addValue(DS18B20_temperature_HORS, 16, get_DS18B20_temperature(), rec_HORS_old);
    rec_HORS_old++;
}

void record_day_data(){
    addValue(BME280_temperature_DAY, 16, get_BME280_temperature(), rec_DAY_old);
    addValue(BME280_humidity_DAY, 16, get_BME280_humidity(), rec_DAY_old);
    addValue(BME280_pressure_DAY, 16, get_BME280_pressure(), rec_DAY_old);
    addValue(DS18B20_temperature_DAY, 16, get_DS18B20_temperature(), rec_DAY_old);
    rec_DAY_old++;
}

void sensor_init() {
    timerHORS.attach(HORS_UPDATE_S, record_hors_data);
    timerDAY.attach(DAY_UPDATE_S, record_day_data);
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


int* get_HORS_BME280_temperature(){
    return BME280_temperature_HORS;
}

int* get_HORS_BME280_humidity(){
    return BME280_humidity_HORS;
}

int* get_HORS_BME280_pressure(){
    return BME280_pressure_HORS;
}

int* get_HORS_DS18B20_temperature(){
    return DS18B20_temperature_HORS;
}


int* get_DAY_BME280_temperature(){
    return BME280_temperature_DAY;
}

int* get_DAY_BME280_humidity(){
    return BME280_humidity_DAY;
}

int* get_DAY_BME280_pressure(){
    return BME280_pressure_DAY;
}

int* get_DAY_DS18B20_temperature(){
    return DS18B20_temperature_DAY;
}