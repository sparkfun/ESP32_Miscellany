/******************************************************************************
phant.h
Phant lwIP library - header
Jim Lindblom @ SparkFun Electronics
January 20, 2016
https://github.com/sparkfun/ESP32_Miscellany

Assemble a Phant post, and send it using lwIP.

Resources:
Requires the ESP32 RTOS SDK: https://github.com/espressif/ESP32_RTOS_SDK

Development environment specifics:
ESP32 RTOS SDK v1.1.0
ESP31B ESP-WROOM-03 Module
******************************************************************************/

#ifndef PHANT_H
#define PHANT_H

//void initPhant(void); // Initialze a new Phant post
void initPhant(const char * pub, const char * prv);
void phantAdd(char * field, int data); // Add a field/value combo to the post
void phantSend(void); // Send the assembled post to the Phant server

#endif