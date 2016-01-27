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

static void mods_to_keycodes(report_keyboard_t *report, uint8_t *keycodes){
	uint8_t mods = report->mods;
	while(mods){
		if(mods >= 128){
			mods = mods % 128;
			keycodes[7] = 0xE7; //HID_KEYBOARD_RIGHT_GUI
		}else if(mods >= 64){
			mods = mods % 64;
			keycodes[6] = 0xE6; //HID_KEYBOARD_RIGHT_ALT
		}else if(mods >= 32){
			mods = mods % 32;
			keycodes[5] = 0xE5; //HID_KEYBOARD_RIGHT_SHIFT
		}else if(mods >= 16){
			mods = mods % 16;
			keycodes[4] = 0xE4; //HID_KEYBOARD_RIGHT_CTRL
		}else if(mods >= 8){
			mods = mods % 8;
			keycodes[3] = 0xE3; //HID_KEYBOARD_LEFT_GUI
		}else if(mods >= 4){
			mods = mods % 4;
			keycodes[2] = 0xE2;//HID_KEYBOARD_LEFT_ALT
		}else if(mods >= 2){
			mods = mods % 2;
			keycodes[1] = 0xE1; //HID_KEYBOARD_LEFT_SHIFT
		}else if(mods >= 1){
			mods = mods % 1;
			keycodes[0] = 0xE0; //HID_KEYBOARD_LEFT_CTRL
		}
	}
}

static void send_keyboard(report_keyboard_t *report)
{
	uint8_t keycodes[KEYBOARD_REPORT_SIZE + 8] = {0};
	uint8_t size = 8; //8 byte mods reserve
	mods_to_keycodes(report, &keycodes);

	for (uint8_t i = 0; i < KEYBOARD_REPORT_SIZE; i++) {
		if(report->keys[i] == 57){
			nrf51822_keyboard_leds = nrf51822_keyboard_leds? 0:2;
		}
		if(report->mods > 0 && i == (KEYBOARD_REPORT_SIZE-1)){
			keycodes[size++] = 0; //mods byte skip
		}else{
			keycodes[size++] = report->keys[i];
		}
	}

	char at_command[128] = {0};
	char * pos = at_command;
	pos += sprintf(pos, "AT+KEY=");

	for (uint8_t i = 0 ; i < size ; i++) {
		if (i) {
			pos += sprintf(pos, "+");
		}
		pos += sprintf(pos, "%d", keycodes[i]);
	}
	_nrf51822_serial_send_line((uint8_t *)at_command);
}

static uint8_t keyboard_leds(void) {
    return nrf51822_keyboard_leds;
}

static void send_mouse(report_mouse_t *report)
{
	if(report->h != 0 || report->v != 0){
		char str_key[64] = { 0 };
		sprintf((char *) str_key, "AT+MOUSE_MOVEMENT=%d,%d", report->h, report->v);
		_nrf51822_serial_send_line((uint8_t *)str_key);
	}
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
