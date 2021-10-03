# simple_balloon_firmware
Simple open source solution for diy stratospheric balloon. Designed to run on arduino uno/nano. Created for NASA Space Apps Challenge 2021 

This firmware implements barometric data reading, gps navigation, gsm tracking and data logging to SD card.
It uses Arduino Nano/Uno, BMP280 digital barometer, MEO6-M GPS module, SIM800L GSM/GPRS Module and SPI SD cardreader module.



//////////////////////////////////////////////////////////////////////
CONNECTIONS:
//////////////////////////////////////////////////////////////////////

///////////////////////

ARDUINO          BMP280

5v----------------Vin

Gnd---------------Gnd

A4----------------SDA

A5----------------SCL

//////////////////////


//////////////////////////

ARDUINO          SD card

3.3v--------------Vin

Gnd---------------Gnd

D7----------------CS

D11---------------MOSI

D12---------------MISO

D13---------------SCK
////////////////////////


/////////////////////////

ARDUINO          Neo6-M

5v----------------Vin

Gnd---------------Gnd

D2----------------TX

D3----------------RX

//////////////////////////


//////////////////////////

ARDUINO          Sim800

3.3v--------------Vin

Gnd---------------Gnd

D7----------------TX

D8----------------RX

RST---------------RST

//////////////////////////

//////////////////////////////////////////////////////////////////////



To run this sketch you need following libraries:

Adafruit BMP280 Library : https://github.com/adafruit/Adafruit_BMP280_Library

TinyGPS++ : https://github.com/mikalhart/TinyGPSPlus

SIM800L Library Revised: https://github.com/vittorioexp/Sim800L-Arduino-Library-revised

