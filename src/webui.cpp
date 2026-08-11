#include "webui.h"
#include "time.h"

GyverPortal ui;

const char *temp_names[] = {"Температура","Улица"};
const char *arr_names[] = {"Влажность","Давление"};

GPtime valTime;
GPdate valDate;

void build_plot_js() {
    GP.SEND(F("<script>\n"
              "function GPlot(id,cfg){\n"
              "var c=document.getElementById(id);if(!c)return{upd:function(){}};\n"
              "var cv=document.createElement('canvas');cv.style.width='100%';cv.style.height='400px';cv.style.display='block';c.appendChild(cv);\n"
              "var x=cv.getContext('2d'),S=cfg.series,N=S.length,step=cfg.step,max=cfg.max;\n"
              "var P=[];for(var i=0;i<N;i++){var a=S[i].data,ar=[];for(var j=0;j<a.length;j++)ar.push(a[j]);P.push(ar);}\n"
              "var cols=[];for(var i=0;i<N;i++)cols.push(S[i].color||['#4fc3f7','#ff8a65','#81c784','#f06292'][i%4]);\n"
              "var names=[];for(var i=0;i<N;i++)names.push(S[i].name||'');\n"
              "var H=400,padL=50,padR=12,padT=24,padB=28;\n"
              "function draw(){\n"
              "var W=cv.clientWidth||600;if(cv.width!=W)cv.width=W;if(cv.height!=H)cv.height=H;\n"
              "var pw=W-padL-padR,ph=H-padT-padB,now=Date.now(),tmin=now-(max-1)*step,tmax=now;\n"
              "x.fillStyle='#11161d';x.fillRect(0,0,W,H);\n"
              "var lo=Infinity,hi=-Infinity;\n"
              "for(var i=0;i<N;i++)for(var j=0;j<P[i].length;j++){var v=P[i][j];if(v<lo)lo=v;if(v>hi)hi=v;}\n"
              "if(!isFinite(lo)){lo=0;hi=1;}if(lo==hi){lo-=1;hi+=1;}\n"
              "var span=hi-lo,g=5;\n"
              "for(var i=0;i<=g;i++){\n"
              "var v=lo+span*i/g,y=padT+ph-ph*i/g;\n"
              "x.strokeStyle='#2a3340';x.beginPath();x.moveTo(padL,y);x.lineTo(W-padR,y);x.stroke();\n"
              "x.fillStyle='#8b98a5';x.font='11px Arial';x.textAlign='right';x.textBaseline='middle';\n"
              "x.fillText(v.toFixed(1),padL-6,y);\n"
              "}\n"
              "for(var i=0;i<N;i++){\n"
              "x.strokeStyle=cols[i];x.lineWidth=2;x.lineJoin='round';x.beginPath();var f=false;\n"
              "for(var j=0;j<P[i].length;j++){\n"
              "var tt=now-(P[i].length-1-j)*step;\n"
              "var px=padL+(tt-tmin)/(tmax-tmin)*pw;\n"
              "var py=padT+ph-(P[i][j]-lo)/span*ph;\n"
              "if(!f){x.moveTo(px,py);f=true;}else x.lineTo(px,py);\n"
              "}\n"
              "x.stroke();\n"
              "}\n"
              "x.textAlign='left';x.textBaseline='top';var lx=padL+4;\n"
              "for(var i=0;i<N;i++){\n"
              "x.fillStyle=cols[i];x.fillRect(lx,padT+4,12,3);\n"
              "x.fillStyle='#cfd8dc';x.font='12px Arial';x.fillText(names[i],lx+16,padT-2);\n"
              "lx+=x.measureText(names[i]).width+16+28;\n"
              "}\n"
              "}\n"
              "this.upd=function(name){\n"
              "var xh=new XMLHttpRequest();\n"
              "xh.onreadystatechange=function(){if(xh.readyState==4&&xh.status==200){\n"
              "var a=xh.responseText.split(',');\n"
              "for(var i=0;i<a.length&&i<N;i++){var nv=Number(a[i]);if(isNaN(nv))continue;P[i].push(nv);if(P[i].length>max)P[i].shift();}\n"
              "draw();\n"
              "}};\n"
              "xh.open('GET','/GP_update?'+name+'=',true);xh.send();\n"
              "};\n"
              "draw();window.addEventListener('resize',draw);\n"
              "}\n"
              "</script>\n"));
}

void build_plot(const String& name, const char** labels, int n, int** bufs, int count, int step, int window) {
    String s = F("<div id='");
    s += name;
    s += F("' class='chartBlock'></div><script>\nvar ");
    s += name;
    s += F("=new GPlot('");
    s += name;
    s += F("',{step:");
    s += String(step);
    s += F(",max:");
    s += String(window);
    s += F(",series:[");
    for (int i = 0; i < n; i++) {
        s += F("{name:'");
        s += String(labels[i]);
        s += F("',data:[");
        for (int j = 0; j < count; j++) {
            s += String(bufs[i][j]);
            if (j != count - 1) s += ",";
        }
        s += F("]}");
        if (i != n - 1) s += ",";
    }
    s += F("]});\nsetInterval(function(){");
    s += name;
    s += F(".upd('");
    s += name;
    s += F("');},");
    s += String(step);
    s += F(");</script>\n");
    GP.SEND(s);
}

void build() {
    GP.BUILD_BEGIN(GP_DARK);

    GP.NAV_TABS_LINKS("/,/settings","Главня,Настройки");

    if(ui.uri("/")) {
        build_plot_js();

        int cnt = get_MIN_count();
        int off = MINUTE_BUFFER_SIZE - cnt;
        int* temp_bufs[] = {get_MIN_BME280_temperature() + off, get_MIN_DS18B20_temperature() + off};
        build_plot("plot_temp", temp_names, 2, temp_bufs, cnt, SENSOR_UPDATE_MS, 100);

        int* arr_bufs[] = {get_MIN_BME280_humidity() + off, get_MIN_BME280_pressure() + off};
        build_plot("plot_arr", arr_names, 2, arr_bufs, cnt, SENSOR_UPDATE_MS, 100);

    } else if(ui.uri("/settings")) {

        GP.TITLE("Настройки");

        GP.UPDATE("online");
        GP.LED("online",wifi_connected());

        if (wifi_is_ap()) {
            GP.LABEL("Режим точки доступа. Введите данные вашего WiFi");
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