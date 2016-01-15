#include "host_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

extern host_driver_t mixin_driver;

#ifdef __cplusplus
}
#endif

void mixin_driver_task(void);
void mixin_change_driver(host_driver_t * driver);
