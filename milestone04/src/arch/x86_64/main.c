#include "vga.h"
#include "commonlib.h"
#include "commonio.h"
#include "ps2.h"
#include "keyboard.h"

void kmain(){
   initialize_PS2_controller();
   initialize_keyboard();
   while(1)
      keyboard_read();
}

