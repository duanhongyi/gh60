NRF51822_DIR = src/nrf51822

SRC += protocol/serial_soft.c

SRC += src/nrf51822/main.c \
	src/nrf51822/nrf51822_driver.c \
	src/nrf51822/mixin_driver.c \


OPT_DEFS += -DPROTOCOL_NRF51822

VPATH += $(NRF51822_DIR)