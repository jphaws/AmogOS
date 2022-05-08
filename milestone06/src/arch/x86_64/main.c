#include <stdint-gcc.h>
#include "commonio.h"
#include "ps2.h"
#include "keyboard.h"
#include "irq.h"
#include "gdt.h"
#include "serial.h"
#include "inline.h"

#define DEBUG 0
#define KB_INT_NUM 0x21
#define SERIAL_INT_NUM 0x24

void kmain(void){
   int spin = DEBUG;
   while(spin);

   SER_init();
   initialize_PS2_controller();
   IRQ_set_handler(KB_INT_NUM, keyboard_read, (void*)0);
   initialize_keyboard();
   initialize_gdt();
   initialize_tss();
   IRQ_set_handler(SERIAL_INT_NUM, SER_handler, (void*)0);
   IRQ_init();


   printk("+------------------------------------------------------------------------------+");
   printk("|                                  Welcome to                                  |");
   printk("|                      _                           ___  ____                   |");
   printk("|                     / \\   _ __ ___   ___   __ _ / _ \\/ ___|                  |");
   printk("|                    / _ \\ | '_ ` _ \\ / _ \\ / _` | | | \\___ \\                  |");
   printk("|                   / ___ \\| | | | | | (_) | (_| | |_| |___) |                 |");
   printk("|                  /_/   \\_|_| |_| |_|\\___/ \\__, |\\___/|____/                  |");
   printk("|                                           |___/                              |");
   printk("+------------------------------------------------------------------------------+");

   //asm volatile ("int %0"::"N"(SERIAL_INT_NUM));
   SER_write("abcdefghijklmnopqrstuvwxyz", 26);

   while(1){
      __asm__ volatile("hlt");
   }
}

