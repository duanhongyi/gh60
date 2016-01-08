NRF51822_DIR = src/nrf51822

SRC +=  protocol/serial_soft.c \
	src/nrf51822/main.c \
	src/nrf51822/nrf51822_driver.c

VPATH += $(NRF51822_DIR)