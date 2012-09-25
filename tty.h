#pragma once

#include <stdint.h>
#include <stddef.h>

int tty_open(const char *tty_dev);
void tty_write(int fd, const uint8_t *buf, size_t len);
