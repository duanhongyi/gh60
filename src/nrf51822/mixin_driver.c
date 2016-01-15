#include <stdint.h>
#include <string.h>

#include "print.h"
#include "suspend.h"
#include "host_driver.h"
#include "lufa.h"
#include "common/wait.h"

#include "./mixin_driver.h"
#include "./nrf51822_driver.h"



/* Host driver */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

host_driver_t mixin_driver = {
        keyboard_leds,
        send_keyboard,
        send_mouse,
        send_system,
        send_consumer
};

static host_driver_t * default_driver = &nrf51822_driver;

void mixin_change_driver(host_driver_t * driver){
	if(driver != default_driver){
		default_driver = driver;
	}
}

void mixin_driver_task(void){
	if(default_driver == &lufa_driver){
		while (USB_DeviceState == DEVICE_STATE_Suspended) {
			print("[s]");
			suspend_power_down();
			if (USB_Device_RemoteWakeupEnabled && suspend_wakeup_condition()) {
					USB_Device_SendRemoteWakeup();
			}
		}
	#if !defined(INTERRUPT_CONTROL_ENDPOINT)
		USB_USBTask();
	#endif
	}
}


static void send_keyboard(report_keyboard_t *report)
{
	default_driver->send_keyboard(report);
}

static uint8_t keyboard_leds(void) {
	return default_driver->keyboard_leds();
}

static void send_mouse(report_mouse_t *report)
{
	return default_driver->send_mouse(report);
}

static void send_system(uint16_t data)
{
	return default_driver->send_system(data);
}

static void send_consumer(uint16_t data)
{
	return default_driver->send_consumer(data);
}
