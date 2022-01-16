# name of the application
APPLICATION = projet-iot

# The default board is ST B-L072Z-LRWAN1 LoRa discovery board
BOARD ?= lora-e5-dev

RIOTBASE ?= $(CURDIR)/../../RIOT

#
# Here we add the modules that are needed
#
USEMODULE += xtimer
USEMODULE += ztimer
USEMODULE += auto_init_loramac
USEMODULE += lm75a
#
# The application needs LoRaWAN related modules and variables:
#

# The Semtech LoRa radio device (SX126X)
USEMODULE += sx126x_stm32wl

# The Semtech Loramac package
USEPKG += semtech-loramac
USEPKG += cayenne-lpp

#FEATURES_REQUIRED += periph_rtc
FEATURES_REQUIRED += periph_gpio_irq

# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# This must be the last line of the Makefile
include $(RIOTBASE)/Makefile.include
