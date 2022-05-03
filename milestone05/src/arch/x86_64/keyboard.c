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
#define KEYBOARD_SET_SCAN_CODE_SET 2
#define KEYBOARD_ENABLE_SCANNING 0xF4

#define MAX_SCAN_CODE 0x58

#define L_SHIFT_PRESSED 0x2A
#define L_SHIFT_RELEASED 0xAA
#define R_SHIFT_PRESSED 0x36
#define R_SHIFT_RELEASED 0xB6
#define CAPS_PRESSED 0x3A

typedef enum state_mod STATE_MOD;
enum state_mod{DEFAULT, SHIFT, CAPS, SHIFT_CAPS};

STATE_MOD state = DEFAULT;

char scan_codes[] = {
   '\0', '\0', '1', '2', 
   '3', '4', '5', '6', 
   '7', '8', '9', '0', 
   '-', '=', '\0', '\t', 
   'q', 'w', 'e', 'r', 
   't', 'y', 'u', 'i', 
   'o', 'p', '[', ']', 
   '\n', '\0', 'a', 's', 
   'd', 'f', 'g', 'h', 
   'j', 'k', 'l', ';', 
   '\'', '`', '\0', '\\', 
   'z', 'x', 'c', 'v', 
   'b', 'n', 'm', ',', 
   '.', '/', '\0', '*', 
   '\0', ' ', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '7', 
   '8', '9', '-', '4', 
   '5', '6', '+', '1', 
   '2', '3', '0', '.', 
   '\0', '\0', '\0', '\0', 
   '\0'
}; //support through f12

char scan_codes_shift[] = {
   '\0', '\0', '!', '@', 
   '#', '$', '%', '&', 
   '&', '*', '(', ')', 
   '_', '+', '\0', '\t', 
   'Q', 'W', 'E', 'R', 
   'T', 'Y', 'U', 'I', 
   'O', 'P', '{', '}', 
   '\n', '\0', 'A', 'S', 
   'D', 'F', 'G', 'H', 
   'J', 'K', 'L', ':', 
   '"', '~', '\0', '|', 
   'Z', 'X', 'C', 'V', 
   'B', 'N', 'M', '<', 
   '>', '?', '\0', '*', 
   '\0', ' ', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '7', 
   '8', '9', '-', '4', 
   '5', '6', '+', '1', 
   '2', '3', '0', '.', 
   '\0', '\0', '\0', '\0', 
   '\0'
};

char scan_codes_caps[] = {
   '\0', '\0', '1', '2', 
   '3', '4', '5', '6', 
   '7', '8', '9', '0', 
   '-', '=', '\0', '\t', 
   'Q', 'W', 'E', 'R', 
   'T', 'Y', 'U', 'I', 
   'O', 'P', '[', ']', 
   '\n', '\0', 'A', 'S', 
   'D', 'F', 'G', 'H', 
   'J', 'K', 'L', ';', 
   '\'', '`', '\0', '\\', 
   'Z', 'X', 'C', 'V', 
   'B', 'N', 'M', ',', 
   '.', '/', '\0', '*', 
   '\0', ' ', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '7', 
   '8', '9', '-', '4', 
   '5', '6', '+', '1', 
   '2', '3', '0', '.', 
   '\0', '\0', '\0', '\0', 
   '\0'
};

char scan_codes_shift_caps[] = {
   '\0', '\0', '!', '@', 
   '#', '$', '%', '&', 
   '&', '*', '(', ')', 
   '_', '+', '\0', '\t', 
   'q', 'w', 'e', 'r', 
   't', 'y', 'u', 'i', 
   'o', 'p', '{', '}', 
   '\n', '\0', 'a', 's', 
   'd', 'f', 'g', 'h', 
   'j', 'k', 'l', ':', 
   '\"', '~', '\0', '|', 
   'z', 'x', 'c', 'v', 
   'b', 'n', 'm', '<', 
   '>', '?', '\0', '*', 
   '\0', ' ', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '\0', 
   '\0', '\0', '\0', '7', 
   '8', '9', '-', '4', 
   '5', '6', '+', '1', 
   '2', '3', '0', '.', 
   '\0', '\0', '\0', '\0', 
   '\0'
};

