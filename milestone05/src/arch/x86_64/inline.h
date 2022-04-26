#ifndef __INLINE_H__
#define __INLINE_H__

#include <stdint-gcc.h>

#define CLI __asm__ volatile ("cli");        // used to disable interrupts
#define STI __asm__ volatile ("sti");        // used to enable interrupts

static inline void lgdt(void *base, uint16_t size){
   extern struct {
      uint16_t length;
      void *base;
   } __attribute__ ((packed)) GDTR;

   GDTR.length = size;
   GDTR.base = base;

   asm ("lgdt %0" : : "m"(GDTR));
}

static inline void outb(uint16_t port, uint8_t val){
   asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port){
   uint8_t ret;
   asm volatile ("inb %1, %0"
                     : "=a"(ret)
                     : "Nd"(port));
   return ret;
}

static inline void io_wait(void){
    outb(0x80, 0);
}
                                             //
#endif
