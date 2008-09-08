obj-m	:=proc_dev.o

KDIR	:=/lib/modules/2.6.17.13/build
PWD	:=$(shell pwd)
CC = arm-linux-gcc

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
