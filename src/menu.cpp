#include "menu.h"

extern LCD_1602_RUS lcd;
extern Alash_DS1302 rtc;

int menu_index = 0;
const int menu_count = 2;
int data[20];

/*Системные функции*/

#define GRAPH_WIDTH 16

void getMinMax(int *array, uint16_t size, int &minValue, int &maxValue) {
    minValue = array[0];
    maxValue = array[0];

    for (uint16_t i = 1; i < size; i++) {
        if (array[i] < minValue)
            minValue = array[i];

        if (array[i] > maxValue)
            maxValue = array[i];
    }
}

void initPlot() {
    byte row8[8] = {255,255,255,255,255,255,255,255};
    byte row7[8] = {0,255,255,255,255,255,255,255};
    byte row6[8] = {0,0,255,255,255,255,255,255};
    byte row5[8] = {0,0,0,255,255,255,255,255};
    byte row4[8] = {0,0,0,0,255,255,255,255};
    byte row3[8] = {0,0,0,0,0,255,255,255};
    byte row2[8] = {0,0,0,0,0,0,255,255};
    byte row1[8] = {0,0,0,0,0,0,0,255};

    lcd.createChar(0, row8);
    lcd.createChar(1, row1);
    lcd.createChar(2, row2);
    lcd.createChar(3, row3);
    lcd.createChar(4, row4);
    lcd.createChar(5, row5);
    lcd.createChar(6, row6);
    lcd.createChar(7, row7);
}


void drawPlotLast(int *array, int arraySize) {
    int start = arraySize - GRAPH_WIDTH;
    for (int x = 0; x < GRAPH_WIDTH; x++) {
        int value = constrain(array[start + x], 0, 100);
        int level = map(value, 0, 100, 0, 32);
        for (byte y = 0; y < 4; y++) {
            int part = level - y * 8;
            lcd.setCursor(x, 3 - y);
            if (part >= 8) {
                lcd.write(0);
            }
            else if (part > 0) {
                lcd.write(part);
            }
            else {
                lcd.write(16);
            }
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
    drawPlotLast(data, 20);

    int minValue, maxValue;
    getMinMax(data, 20, minValue, maxValue);

    lcd.setCursor(17, 0);
    lcd.print(String(maxValue));
    lcd.setCursor(17, 1);
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
            menu_test();
            break;
    }
}

void draw_menu() {
    static uint32_t lastLcd = 0;
    if (millis() - lastLcd > DISPLAY_UPDATE_MS){
        lastLcd = millis();
        draw_menu_by_index(menu_index);
    }
}