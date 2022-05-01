#include <stdint-gcc.h>
#include "vga.h"
#include "commonlib.h"
#include "inline.h"
#include <stdbool.h>

#define VGA_BASE 0xb8000

enum VGA_COLORS {VGA_BLACK, VGA_BLUE, VGA_GREEN, VGA_CYAN, VGA_RED, 
   VGA_MAGENTA, VGA_BROWN, VGA_LIGHT_GREY, VGA_DARK_GREY, VGA_LIGHT_BLUE, 
   VGA_LIGHT_GREEN, VGA_LIGHT_CYAN, VGA_LIGHT_RED, VGA_LIGHT_MAGENTA, VGA_YELLOW, 
   VGA_WHITE};

#define FG(fore) (fore) << (0)
#define BG(back) (back) << (4)


static uint16_t *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;
static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);

void scroll(void);

#define LINE(cur) (cur) / (width)

void VGA_clear(void){
   for (int i = 0; i < width * height; i++)
      VGA_display_char(' ');
   cursor = 0;
}

void VGA_display_char(char c){
   bool enable_ints = 0;
   if (are_interrupts_enabled())
      STI

   if (c == '\n') {
      cursor = (LINE(cursor) + 1) * width;
      if (cursor >= width*height)
         scroll(); 
   }
   else if (c == '\r')
      cursor = LINE(cursor);
   else {
      vgaBuff[cursor] = (color << 8) | c;
      cursor++;
      if (cursor >= width * height)
         scroll();
   }

   if (enable_ints)
      CLI
}

void VGA_display_str(const char *str){
   while (*str != '\0'){
      VGA_display_char(*str);
      ++str;
   }
}

void scroll(){
   for (int i = 0; i < (width * (height - 1)); i++){
      vgaBuff[i] = vgaBuff[i + width];
   }

   for (int i = (width * (height - 1)); i < width * height; i++){
      vgaBuff[i] = ((FG(VGA_BLACK) | BG(VGA_BLACK)) << 8) | ' ';
   }
   cursor = width * (height - 1);
}
