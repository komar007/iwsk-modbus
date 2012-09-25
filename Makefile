CC=gcc -std=gnu99
CFLAGS=

all: master slave

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

master: master.o tty.o modbus.o
	$(CC) $(CFLAGS) $^ -o $@

slave: slave.o tty.o modbus.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	-rm -fr master slave *.o
