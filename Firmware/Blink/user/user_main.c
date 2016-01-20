/******************************************************************************
user_main.c
Main source file for Blink application
Jim Lindblom @ SparkFun Electronics
January 20, 2016
https://github.com/sparkfun/ESP32_Miscellany

This source file includes the user_init() function - entry point for the 
application, as well as a helper function to get connnected to a WiFi network.

The WiFi network definitions are in "include/user_config.h".

Resources:
Requires the ESP32 RTOS SDK: https://github.com/espressif/ESP32_RTOS_SDK

Development environment specifics:
ESP32 RTOS SDK v1.1.0
ESP31B ESP-WROOM-03 Module
******************************************************************************/

#include "esp_common.h"
#include "gpio.h"
#include "uart.h"
#include "user_config.h"

////////////////////////////
// Blinky LED definitions //
////////////////////////////
os_timer_t blink_timer;
void blink(void);
void setupBlinkTimer(uint32_t ms);

//////////////////////////////////
// WiFi Functions and Constants //
//////////////////////////////////
void connectWiFi(void); // Utility function to connect to a WiFi network
void wifi_event_handler_cb(System_Event_t *event); // WiFi event callback

////////////////////////////////////////////
// user_init - Entry point of application //
////////////////////////////////////////////
void user_init(void)
{
	// Set the LED as an output, turn it on:
	GPIO_AS_OUTPUT(LED_PIN);
	GPIO_OUTPUT_SET(LED_PIN, 1);
	
	// Configure the UART, print a message:
	UART_SetBaudrate(UART0, BIT_RATE_115200);
    printf("SDK version:%s\n", system_get_sdk_version());
	
	// Connect to WiFi:
	connectWiFi();
}

void connectWiFi(void)
{
    wifi_set_opmode(STATION_MODE); // Set into station mode

    // Create a station_config struct, and fill it with our SSID and Password
    struct station_config config;
    bzero(&config, sizeof(struct station_config)); // Zero it out first
    sprintf(config.ssid, SSID); // Put the SSID in
    sprintf(config.password, PASSWORD); // Put the password in
	
	// Put our new config struct into the wifi_station_set_config function
    wifi_station_set_config(&config);
	// Set up a callback function for any WiFi event
    wifi_set_event_handler_cb(wifi_event_handler_cb);
}

void wifi_event_handler_cb(System_Event_t *event)
{
    if (event == NULL) 
	{
        return;
    }

    switch (event->event_id)  // Switch on the event that brought us here
	{
		case EVENT_STAMODE_SCAN_DONE: //  Once the scan is  done, blink at 10Hz
			setupBlinkTimer(BLINK_RATE_SCAN);
			break;
		case EVENT_STAMODE_CONNECTED:
			setupBlinkTimer(BLINK_RATE_CONNECT); // Once connected to WiFi (no IP yet), blink at 4Hz
			break;
        case EVENT_STAMODE_GOT_IP: // If we got an IP address,
			setupBlinkTimer(BLINK_RATE_GOT_IP); // blink at 1Hz
            break;
		case EVENT_STAMODE_DISCONNECTED: // If we get disconnected, blink every 5 seconds
			setupBlinkTimer(BLINK_RATE_DISCONNECT);
			break;
        default:
            break;
    }
}

void setupBlinkTimer(uint32_t ms)
{
	// Disarm the blink_timer timer before (re-)configuring it:
	os_timer_disarm(&blink_timer);
	// Now set which function blink_timer will call, and define arguments (if any):
	os_timer_setfn(&blink_timer, (os_timer_func_t *) blink, (void *) 0); 
	// Now arm the timer and set the length between calls:
	os_timer_arm(&blink_timer, ms/2, 1);
}

void blink(void)
{
	static bool flag = false;
	if (flag)
	{
		GPIO_OUTPUT_SET(LED_PIN, 1);
		flag = false;
	}
	else
	{
		GPIO_OUTPUT_SET(LED_PIN, 0);
		flag = true;
	}
}

