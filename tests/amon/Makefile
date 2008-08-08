#
## Makfile for bmp example
#
#

CC= arm-linux-gcc
CFLAGS= -g -Wall
LDFLAGS=
OBJS= fb-util.o test.o
EXE=test
TARGET=/tftpboot/myrootdir/root/

all: test

test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

install:
	cp -f $(EXE) $(TARGET)
clean:
	rm -f $(OBJS) test 
