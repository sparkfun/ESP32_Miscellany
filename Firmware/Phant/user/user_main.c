/******************************************************************************
user_main.c
Main source file for Phant application
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
#include "phant.h"
#include "user_config.h"

////////////////////
// WiFi Functions //
////////////////////
void connectWiFi(void); // Utility function to connect to a WiFi network
void wifi_event_handler_cb(System_Event_t *event); // WiFi event callback

/////////////////////////////////////
// Timer Functions and Definitions //
/////////////////////////////////////
// setupTimer is a generic timer-setting function. Give it a os_timer_t pointer,
// milliseconds between call, and a function to be called
void setupTimer(os_timer_t * timer, uint32_t ms, void * func);
os_timer_t blink_timer; // Blink timer
void blink(void); // Blink function
os_timer_t post_timer; // Post timer
void post(void); // Post function

////////////////////////////////////////////
// user_init - Entry point of application //
////////////////////////////////////////////
void user_init(void)
{
	// Set the LED as an output, turn it off:
	GPIO_AS_OUTPUT(LED_PIN);
	GPIO_OUTPUT_SET(LED_PIN, 0);
	
	// Configure the UART, print a message:
	UART_SetBaudrate(UART0, BIT_RATE_115200);
    printf("SDK version:%s\n", system_get_sdk_version());
	
	// Connect to WiFi:
	connectWiFi();
}

////////////////////////////////////////////////////////////////////
// connectWiFi() - Connects to SSID and PASSWORD in user_config.h //
////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////
// wifi_event_handler_cb() - Callback function for WiFi events //
/////////////////////////////////////////////////////////////////
void wifi_event_handler_cb(System_Event_t *event)
{
    if (event == NULL) 
	{
        return;
    }

    switch (event->event_id)  // Switch on the event that brought us here
	{
		case EVENT_STAMODE_SCAN_DONE: //  Once the scan is  done,
			setupTimer(&blink_timer, 100/2, blink); // Blink at 10Hz
			break;
		case EVENT_STAMODE_CONNECTED: // Once we've connected (no IP yet),
			setupTimer(&blink_timer, 250/2, blink); // Blink at 4Hz
			break;
        case EVENT_STAMODE_GOT_IP: // If we're connected and received an IP,
			// Start the Phant post timer:
			setupTimer(&post_timer, POST_FREQUENCY, post);
			setupTimer(&blink_timer, 0, blink); // Turn blink off
			GPIO_OUTPUT_SET(LED_PIN, 1); // Turn LED on
            break;
		case EVENT_STAMODE_DISCONNECTED: // If disconnected,
			setupTimer(&blink_timer, 5000/2, blink); // blink every 5s
			break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////
// setupTimer() - Starts a timer to be called every ms milliseconds, //
//                and calls the func function                        //
///////////////////////////////////////////////////////////////////////
void setupTimer(os_timer_t * timer, uint32_t ms, void * func)
{
	// Disarm the blink_timer timer before (re-)configuring it:
	os_timer_disarm(timer);
	
	if (ms > 0)
	{
		// Now set which function blink_timer will call, and define arguments:
		os_timer_setfn(timer, (os_timer_func_t *)func, (void *) 0); 
		// Now arm the timer and set the length between calls:
		os_timer_arm(timer, ms, 1);
	}
}

/////////////////////////////////////////////////////////////////////
// post() - Creates a Phant post, and sends it to the Phant server //
/////////////////////////////////////////////////////////////////////
void post(void)
{
	//initPhant(); // Initialize a new Phant post
	//initPhant("jqLLvVZo5zS2o8mx7a2z", "zz554jvKx0C8ADVn1E8J");
	initPhant(publicKey, privateKey);
	int i=0;
	for (; i<NUM_PHANT_FIELDS; i++)
	{
		// Use phantAdd to add a field/value combo to the Phant post
		// Call system_adc1_read(pad, atten) to read an ADC pad.
		// At ADC1_ATTEN_12DB, the ADC range is 0-4V.
		phantAdd(fieldOrder[i], system_adc1_read(padOrder[i], ADC1_ATTEN_12DB));
	}
	// Once the post is assembled, call phantSend() to send it to the server
	phantSend();
}

/////////////////////////////////////////////////////////////
// blink() - Changes the state of the LED each time called //
/////////////////////////////////////////////////////////////
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