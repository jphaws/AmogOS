#ifndef __PS2_H__
#define __PS2_H__

#include <stdint-gcc.h>

void initialize_PS2_controller(void);
uint8_t ps2_poll_read(uint16_t port);
int8_t ps2_read(uint16_t port);
void ps2_poll_write(uint16_t port, uint8_t val);

#endif
