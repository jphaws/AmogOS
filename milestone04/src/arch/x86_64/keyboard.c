#include <stdint-gcc.h>
#include "commonio.h"
#include "ps2.h"

#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD

#define KEYBOARD_RESET 0xFF
#define KEYBOARD_ACK 0xFA
#define KEYBOARD_RESET_SUCCESS 0xAA
#define KEYBOARD_SCAN_CODE 0xF0
#define KEYBOARD_SET_SCAN_CODE_2 2
#define KEYBOARD_GET_SCAN_CODE_2 0x41
#define KEYBOARD_ENABLE_SCANNING 0xF4

enum state{SHIFT, ALT, CTRL, CAPS};

void reset_keyboard(void){
   uint8_t test;

   printk("Begin keyboard reset:\n");
   do{
      ps2_poll_write(PS2_DATA, KEYBOARD_RESET);

      // Receive ACK
      test = ps2_poll_read(PS2_DATA);
      printk("   keyboard reset byte received: 0x%x\n", test);
   }while(test != KEYBOARD_ACK);

   // Receive success or failure byte
   test = ps2_poll_read(PS2_DATA);
   printk("   keyboard reset byte received: 0x%x\n", test);

   if (test != KEYBOARD_RESET_SUCCESS){
      printk("Keyboard reset failed!\n");
      reset_keyboard();
   }
}

void set_scan_code(void){
   uint8_t response;
   
   printk("Begin setting scan code 2\n");

   do {

      printk("   enter scan code command\n");
      ps2_poll_write(PS2_DATA, KEYBOARD_SCAN_CODE);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

   do {

      printk("   set scan code 2\n");
      ps2_poll_write(PS2_DATA, KEYBOARD_SET_SCAN_CODE_2);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

}

void enable_keyboard(void){
   uint8_t response;

   printk("Begin enable keyboard scanning\n");

   do {
      ps2_poll_write(PS2_DATA, KEYBOARD_ENABLE_SCANNING);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK);
}

/*
 * Entry function for initializing a PS2 keyboard. 
 * PS2 controller must be initialized before calling.
 * See the OS Dev Wiki for more information
 */
void initialize_keyboard(void){
   printk("Begin keyboard initialization\n");
   reset_keyboard();
   set_scan_code();
   enable_keyboard();
   printk("Finished keyboard initialization\n");
}

void keyboard_poll(void){

}
