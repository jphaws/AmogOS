#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <stdint-gcc.h>

extern void initialize_multiboot(void *multiboot_ptr);
extern uint64_t get_high_memory();

#endif
