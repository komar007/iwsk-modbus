#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

struct config {
	int fd;
	int baud;
	int byte_interval;
	int byte_interval_limit;
};

struct frame {
	size_t len;
	uint8_t *data;
};

uint16_t modbus_crc(const uint8_t *data, size_t len);
void modbus_make_frame(struct frame *frame, uint8_t addr, uint8_t func,
		size_t data_len, const uint8_t *data);
int modbus_send_frame(struct config *cfg, struct frame *frame);
int modbus_open(struct config *cfg, const char *dev, int baud,
		int byte_interval, int byte_interval_limit);
int modbus_recv_frame(struct config *cfg, struct frame *frame, int timeout);
void modbus_print_frame(struct frame *frame);
bool modbus_frame_correct(struct frame *frame);
uint8_t modbus_get_frame_addr(struct frame *frame);
uint8_t modbus_get_frame_func(struct frame *frame);
uint8_t *modbus_get_frame_payload(struct frame *frame);
size_t modbus_get_frame_payload_len(struct frame *frame);
void modbus_free_frame(struct frame *frame);
