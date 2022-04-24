#include <stdint-gcc.h>
#include "commonio.h"
#include "ps2.h"

#define KEYBOARD_RESET 0xFF
#define KEYBOARD_ACK 0xFA
#define KEYBOARD_RESET_SUCCESS 0xAA

enum state{SHIFT, ALT, CTRL, CAPS};

void reset_keyboard(void){
   uint8_t test;

   printk("Begin keyboard reset:\n");
   do{
      ps2_poll_write_status();
      outb(PS2_DATA, KEYBOARD_RESET);

      // Receive ACK
      ps2_poll_read_status();
      test = inb(PS2_DATA);
      printk("   keyboard reset byte received: 0x%x\n", test);
   }while(test != KEYBOARD_ACK);

   // Receive success or failure byte
   ps2_poll_read_status();
   test = inb(PS2_DATA);
   printk("   keyboard reset byte received: 0x%x\n", test);

   if (test != KEYBOARD_RESET_SUCCESS){
      printk("Keyboard reset failed!\n");
      reset_keyboard();
   }
}

void initialize_keyboard(){
   printk("Begin keyboard initialization\n");
   reset_keyboard();

   printk("Finished keyboard initialization\n");
}
