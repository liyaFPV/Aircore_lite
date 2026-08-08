#include "meteo.h"

#include "config.h"
#include "wifi.h"

static bool getGeoIP(float &lat, float &lon)
{
    BearSSL::WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(8000);

    HTTPClient http;

    if (!http.begin(client, "https://ipwho.is/"))
    {
        Serial.println("GeoIP HTTP begin failed");
        return false;
    }

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf("GeoIP HTTP Error: %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;

    DeserializationError err = deserializeJson(doc, payload);

    if (err)
    {
        Serial.print("GeoIP parse failed: ");
        Serial.println(err.c_str());
        return false;
    }

    lat = doc["latitude"];
    lon = doc["longitude"];

    return (lat != 0.0 || lon != 0.0);
}

static void resolve_coords(float &lat, float &lon)
{
    lat = Latitude;
    lon = Longitude;

    if (wifi_connected() && getGeoIP(lat, lon))
    {
        Serial.println("Coordinates by GeoIP:");
    }
    else
    {
        lat = Latitude;
        lon = Longitude;
        Serial.println("Using config coordinates:");
    }

    Serial.printf("  Lat: %.6f  Lon: %.6f\n", lat, lon);
}

void getMeteo()
{
    float lat, lon;
    resolve_coords(lat, lon);

    BearSSL::WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url =
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=" + String(lat, 6) +
    "&longitude=" + String(lon, 6) +
    "&daily=temperature_2m_min,temperature_2m_mean,temperature_2m_max,"
    "wind_speed_10m_min,wind_speed_10m_mean,wind_speed_10m_max,weather_code" +
    "&wind_speed_unit=ms" +
    "&forecast_days=1" +
    "&timezone=Asia%2FYekaterinburg";

    Serial.println(url);

    if (!http.begin(client, url))
    {
        Serial.println("HTTP begin failed");
        return;
    }

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf("HTTP Error: %d\n", httpCode);
        http.end();
        return;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;

    DeserializationError err = deserializeJson(doc, payload);

    if (err)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(err.c_str());
        return;
    }

    // Температура
    float tempMin  = doc["daily"]["temperature_2m_min"][0];
    float tempMean = doc["daily"]["temperature_2m_mean"][0];
    float tempMax  = doc["daily"]["temperature_2m_max"][0];

    // Ветер
    float windMin  = doc["daily"]["wind_speed_10m_min"][0];
    float windMean = doc["daily"]["wind_speed_10m_mean"][0];
    float windMax  = doc["daily"]["wind_speed_10m_max"][0];

    // Код погоды
    int weatherCode = doc["daily"]["weather_code"][0];

    Serial.println();
    Serial.println("========== Open-Meteo ==========");

    Serial.println("Temperature:");
    Serial.printf("  Min : %.1f C\n", tempMin);
    Serial.printf("  Mean: %.1f C\n", tempMean);
    Serial.printf("  Max : %.1f C\n", tempMax);

    Serial.println();

    Serial.println("Wind:");
    Serial.printf("  Min : %.1f m/s\n", windMin);
    Serial.printf("  Mean: %.1f m/s\n", windMean);
    Serial.printf("  Max : %.1f m/s\n", windMax);

    Serial.println();

    Serial.printf("Weather code: %d\n", weatherCode);

    Serial.println("================================");
}
