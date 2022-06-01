#include <stdint-gcc.h>
#include <stddef.h>
#include "commonio.h"
#include "ps2.h"
#include "keyboard.h"
#include "irq.h"
#include "gdt.h"
#include "serial.h"
#include "inline.h"
#include "multiboot.h"
#include "pf.h"
#include "vmem.h"

#define DEBUG 0
#define KB_INT_NUM 0x21
#define SERIAL_INT_NUM 0x24

void *addrs[40000];

void kmain(void *multiboot_ptr){
   int spin = DEBUG;

   SER_init();

   initialize_PS2_controller();
   IRQ_set_handler(KB_INT_NUM, keyboard_read, (void*)0);
   initialize_keyboard();
   initialize_gdt();
   initialize_tss();
   IRQ_set_handler(SERIAL_INT_NUM, SER_handler, (void*)0);
   IRQ_init();
   initialize_multiboot(multiboot_ptr);
   while(spin);

   printk("+-----------------------------------------------------------------------------+\n");
   printk("|                                 Welcome to                                  |\n");
   printk("|                     _                           ___  ____                   |\n");
   printk("|                    / \\   _ __ ___   ___   __ _ / _ \\/ ___|                  |\n");
   printk("|                   / _ \\ | '_ ` _ \\ / _ \\ / _` | | | \\___ \\                  |\n");
   printk("|                  / ___ \\| | | | | | (_) | (_| | |_| |___) |                 |\n");
   printk("|                 /_/   \\_|_| |_| |_|\\___/ \\__, |\\___/|____/                  |\n");
   printk("|                                          |___/                              |\n");
   printk("+-----------------------------------------------------------------------------+\n");

   //asm volatile ("int %0"::"N"(SERIAL_INT_NUM));
   void *p = MMU_pf_alloc();
   printk("Aloc: %p\n", p);
   MMU_pf_free(p);
   printk("Free: %p\n\n", p);

   void *a = MMU_pf_alloc();
   printk("Aloc: %p\n", a);

   void *b = MMU_pf_alloc();
   printk("Aloc: %p\n", b);

   void *c = MMU_pf_alloc();
   printk("Aloc: %p\n", c);
   MMU_pf_free(c);
   printk("Free: %p\n\n", c);

   void *d = MMU_pf_alloc();
   printk("Aloc: %p\n", d);
   MMU_pf_free(d);
   printk("Free: %p\n\n", d);

   MMU_pf_free(b);
   MMU_pf_free(a);
   printk("Free: %p\n", b);
   printk("Free: %p\n\n", a);


   printk("Test 1 passed!\n");

   p = MMU_pf_alloc();
   uint64_t inc = 0;
   while (p != NULL){
      uint64_t* np = (uint64_t*)p;
      addrs[inc++] = p;
      for (int i = 0; i < 4096 / sizeof(uint64_t); i++){
         np[i] = (uint64_t)p;
      }
      p = MMU_pf_alloc();
   }

   for (int i = 0; i < inc; i++){
      uint64_t* np = (uint64_t*)addrs[i];
      for (int j = 0; j < 4096 / sizeof(uint64_t); j++){
         if (np[j] != (uint64_t)addrs[i])
            printk("Expected: 0x%lx Got: 0x%lx @ %p\n", (uint64_t)addrs[i], np[j], addrs[i]);
      }
      MMU_pf_free(addrs[i]);
   }

   printk("Test 2 passed!\n");

   init_virtual_memory();

   while(1){
      __asm__ volatile("hlt");
   }
}

