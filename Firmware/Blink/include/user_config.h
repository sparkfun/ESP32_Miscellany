/******************************************************************************
user_config.h
Config file for Blink application
Jim Lindblom @ SparkFun Electronics
January 20, 2016
https://github.com/sparkfun/ESP32_Miscellany

This file defines the WiFi network and blinking LED pin.

Resources:
Requires the ESP32 RTOS SDK: https://github.com/espressif/ESP32_RTOS_SDK

Development environment specifics:
ESP32 RTOS SDK v1.1.0
ESP31B ESP-WROOM-03 Module
******************************************************************************/

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

/////////////////////////
// WiFi Network Config //
/////////////////////////
#define SSID "YOUR_WIFI_SSID" // WiFi Network SSID
#define PASSWORD "YOUR_WIFI_PASSWORD" // WiFi Network password

////////////////
// LED Config //
////////////////
#define LED_PIN 16
#define BLINK_RATE_SCAN 100
#define BLINK_RATE_CONNECT 250
#define BLINK_RATE_GOT_IP 1000
#define BLINK_RATE_DISCONNECT 5000

#endif

