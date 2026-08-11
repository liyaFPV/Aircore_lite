#include "menu.h"

extern LCD_1602_RUS lcd;
extern Alash_DS1302 rtc;

int menu_index = 0;
const int menu_count = 9;
int data[20];

/*Системные функции*/

#define GRAPH_WIDTH 16

void getMinMax(int *array, uint16_t size, int count, int &minValue, int &maxValue) {
    minValue = 10000;
    maxValue = -10000;

    int start = size - count;
    if (start < 0) start = 0;

    for (uint16_t i = start; i < size; i++) {
        if (array[i] <= -999) continue;

        if (array[i] < minValue)
            minValue = array[i];

        if (array[i] > maxValue)
            maxValue = array[i];
    }

    if (minValue == 10000) {
        minValue = 0;
        maxValue = 0;
    }
}

void initPlot() {
    byte row2[8] = {0,0,0,0,0,0,255,255};
    byte row4[8] = {0,0,0,0,255,255,255,255};
    byte row6[8] = {0,0,255,255,255,255,255,255};
    byte row8[8] = {255,255,255,255,255,255,255,255};

    lcd.createChar(4, row2);
    lcd.createChar(5, row4);
    lcd.createChar(6, row6);
    lcd.createChar(7, row8);
    lcd.ResetAllIndex();
}


