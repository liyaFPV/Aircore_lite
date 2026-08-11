#include "sensor.h"

int BME280_temperature_HORS[HORS_BUFFER_SIZE];
int BME280_humidity_HORS[HORS_BUFFER_SIZE];
int BME280_pressure_HORS[HORS_BUFFER_SIZE];
int DS18B20_temperature_HORS[HORS_BUFFER_SIZE];

int BME280_temperature_DAY[DAY_BUFFER_SIZE];
int BME280_humidity_DAY[DAY_BUFFER_SIZE];
int BME280_pressure_DAY[DAY_BUFFER_SIZE];
int DS18B20_temperature_DAY[DAY_BUFFER_SIZE];

int BME280_temperature_MIN[MINUTE_BUFFER_SIZE];
int BME280_humidity_MIN[MINUTE_BUFFER_SIZE];
int BME280_pressure_MIN[MINUTE_BUFFER_SIZE];
int DS18B20_temperature_MIN[MINUTE_BUFFER_SIZE];

int hors_count = 0;
int day_count = 0;
int minute_count = 0;
int hors_records = 0;
int day_records = 0;

Ticker timerHORS;
Ticker timerDAY;
Ticker timerMIN;

void addValue(int *array, int size, int value) {
    memmove(array, array + 1, (size - 1) * sizeof(array[0]));
    array[size - 1] = value;
}

void record_minute_data(){
    addValue(BME280_temperature_MIN, MINUTE_BUFFER_SIZE, get_BME280_temperature());
    addValue(BME280_humidity_MIN, MINUTE_BUFFER_SIZE, get_BME280_humidity());
    addValue(BME280_pressure_MIN, MINUTE_BUFFER_SIZE, get_BME280_pressure());
    addValue(DS18B20_temperature_MIN, MINUTE_BUFFER_SIZE, get_DS18B20_temperature());
    if (minute_count < MINUTE_BUFFER_SIZE) minute_count++;
}

void record_hors_data(){
    addValue(BME280_temperature_HORS, HORS_BUFFER_SIZE, get_BME280_temperature());
    addValue(BME280_humidity_HORS, HORS_BUFFER_SIZE, get_BME280_humidity());
    addValue(BME280_pressure_HORS, HORS_BUFFER_SIZE, get_BME280_pressure());
    addValue(DS18B20_temperature_HORS, HORS_BUFFER_SIZE, get_DS18B20_temperature());
    if (hors_count < HORS_BUFFER_SIZE) hors_count++;
    hors_records++;
}

void record_day_data(){
    addValue(BME280_temperature_DAY, DAY_BUFFER_SIZE, get_BME280_temperature());
    addValue(BME280_humidity_DAY, DAY_BUFFER_SIZE, get_BME280_humidity());
    addValue(BME280_pressure_DAY, DAY_BUFFER_SIZE, get_BME280_pressure());
    addValue(DS18B20_temperature_DAY, DAY_BUFFER_SIZE, get_DS18B20_temperature());
    if (day_count < DAY_BUFFER_SIZE) day_count++;
    day_records++;
}

void sensor_init() {
    timerHORS.attach(HORS_UPDATE_S, record_hors_data);
    timerDAY.attach(DAY_UPDATE_S, record_day_data);
    timerMIN.attach(SENSOR_UPDATE_MS / 1000.0, record_minute_data);
    record_hors_data();
    record_day_data();
    record_minute_data();
}

int get_HORS_count(){
    return hors_count;
}

int get_DAY_count(){
    return day_count;
}

int get_MIN_count(){
    return minute_count;
}

int get_HORS_records(){
    return hors_records;
}

int get_DAY_records(){
    return day_records;
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

int* get_MIN_BME280_temperature(){
    return BME280_temperature_MIN;
}

int* get_MIN_BME280_humidity(){
    return BME280_humidity_MIN;
}

int* get_MIN_BME280_pressure(){
    return BME280_pressure_MIN;
}

int* get_MIN_DS18B20_temperature(){
    return DS18B20_temperature_MIN;
}