#include "time.h"

#include <time.h>
#include <Alash_DS1302.h>

extern Alash_DS1302 rtc;

bool rtc_set_from_unix(uint32_t unix, int16_t gmt_min)
{
    time_t t = (time_t)(unix + (int32_t)gmt_min * 60L);
    bool ok = rtc.setEpoch(t);
    if (ok) Serial.println("RTC: time synced from browser");
    return ok;
}

bool rtc_set_manual(uint8_t h, uint8_t m, uint8_t s,
                    uint8_t day, uint8_t mon, uint16_t year)
{
    if (year >= 2000)
    {
        struct tm dt;
        dt.tm_hour = h;
        dt.tm_min = m;
        dt.tm_sec = s;
        dt.tm_mday = day;
        dt.tm_mon = mon - 1;
        dt.tm_year = year - 1900;
        dt.tm_isdst = -1;
        mktime(&dt);

        bool ok = rtc.write(&dt);
        if (ok) Serial.println("RTC: date/time set manually");
        return ok;
    }

    bool ok = rtc.setTime(h, m, s);
    if (ok) Serial.println("RTC: time set manually");
    return ok;
}

String rtc_now_str()
{
    uint8_t hh, mm, ss, mday, mon, wday;
    uint16_t yyear;

    if (!rtc.getDateTime(&hh, &mm, &ss, &mday, &mon, &yyear, &wday))
    {
        return String("read error");
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d.%02d.%04d %02d:%02d:%02d",
             mday, mon, yyear, hh, mm, ss);
    return String(buf);
}
