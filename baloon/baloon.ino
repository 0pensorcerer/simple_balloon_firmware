 /*
 * Created by "PARADOX" Team for "NASA Space Apps Challenge 2021"
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/



//необходимые библиотеки
#include "Pinout.h"
#include "sys_var.h"
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Adafruit_BMP280.h"
#include "TinyGPS++.h"
#include "Sim800L.h"
#include <SD.h>

//создаем объекты библиотечных связей
SoftwareSerial GPS_PORT(Soft_UART1_RX, Soft_UART1_TX);
Sim800L GSM(Soft_UART2_RX, Soft_UART2_TX/*,RESET*/);
TinyGPSPlus gps;
Adafruit_BMP280 bmp;
File Log;

//инициализация таймеров
void timers_init()
{
  elapsed_time = 0;
  sensor_timer = 0;
  sms_timer = 0;
  log_timer = 0;
}

//инициализация gps модуля
void gps_init()
{
  GPS_PORT.begin(9600);
  while (GPS_PORT.available() <= 0) {}
  gps_update();
}

//инициализация gsm модуля
void gsm_init()
{
  GSM.begin(4800);
  GSM.sendSms(PHONE_NUM, "GSM initialization. SMS test");
}

//инициализация модуля сенсоров
void sensors_init()
{
  bmp.begin(0x76);//specified adress for module
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

////инициализация sd карты
bool sd_init()
{
  const char test_log[] = "SD card initialization. Test log.";
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS))
  {
    return 0;
  }
  else
  {
    Log = SD.open("log.txt", FILE_WRITE);
    if (Log)
    {
      Log.println(test_log);
      Log.close();
      log_timer = 0;
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

//обновление таймеров
void timers_update()
{
  sms_timer += millis() - elapsed_time;
  log_timer += millis() - elapsed_time;
  sensor_timer += millis() - elapsed_time;
  elapsed_time = millis();
}

//обновление показаний датчиков
void sensors_update(uint32_t sensor_timeout = 0)
{
  if (sensor_timer >= sensor_timeout)
  {
    pressure = bmp.readPressure();//считываем давление
    temperature = bmp.readTemperature();//считываем температуру
    bar_alt = bmp.readAltitude(ZERO_LVL_PRESSURE);// рассчитываем высоту
    sensor_timer = 0;//обнуляем таймер
  }
}

//обновление показаний gps
void gps_update()
{
  while (GPS_PORT.available() > 0)//если в порте есть данные
  {
    if (gps.encode(GPS_PORT.read()))
    {
      if (gps.time.isValid())//считываем время
      {
        gps_hour = gps.time.hour();
        gps_min = gps.time.minute();
        gps_sec = gps.time.second();
      }

      if (gps.location.isValid())//считываем координаты
      {
        gps_lng = gps.location.lng();
        gps_lat = gps.location.lat();
      }

      if (gps.speed.isValid())//считываем скорость
      {
        gps_spd = gps.speed.mps();
        //gps_spd = gps.speed.kmph();
      }

      if (gps.altitude.isValid())//считываем высоту
      {
        gps_alt = gps.altitude.meters();
      }
    }
  }
}

//отправляем сообщение с координатами
bool send_log(uint32_t sms_timeout)
{
  if ((sms_timer >= sms_timeout) && (bar_alt <= 4000))
  {
    String data = "Alt: " + String(gps_alt) + "; " + '\n' + "Lng: " +
                  String(gps_lng) + "; " + '\n' + "Lat: " + String(gps_lat) + "Spd: " + String(gps_spd) + ";";

    if (GSM.sendSms(PHONE_NUM, data[0]))
    {
      sms_timer = 0;
      return 1;
    }
    else return 0;
  }
}

//сохраняем лог на карту памяти
bool save_log(uint32_t log_timeout)
{
  if (log_timer >= log_timeout)
  {
    Log = SD.open("log.txt", FILE_WRITE);
    if (Log)
    {
      Log.println("Alt: " + String(gps_alt) + "; Lng: " + String(gps_lng) + "; Lat: " + String(gps_lat) + "; Spd: " + String(gps_spd)
                  + "; Pressure: " + String(pressure) + "; Temperature: " + String(temperature) + "; Barometric_Altitude" + String(bar_alt)
                  + "; Time: " + String(gps_hour) + ":" + String(gps_min) + ":" + String(gps_sec));
      Log.close();
      log_timer = 0;
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  timers_init();
  gsm_init();
  sensors_init();
  gps_init();
  sd_init();
}


void loop()
{
  timers_update();
  sensors_update();
  gps_update();
  send_log(SMS_TIMOUT);
  save_log(LOG_TIMOUT);
}
