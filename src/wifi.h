#pragma once

#include <Arduino.h>

void wifi_init();
void wifi_tick();
bool wifi_connected();
bool wifi_is_ap();
String wifi_get_ip();
String wifi_get_ssid();
void wifi_save_config(const char* ssid, const char* pass);
void wifi_reset_config();
