KDIR	:= /home/work/source/rebis-2.6
PWD		:= $(shell pwd)
CC		:= arm-linux-gcc

TARGET	:= /tftpboot/myrootdir/root
OBJS	:= oo_fb.ko
obj-m	:= oo_fb.o

default: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	cp -f $(OBJS) $(TARGET)

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
