#include "modbus.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct config cfg;
char *text_to_send = "ala ma kota";

void process_frame(struct frame *frame)
{
	int func = modbus_get_frame_func(frame);
	int addr = modbus_get_frame_addr(frame);
	if (func == 1) {
		size_t len = modbus_get_frame_payload_len(frame);
		char *text = malloc(len + 1);
		memcpy(text, modbus_get_frame_payload(frame), len);
		text[len] = '\0';
		printf("received text from master: %s\n", text);
		struct frame frame;
		int func_reply = func;
		/* one in five times send an error response (MSB set in
		 * function field */
		if (rand() % 5 == 0) {
			func_reply |= (1 << 7);
			printf("sending error reply to master... ");
		} else {
			printf("sending success reply to master... ");
		}
		modbus_make_frame(&frame, addr, func_reply, 0, NULL);
		modbus_send_frame(&cfg, &frame);
		modbus_free_frame(&frame);
		printf("sent!\n");
	} else if (func == 2) {
		printf("received text request from master\n");
		printf("sending text \"%s\"... ", text_to_send);
		struct frame frame;
		modbus_make_frame(&frame, addr, func, strlen(text_to_send), text_to_send);
		modbus_send_frame(&cfg, &frame);
		modbus_free_frame(&frame);
		printf("sent!\n");
	} else {
		printf("received unimplemented function\n");
	}
}

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	if (modbus_open(&cfg, argv[1], 9600, 20000, 10000) < 0) {
		perror("modbus_open:");
		exit(-1);
	}
	int self_addr = atoi(argv[2]);
	while (true) {
		struct frame frame;
		if (modbus_recv_frame(&cfg, &frame, 0) < 0)
			continue;

		if (modbus_frame_correct(&frame)) {
			int addr = modbus_get_frame_addr(&frame);
			if (addr == 0) {
				printf("received broadcast frame\n");
			} else if (addr == self_addr) {
				printf("received frame to myself\n");
			} else {
				printf("received frame for address %.2x, ignoring\n", addr);
				goto ignore;
			}
			modbus_print_frame(&frame);
			process_frame(&frame);
		} else {
			printf("received corrupted frame (crc error)\n");
		}
ignore:
		modbus_free_frame(&frame);
	}
	return 0;
}
