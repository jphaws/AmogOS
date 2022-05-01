#include "vga.h"
#include "commonlib.h"
#include "commonio.h"
#include "ps2.h"
#include "keyboard.h"
#include "irq.h"
#include "gdt.h"

#define ZERO 0

void kmain(void){
   int run = ZERO;
   // int zero = ZERO;
   while(run);

   initialize_gdt();
   // run = run/zero;
   //asm volatile ("int %0"::"N"(0));

   initialize_PS2_controller();
   initialize_keyboard();

   IRQ_init();
   while(1){
   }
}

