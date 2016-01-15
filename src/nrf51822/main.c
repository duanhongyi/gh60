#include "sendchar.h"
#include "descriptor.h"
#include "keyboard.h"
#include "lufa.h"
#include "print.h"
#include "protocol/serial.h"

#include "mixin_driver.h"

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
}


int main(void)  __attribute__ ((weak));
int main(void)
{
    setup_mcu();
    keyboard_setup();
    sei();
    serial_init();
    setup_usb();

    /* init modules */
    keyboard_init();
    host_set_driver(&mixin_driver);
#ifdef SLEEP_LED_ENABLE
    sleep_led_init();
#endif
    print("Keyboard start.\n");
    while (1) {
    	keyboard_task();
    	mixin_driver_task();
    }
}
