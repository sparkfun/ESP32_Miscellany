/******************************************************************************
phant.c
Phant lwIP library - source
Jim Lindblom @ SparkFun Electronics
January 20, 2016
https://github.com/sparkfun/ESP32_Miscellany

Assemble a Phant post, and send it using lwIP.

Resources:
Requires the ESP32 RTOS SDK: https://github.com/espressif/ESP32_RTOS_SDK

Development environment specifics:
ESP32 RTOS SDK v1.1.0
ESP31B ESP-WROOM-03 Module

	Sources: http://lwip.wikia.com/wiki/Raw/TCP
	http://stackoverflow.com/questions/26192758/how-can-i-send-a-simple-http-request-with-a-lwip-stack
******************************************************************************/
#include "phant.h"
#include "esp_common.h"
#include "string.h"

#include "lwip/tcp.h"
#include "lwip/ip4.h"

uint8_t dest_ip[4] = {54, 86, 132, 254};
const char phantServer[] = "data.sparkfun.com";

int connect(void);
void sendData(void);
struct tcp_pcb * pcb;
void tcpErrorHandler(void *arg, err_t err);
err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t tcpSendCallback(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t connectCallback(void *arg, struct tcp_pcb *tpcb, err_t err);

char postContent[512];

void initPhant(const char * pub, const char * prv)
{
	memset(postContent, 0, 512);
	sprintf(postContent, "GET /input/%s?private_key=%s", pub, prv);
}

void phantSend(void)
{
	connect();
	// connect() will connect to the server, the rest of our posting
	// is taken care of through callbacks
}

void phantAdd(char * field, int data)
{
	// Add the field/data value to our postContent string:
	char temp[32];
	memset(temp, 0, 32);
	sprintf(temp, "&%s=%d", field, data);
	strcat(postContent, temp);
}

void sendData(void)
{
	// Create the end of the post
	strcat(postContent, " HTTP/1.0\r\nHost: ");
	strcat(postContent, phantServer);
	strcat(postContent, "\r\n\r\n ");
	
	uint32_t len = strlen(postContent);
	
	// Queue data up. Set API flag to allocate new memory and copy data into it.
	//err_t errRet = tcp_write(pcb, string, len, TCP_WRITE_FLAG_COPY);
	err_t errRet = tcp_write(pcb, postContent, len, TCP_WRITE_FLAG_COPY);
	
	if (errRet != ERR_OK)
	{
		printf("Error queueing data: %d\r\n", errRet);
		return;
	}
	
	// Send data:
	errRet = tcp_output(pcb);
	
	if (errRet != ERR_OK)
	{
		printf("Error sending data: %d\r\n", errRet);
		return;
	}
	return;
}

int connect(void)
{
	uint32_t data = 0x55555555; // Data to identify connection
	err_t errRet;
	
	struct ip_addr dest;
	// Create an ip_addr variable, based on the dest_ip global:
	IP4_ADDR(&dest, dest_ip[0], dest_ip[1], dest_ip[2], dest_ip[3]);
	
	pcb = tcp_new(); // Create a new tcp connection
	if (pcb == NULL) // tcp_new returns NULL if it failed
	{
		printf("Failed to create connection\r\n");
		return -1;
	}
	// Otherwise tcp_new will return a tcp_pcb "protocol control block" pointer
	tcp_arg(pcb, &data); // Assign our data to the pcb
	
	// Assign error, receive and sent callback functions to the pcb
	tcp_err(pcb, tcpErrorHandler);
	tcp_recv(pcb, tcpRecvCallback);
	tcp_sent(pcb, tcpSendCallback);
	
	// Call tcp_connect to assign our pcb to a connection.
	// dest is the IP address generated from the dest_ip global
	// 80 is the port
	// connectCallback is the callback function to be called when the pcb connects
	errRet = tcp_connect(pcb, &dest, 80, connectCallback);
	if (errRet == ERR_OK)
	{
		printf("Setting up the connection\r\n");
		return 1;
	}
	else
	{
		printf("Error setting up connection\r\n");
		return -1;
	}
}

void tcpErrorHandler(void *arg, err_t err)
{
	printf("TCP Error: %d\r\n", err);
}

err_t tcpRecvCallback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	printf("Data received.\r\n====================\r\n\r\n");
	if (p == NULL)
	{
		printf("Closing connection");
		//tcp_close(pcb); // Hmm, this causes the ESP32 to reset, doing something bad to memory somewhere
		return ERR_ABRT;
	}
	else
	{
		printf("Return data size: %d\r\n", p->len);
		printf("Contents: %s\r\n", (char *)p->payload);
		printf("\r\n====================\r\n");
	}
	return ERR_OK;
}

err_t tcpSendCallback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	printf("TCP Data sent\r\n");
	return ERR_OK;
}

err_t connectCallback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	printf("TCP Connection established\r\n");
	sendData(); // Call the sendData() function to send our Phant data.
	return ERR_OK;
}
