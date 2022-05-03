#include "commonio.h"
#include "ps2.h"
#include "keyboard.h"
#include "irq.h"
#include "gdt.h"

#define DEBUG 0
#define KB_INT_NUM 0x21

void kmain(void){
   while(DEBUG);

   initialize_PS2_controller();
   initialize_keyboard();
   initialize_gdt();
   initialize_tss();
   IRQ_init();
   IRQ_set_handler(KB_INT_NUM, keyboard_read, (void*)0);

   printk("+------------------------------------------------------------------------------+");
   printk("|                             Welcome to BellardOS                             |");
   printk("+------------------------------------------------------------------------------+");

   //asm volatile ("int %0"::"N"(0));

   while(1){
      for (int i = 0; i < 400; i++){
         for (int j = 0; j < 1000000; j++){
            ;
         }
      }
      printk(".");
   }
}

