#include "host_driver.h"


#ifdef __cplusplus
extern "C" {
#endif

extern host_driver_t nrf51822_driver;

#ifdef __cplusplus
}
#endif


void nrf51822_init(void);
void nrf51822_software_reset(void);
void nrf51822_hardware_reset(void);
