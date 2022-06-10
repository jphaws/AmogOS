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
#define PF_INT_NUM 0xE

void *addrs[40000];

void kmain(void *multiboot_ptr){
   int spin = DEBUG;

   SER_init();

   initialize_PS2_controller();
   IRQ_set_handler(KB_INT_NUM, keyboard_read, (void*)0);
   initialize_keyboard();
   initialize_gdt();
   initialize_tss();
   initialize_multiboot(multiboot_ptr);
   init_virtual_memory();
   IRQ_set_handler(SERIAL_INT_NUM, SER_handler, (void*)0);
   IRQ_set_handler(PF_INT_NUM, PF_handler, (void*)0);
   IRQ_init();

   printk("+-----------------------------------------------------------------------------+\n");
   printk("|                                 Welcome to                                  |\n");
   printk("|                     _                           ___  ____                   |\n");
   printk("|                    / \\   _ __ ___   ___   __ _ / _ \\/ ___|                  |\n");
   printk("|                   / _ \\ | '_ ` _ \\ / _ \\ / _` | | | \\___ \\                  |\n");
   printk("|                  / ___ \\| | | | | | (_) | (_| | |_| |___) |                 |\n");
   printk("|                 /_/   \\_|_| |_| |_|\\___/ \\__, |\\___/|____/                  |\n");
   printk("|                                          |___/                              |\n");
   printk("+-----------------------------------------------------------------------------+\n");

   while(spin);

   uint64_t *v = (uint64_t*)MMU_alloc_pages(32133);
   void *o = (void*)v;
   uint64_t inc = 0;
   while (v != NULL){
      inc++;
      printk("ptr: %p\n", v);
      for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++){
         v[i] = inc;
      }
      v = (uint64_t*)MMU_alloc_page();
   }

   // printk("ptr: %p\n", o);
   printk("Virtual Pages: %lu\n", inc);
   MMU_free_pages(o, 1000);

   v = (uint64_t*)MMU_alloc_page();
   while (v != NULL){
      inc++;
      // printk("ptr: %p\n", v);
      for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++){
         v[i] = inc;
      }
      v = (uint64_t*)MMU_alloc_page();
   }

   printk("Test 1 complete! Stress tested virtual memory allocator\n");

   uint8_t *stack = (void*)0x800000000;
   stack[0x2000] = 5;
   stack[0x20000] = 5;

   printk("Test 2 complete! Written to kernel stack area\n");


   while(1){
      __asm__ volatile("hlt");
   }
}

