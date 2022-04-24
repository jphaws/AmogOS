#ifndef __PS2_H__
#define __PS2_H__

static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
void initialize_PS2_controller(void);
void ps2_poll_write_status(void);
static void ps2_poll_read_status(void);

#endif
