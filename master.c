#include "modbus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct config cfg;
	if (modbus_open(&cfg, argv[1], 9600, 20000, 10000) < 0) {
		perror("modbus_open:");
		exit(-1);
	}
	int timeout = atoi(argv[2]);
	char *text = "kupka";
	struct frame frame;
	printf("sending frame to slave... ");
	modbus_make_frame(&frame, 0x03, 0x02, strlen(text), text);
	modbus_send_frame(&cfg, &frame);
	modbus_free_frame(&frame);
	printf("sent!\nwaiting for reply... ");

	if (modbus_recv_frame(&cfg, &frame, timeout) < 0) {
		printf("timed out:(\n");
		exit(-1);
	}
	if (!modbus_frame_correct(&frame)) {
		printf("received corrupted frame!\n");
		return -1;
	}
	printf("got reply:\n");
	modbus_print_frame(&frame);
	if (modbus_get_frame_func(&frame) & (1 << 7))
		printf("slave returned error\n");
	else
		printf("slave performed correctly\n");
	size_t len = modbus_get_frame_payload_len(&frame);
	if (len > 0) {
		char *buf = malloc(len+1);
		memcpy(buf, modbus_get_frame_payload(&frame), len);
		buf[len] = '\0';
		printf("received reply: %s\n", buf);
	} else {
		printf("received no reply\n");
	}
	return 0;
}
