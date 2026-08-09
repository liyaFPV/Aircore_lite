#include "menu.h"

extern LCD_1602_RUS lcd;
extern Alash_DS1302 rtc;

int menu_index = 0;
const int menu_count = 1;

/*Системные функции*/
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


/*Системный Api*/
void menu_init() {
    menu_index = 0;
    pinMode(BUTTON_PIN, INPUT_PULLUP);

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
    }
}

void draw_menu() {
    static uint32_t lastLcd = 0;
    if (millis() - lastLcd > DISPLAY_UPDATE_MS){
        lastLcd = millis();
        draw_menu_by_index(menu_index);
    }
}