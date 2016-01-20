/******************************************************************************
user_config.h
Configuration variables for the Phant example application
Jim Lindblom @ SparkFun Electronics
January 20, 2016
https://github.com/sparkfun/ESP32_Miscellany

This file defines the desired WiFi network, and determines the Phant stream
fields and values.

Resources:
Requires the ESP32 RTOS SDK: https://github.com/espressif/ESP32_RTOS_SDK

Development environment specifics:
ESP32 RTOS SDK v1.1.0
ESP31B ESP-WROOM-03 Module
******************************************************************************/

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define SSID "YOUR_WIFI_SSIDE" // WiFi Network SSID
#define PASSWORD "YOUR_WIFI_PASSWORD" // WiFi Network password

//////////////////
// Phant Config //
//////////////////
#define POST_FREQUENCY 15000 // Post every 15 seconds
const char publicKey[] = "PHANT_PUBLIC_KEY";
const char privateKey[] = "PHANT_PRIVATE_KEY";
#define NUM_PHANT_FIELDS 8 // Number of Phant fields
// Names of the Phant fields:
char *fieldOrder[NUM_PHANT_FIELDS] = {"pad32", "pad33", "pad34", "pad35", 
                        "pad36", "pad37", "pad38", "pad39"};
// ADC pads mapped to Phant fields:
adc1_read_pad padOrder[NUM_PHANT_FIELDS] = {ADC1_PAD_GPIO32, ADC1_PAD_GPIO33, ADC1_PAD_GPIO34,
                             ADC1_PAD_GPIO35, ADC1_PAD_GPIO36, ADC1_PAD_GPIO37, 
							 ADC1_PAD_GPIO38, ADC1_PAD_GPIO39};
					 
///////////////////////
// Blinky LED Config //
///////////////////////
#define LED_PIN 16 // LED attached to pin 16

#endif

