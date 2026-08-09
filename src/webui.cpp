#include "webui.h"
#include "time.h"

GyverPortal ui;

const char *temp_names[] = {"Температура","Улица"};
const char *arr_names[] = {"Влажность","Давление"};

GPtime valTime;
GPdate valDate;

void build() {
    GP.BUILD_BEGIN(GP_DARK);

    GP.NAV_TABS_LINKS("/,/settings","Главня,Настройки");

    if(ui.uri("/")) {

        GP.AJAX_PLOT_DARK("plot_temp",temp_names,2,100,SENSOR_UPDATE_MS);
        GP.AJAX_PLOT_DARK("plot_arr",arr_names,2,100,SENSOR_UPDATE_MS);

    } else if(ui.uri("/settings")) {

        GP.TITLE("Настройки");

        GP.UPDATE("online");
        GP.LED("online",wifi_connected());

        if (wifi_is_ap()) {
            GP.LABEL("Режим точки доступа. Подключитесь к сети " + wifi_get_ssid() + " (без пароля) и введите данные вашего WiFi");
        }

        GP.FORM_BEGIN("/wifi");
        GP.TEXT("ssid","SSID",wifi_get_ssid());
        GP.PASS("pass","Password");
        GP.SUBMIT("Сохранить WiFi");
        GP.FORM_END();

        GP.BREAK();

        GP.FORM_BEGIN("/resetwifi");
        GP.SUBMIT("Забыть сеть");
        GP.FORM_END();

        GP.BREAK();

        GP.UPDATE("rtcnow");
        GP.LABEL("Сейчас в RTC: " + rtc_now_str(), "rtcnow");

        GP.FORM_BEGIN("/time_sync");
        GP.SUBMIT("Синхронизировать время с устройства");
        GP.FORM_END();
        GP.LABEL("Время берётся с вашего телефона/компьютера (часовой пояс клиента)");

        GP.BREAK();

        GP.FORM_BEGIN("/time");
        GP.DATE("date",valDate);
        GP.TIME("time",valTime);
        GP.SUBMIT("Сохранить время");
        GP.FORM_END();

    }

    GP.BUILD_END();
}


void action() {

    if(ui.update("plot_temp")) {
        int ds=get_DS18B20_temperature();
        if(ds==-999) ds=0;

        int d[]={get_BME280_temperature(),ds};
        ui.answer(d,2);
    }


    if(ui.update("plot_arr")) {
        int d[]={get_BME280_humidity(),get_BME280_pressure()};
        ui.answer(d,2);
    }


    if(ui.update("online"))
        ui.answer(wifi_connected());


    if(ui.update("rtcnow"))
        ui.answer(rtc_now_str());


    if(ui.form("/wifi")) {
        String ssid = ui.getString("ssid");
        String pass = ui.getString("pass");
        ssid.trim();
        if (ssid.length() > 0) {
            wifi_save_config(ssid.c_str(), pass.c_str());
        }
    }


    if(ui.form("/resetwifi")) {
        wifi_reset_config();
    }


    if(ui.form("/time_sync")) {
        if(ui.timeSynced()) {
            bool ok = rtc_set_from_unix(ui.getUnix(), ui.getGMT());
            ui.answer(ok ? "Время сохранено в RTC" : "Ошибка записи в RTC");
        } else {
            ui.answer("Время не синхронизировано, попробуйте ещё раз");
        }
    }


    if(ui.form("/time")) {
        GPtime t;
        GPdate d;
        ui.copyTime("time",t);
        ui.copyDate("date",d);

        if(rtc_set_manual(t.hour, t.minute, t.second,
                          d.day, d.month, d.year)) {
            ui.answer("Время сохранено в RTC");
        } else {
            ui.answer("Ошибка записи в RTC");
        }
    }
}


void webui_begin(){
    ui.attachBuild(build);
    ui.attach(action);
    ui.start();
}


void webui_tick(){
    ui.tick();
}