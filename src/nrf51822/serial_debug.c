#include "serial_debug.h"

/*
 * serial_debug.c
 *
 *  Created on: 2016年1月21日
 *      Author: duanhongyi
 */


void serial_debug_str(uint8_t * line){
    for (uint8_t i = 0; i < strlen((char *)line); i++) {
    	serial_send(line[i]);
    }
    serial_send((uint8_t)'\r');
    serial_send((uint8_t)'\n');
}

void serial_debug_num(uint32_t num){
	char str_key[32] = { 0 };
	sprintf((char *) str_key, "%d", num);
	serial_debug_str((uint8_t *) str_key);
}