void reset_keyboard(void){
   uint8_t test;

   // printk("Begin keyboard reset:\n");
   do{
      ps2_poll_write(PS2_DATA, KEYBOARD_RESET);

      // Receive ACK
      test = ps2_poll_read(PS2_DATA);
      // printk("   keyboard reset byte received: 0x%x\n", test);
   }while(test != KEYBOARD_ACK);

   // Receive success or failure byte
   test = ps2_poll_read(PS2_DATA);
   // printk("   keyboard reset byte received: 0x%x\n", test);

   if (test != KEYBOARD_RESET_SUCCESS){
      printk("Keyboard reset failed!\n");
      reset_keyboard();
   }
}

void set_scan_code(void){
   uint8_t response;
   
   // printk("Begin setting scan code 2\n");

   do {

      // printk("   enter scan code command\n");
      ps2_poll_write(PS2_DATA, KEYBOARD_SCAN_CODE);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

   do {

      // printk("   set scan code 2\n");
      ps2_poll_write(PS2_DATA, KEYBOARD_SET_SCAN_CODE_SET);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

   do {

      // printk("   enter scan code command\n");
      ps2_poll_write(PS2_DATA, KEYBOARD_SCAN_CODE);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

   do {

      // printk("   reading current scan code\n");
      ps2_poll_write(PS2_DATA, 0);

      response = ps2_poll_read(PS2_DATA);
   } while (response != KEYBOARD_ACK); 

   response = ps2_poll_read(PS2_DATA);
   // printk("   Keyboard is using scancode: 0x%x\n", response);

}

void enable_keyboard(void){
   uint8_t response;

   // printk("Begin enable keyboard scanning\n");

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
   reset_keyboard();
   set_scan_code();
   enable_keyboard();
   printk("Keyboard Initialized\n");
}

void modify_state(uint8_t code){
   switch(state){
      case SHIFT:
         if (code == L_SHIFT_RELEASED || code == R_SHIFT_RELEASED)
            state = DEFAULT;
         else if (code == CAPS_PRESSED)
            state = SHIFT_CAPS;
         break;
      case CAPS:
         if (code == L_SHIFT_PRESSED || code == R_SHIFT_PRESSED)
            state = SHIFT_CAPS;
         else if (code == CAPS_PRESSED)
            state = DEFAULT;
         break;
      case SHIFT_CAPS:
         if (code == CAPS_PRESSED)
            state = SHIFT;
         else if (code == L_SHIFT_RELEASED || code == R_SHIFT_RELEASED)
            state = CAPS;
         break;
      case DEFAULT:
         if (code == L_SHIFT_PRESSED || code == R_SHIFT_PRESSED)
            state = SHIFT;
         else if (code == CAPS_PRESSED)
            state = CAPS;
         break;
   }
}

void keyboard_read(int irq, int err, void *data){
   uint8_t code;
   code = ps2_poll_read(PS2_DATA);

   modify_state(code);

   switch(state){
      case SHIFT:
         if (code <= MAX_SCAN_CODE && scan_codes_shift[code] != '\0')
            printk("%c", scan_codes_shift[code]);
         break;
      case CAPS:
         if (code <= MAX_SCAN_CODE && scan_codes_caps[code] != '\0')
            printk("%c", scan_codes_caps[code]);
         break;
      case SHIFT_CAPS:
         if (code <= MAX_SCAN_CODE && scan_codes_shift_caps[code] != '\0')
            printk("%c", scan_codes_shift_caps[code]);
         break;
      case DEFAULT:
         if (code <= MAX_SCAN_CODE && scan_codes[code] != '\0')
            printk("%c", scan_codes[code]);
         break;
   }
}
