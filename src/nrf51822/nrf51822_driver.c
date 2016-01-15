#include <stdint.h>
#include "host_driver.h"
#include "protocol/serial.h"
#include "string.h"
#include "common/wait.h"

static uint8_t nrf51822_keyboard_leds = 0;


/* Host driver */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

host_driver_t nrf51822_driver = {
        keyboard_leds,
        send_keyboard,
        send_mouse,
        send_system,
        send_consumer
};


static void _nrf51822_serial_send_line(uint8_t * line){
    for (uint8_t i = 0; i < strlen((char *)line); i++) {
    	serial_send(line[i]);
    }
    serial_send((uint8_t)'\r');
    serial_send((uint8_t)'\n');
}


static void send_keyboard(report_keyboard_t *report)
{
	char at_command[128] = {0};
	memcpy(at_command, "AT+KEY=", 7);
	uint8_t count = 7;
    for (uint8_t i = 0; i < KEYBOARD_REPORT_SIZE; i++) {
    	if(i != 0){
    		at_command[count++] = '+';
    	}
    	uint8_t key = report->raw[i];
    	char str_key[3] = { 0 };
    	sprintf((char *) str_key, "%d", (int) key);

    	for(int j=0; j<3; j++){
    		if(str_key[j] == '\0'){
    			break;
    		}else{
    			at_command[count++] = str_key[j];
    		}
    	}
    }
    _nrf51822_serial_send_line((uint8_t *)at_command);
}

static uint8_t keyboard_leds(void) {
    return nrf51822_keyboard_leds;
}

static void send_mouse(report_mouse_t *report)
{
	char str_key[64] = { 0 };
	sprintf((char *) str_key, "AT+MOUSE_MOVEMENT=%d,%d", report->h, report->v);
	_nrf51822_serial_send_line((uint8_t *)str_key);
}

static void send_system(uint16_t data)
{
	//ble hids not support system
}

static void send_consumer(uint16_t data)
{
	uint8_t key =(uint16_t) data;
	char str_key[15] = { 0 };
	sprintf((char *) str_key, "AT+CONSUMER=%d", (int) key);
	_nrf51822_serial_send_line((uint8_t *)str_key);
}

void nrf51822_init(void){
	serial_init();
	//HI
	DDRF &= ~(1<<6);
	PORTF &= ~(1<<6);
}

void nrf51822_software_reset(void){
	_nrf51822_serial_send_line((uint8_t *)"AT+SYSTEM=RESET");
}

void nrf51822_hardware_reset(void){
	//LOW
    DDRF |= (1<<6);
    PORTF &= ~(1<<6);
    _delay_ms(1000);
    //HI
	DDRF &= ~(1<<6);
	PORTF &= ~(1<<6);
}

void nrf51822_task(void){
	serial_recv();
}
