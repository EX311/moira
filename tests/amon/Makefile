#
## Makfile for bmp example
#
#

CC= arm-linux-gcc
CFLAGS= -g -Wall
LDFLAGS=
OBJS= fb-util.o oo_network.o bmplib.o my_test.o
EXE=my_test
TARGET=/tftpboot/myrootdir/root/

all: my_test

my_test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

install:
	cp -f $(EXE) $(TARGET)
clean:
	rm -f $(OBJS) my_test 
