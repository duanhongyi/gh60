#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "suspend.h"
#include "sendchar.h"
#include "descriptor.h"
#include "keyboard.h"
#include "lufa.h"
#include "print.h"
#include "wait.h"
#include "protocol/serial.h"


/*******************************************************************************
 * main
 ******************************************************************************/
static void setup_mcu(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    //Disable jtag
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
}

static void setup_usb(void)
{
    // Leonardo needs. Without this USB device is not recognized.
    USB_Disable();

    USB_Init();

    // for Console_Task
    USB_Device_EnableSOFEvents();
    print_set_sendchar(sendchar);
    /* wait for USB startup & debug output */
    uint8_t timeout = 255;  // timeout when USB is not available
    while (timeout-- &&USB_DeviceState != DEVICE_STATE_Configured) {
        wait_ms(4);
#if defined(INTERRUPT_CONTROL_ENDPOINT)
        ;
#else
        USB_USBTask();
#endif
    }
    print("USB configured.\n");
}



int main(void)  __attribute__ ((weak));
int main(void)
{
    setup_mcu();
    keyboard_setup();
    setup_usb();
    sei();
    /* init modules */
    serial_init();
    keyboard_init();
    host_set_driver(&lufa_driver);
#ifdef SLEEP_LED_ENABLE
    sleep_led_init();
#endif
    print("Keyboard start.\n");
    while (1) {
        while (USB_DeviceState == DEVICE_STATE_Suspended) {
            print("[s]");
            matrix_power_down();
            suspend_power_down();
            if (USB_Device_RemoteWakeupEnabled && suspend_wakeup_condition()) {
                    USB_Device_SendRemoteWakeup();
            }
        }

        keyboard_task();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
        USB_USBTask();
#endif
    }
}
