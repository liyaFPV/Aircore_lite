#pragma once

#include <Arduino.h>

// Кнопка синхронизации: unix (сек) + смещение часового пояса клиента (мин)
bool rtc_set_from_unix(uint32_t unix, int16_t gmt_min);

// Ручная установка. Если year >= 2000 — дата и время, иначе только время
bool rtc_set_manual(uint8_t h, uint8_t m, uint8_t s,
                    uint8_t day, uint8_t mon, uint16_t year);

// Текущее время в RTC строкой "ДД.ММ.ГГГГ ЧЧ:ММ:СС"
String rtc_now_str();