void drawPlotLast(int *array, int arraySize, int count) {
    int start = arraySize - GRAPH_WIDTH;
    int valid = min(count, GRAPH_WIDTH);

    int minValue = 10000, maxValue = -10000;
    for (int i = 0; i < valid; i++) {
        int v = array[arraySize - 1 - i];
        if (v <= -999) continue;
        if (v < minValue) minValue = v;
        if (v > maxValue) maxValue = v;
    }
    if (minValue == 10000) {
        minValue = 0;
        maxValue = 1;
    }
    if (maxValue == minValue) maxValue = minValue + 1;

    for (int x = 0; x < GRAPH_WIDTH; x++) {
        int idx = start + x;
        int v = array[idx];

        int level;
        if (idx < arraySize - valid) {
            level = 0;
        }
        else {
            if (v <= -999) v = minValue;
            level = map(constrain(v, minValue, maxValue), minValue, maxValue, 1, 32);
        }

        for (byte y = 0; y < 4; y++) {
            int part = level - y * 8;
            lcd.setCursor(x, 3 - y);
            if (part >= 7)      lcd.write(7);
            else if (part >= 5) lcd.write(6);
            else if (part >= 3) lcd.write(5);
            else if (part >= 1) lcd.write(4);
            else                lcd.write(' ');
        }
    }
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

void IRAM_ATTR button_isr() {
    static uint32_t lastBTN = 0;
    if (millis() - lastBTN > DEBOUNCE_TIME) {
        lastBTN = millis();
        menu_index++;
        if (menu_index >= menu_count) {
            menu_index = 0;
        }
    } 
}

/*Саздание функций меню и страниц*/
void menu_main(){
    uint8_t hh, mm, ss, mday, mon, wday;
    uint16_t yyear;

    if (!rtc.getDateTime(&hh, &mm, &ss, &mday, &mon, &yyear, &wday)) {
        error("Ошибка чтения даты/времени");
        return;
    } 

    char buf[21];
    snprintf(buf, sizeof(buf), "%02d:%02d", hh, mm);
    lcd.setCursor(0, 0);
    lcd.print(buf);
    snprintf(buf, sizeof(buf), "%02d.%02d.%04d", mday, mon, yyear);
    lcd.setCursor(0, 1);
    lcd.print(buf);
}


void menu_test(){
    randomSeed(micros());
    for (int i = 0; i < 20; i++) {
        data[i] = random(0, 101);
    }

    // Выводим массив в Serial
    for (int i = 0; i < 20; i++) {
        Serial.print(data[i]);
        Serial.print(" ");
    }
    drawPlotLast(data, 20, 20);

    int minValue, maxValue;
    getMinMax(data, 20, 20, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("Test");
    lcd.setCursor(16, 1);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 2);
    lcd.print(String(minValue));
}

void menu_hors_home_tmp(){
    initPlot();
    int* buff_data=get_HORS_BME280_temperature();
    int count=get_HORS_count();
    drawPlotLast(buff_data, HORS_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, HORS_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("htmp");
    lcd.setCursor(16, 1);
    lcd.print("Час");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_hors_open_tmp(){
    initPlot();
    int* buff_data=get_HORS_DS18B20_temperature();
    int count=get_HORS_count();
    drawPlotLast(buff_data, HORS_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, HORS_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("Otmp");
    lcd.setCursor(16, 1);
    lcd.print("Час");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_hors_home_hum(){
    initPlot();
    int* buff_data=get_HORS_BME280_humidity();
    int count=get_HORS_count();
    drawPlotLast(buff_data, HORS_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, HORS_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("hhum");
    lcd.setCursor(16, 1);
    lcd.print("Час");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_hors_home_pres(){
    initPlot();
    int* buff_data=get_HORS_BME280_pressure();
    int count=get_HORS_count();
    drawPlotLast(buff_data, HORS_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, HORS_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("hprs");
    lcd.setCursor(16, 1);
    lcd.print("Час");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_day_home_tmp(){
    initPlot();
    int* buff_data=get_DAY_BME280_temperature();
    int count=get_DAY_count();
    drawPlotLast(buff_data, DAY_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, DAY_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("htmp");
    lcd.setCursor(16, 1);
    lcd.print("День");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_day_open_tmp(){
    initPlot();
    int* buff_data=get_DAY_DS18B20_temperature();
    int count=get_DAY_count();
    drawPlotLast(buff_data, DAY_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, DAY_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("Otmp");
    lcd.setCursor(16, 1);
    lcd.print("День");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_day_home_hum(){
    initPlot();
    int* buff_data=get_DAY_BME280_humidity();
    int count=get_DAY_count();
    drawPlotLast(buff_data, DAY_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, DAY_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("hhum");
    lcd.setCursor(16, 1);
    lcd.print("День");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

void menu_day_home_pres(){
    initPlot();
    int* buff_data=get_DAY_BME280_pressure();
    int count=get_DAY_count();
    drawPlotLast(buff_data, DAY_BUFFER_SIZE, count);

    int minValue, maxValue;
    getMinMax(buff_data, DAY_BUFFER_SIZE, count, minValue, maxValue);

    lcd.setCursor(16, 0);
    lcd.print("hprs");
    lcd.setCursor(16, 1);
    lcd.print("День");
    lcd.setCursor(16, 2);
    lcd.print(String(maxValue));
    lcd.setCursor(16, 3);
    lcd.print(String(minValue));
}

/*Системный Api*/
void menu_init() {
    menu_index = 0;
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    initPlot();

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        button_isr,
        FALLING
    );
}

void draw_menu_by_index(int index) {
    lcd.clear();

    switch (index) {
        case 0:
            menu_main();
            break;
        case 1:
            menu_hors_home_tmp();
            break;
        case 2:
            menu_hors_open_tmp();
            break;
        case 3:
            menu_hors_home_hum();
            break;
        case 4:
            menu_hors_home_pres();
            break;
        case 5:
            menu_day_home_tmp();
            break;
        case 6:
            menu_day_open_tmp();
            break;
        case 7:
            menu_day_home_hum();
            break;
        case 8:
            menu_day_home_pres();
            break;
    }
}

int screen_version(int index) {
    if (index == 0) {
        uint8_t hh, mm, ss, mday, mon, wday;
        uint16_t yyear;
        if (rtc.getDateTime(&hh, &mm, &ss, &mday, &mon, &yyear, &wday)) {
            return mm;
        }
        return -1;
    }
    if (index < 5) return get_HORS_records();
    return get_DAY_records();
}

void draw_menu() {
    static uint32_t lastLcd = 0;
    static int lastIndex = -1;
    static int lastVersion = -1;

    if (millis() - lastLcd < DISPLAY_UPDATE_MS) return;
    lastLcd = millis();

    int index = menu_index;
    int version = screen_version(index);

    if (index != lastIndex || version != lastVersion) {
        lastIndex = index;
        lastVersion = version;
        draw_menu_by_index(index);
    }
}