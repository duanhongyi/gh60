#include "host_driver.h"


void nrf51822_init(void);
void nrf51822_software_reset(void);
void nrf51822_hardware_reset(void);
host_driver_t *nrf51822_driver(void);
