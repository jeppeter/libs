# This should be defined if the modules from pcmcia-cs are used.
# Leave it blank if you have PCMCIA support in the kernel.
PCMCIA_PATH =

KERNEL_VERSION = $(shell uname -r)
KERNEL_PATH = /lib/modules/$(KERNEL_VERSION)/build

OUR_DIR = $(shell pwd)

all: modules

obj-m := plx9052.o

modules clean modules_install:
	$(MAKE) -C $(KERNEL_PATH) M=$(OUR_DIR) "$@"
