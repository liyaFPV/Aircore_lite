#include "wifi.h"

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <string.h>
#include "config.h"

#define EEPROM_SIZE 512
#define CFG_MAGIC   0xA5
#define CFG_VERSION 1

struct WifiCfg {
    uint8_t magic;
    uint8_t version;
    char ssid[33];
    char pass[65];
    uint8_t crc;
};

static bool apMode = false;
static unsigned long restartAt = 0;

static uint8_t cfg_crc(const WifiCfg& cfg) {
    uint8_t crc = 0;
    const uint8_t* p = (const uint8_t*)&cfg;
    for (size_t i = 0; i < sizeof(cfg) - 1; i++) crc += p[i];
    return crc;
}

static bool wifi_load(WifiCfg& cfg) {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, cfg);
    if (cfg.magic != CFG_MAGIC || cfg.version != CFG_VERSION) return false;
    if (cfg.crc != cfg_crc(cfg)) return false;
    return true;
}

static void wifi_apply() {
    WifiCfg cfg;
    bool have = wifi_load(cfg);

    if (have && strlen(cfg.ssid) > 0) {
        Serial.println("Connecting to WiFi...");
        Serial.print("SSID: ");
        Serial.println(cfg.ssid);

        WiFi.mode(WIFI_STA);
        WiFi.begin(cfg.ssid, cfg.pass);

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED &&
               millis() - start < CONNECT_TIMEOUT)
        {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println();
            Serial.println("WiFi connected");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            apMode = false;
            return;
        }

        Serial.println();
        Serial.println("Connection failed");
    }
    else
    {
        Serial.println("No saved WiFi config");
    }

    Serial.println("Starting Access Point...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    apMode = true;
}

void wifi_init()
{
    wifi_apply();
}

void wifi_tick()
{
    if (restartAt && millis() >= restartAt)
    {
        ESP.restart();
    }
}

static void wifi_schedule_restart()
{
    restartAt = millis() + 1500;
}

void wifi_save_config(const char* ssid, const char* pass)
{
    WifiCfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.magic = CFG_MAGIC;
    cfg.version = CFG_VERSION;
    strncpy(cfg.ssid, ssid, sizeof(cfg.ssid) - 1);
    strncpy(cfg.pass, pass, sizeof(cfg.pass) - 1);
    cfg.crc = cfg_crc(cfg);

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0, cfg);
    EEPROM.commit();

    Serial.println("WiFi config saved");
    wifi_schedule_restart();
}

void wifi_reset_config()
{
    WifiCfg cfg;
    memset(&cfg, 0, sizeof(cfg));

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0, cfg);
    EEPROM.commit();

    Serial.println("WiFi config cleared");
    wifi_schedule_restart();
}

bool wifi_connected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool wifi_is_ap()
{
    return apMode;
}

String wifi_get_ip()
{
    if (wifi_connected())
    {
        return WiFi.localIP().toString();
    }
    else
    {
        return WiFi.softAPIP().toString();
    }
}

String wifi_get_ssid()
{
    if (wifi_connected())
    {
        return WiFi.SSID();
    }
    else
    {
        return AP_SSID;
    }
}
