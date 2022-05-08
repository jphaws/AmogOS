#include <stdint-gcc.h>
#include "vga.h"
#include "commonlib.h"
#include "inline.h"
#include <stdbool.h>

#define VGA_BASE 0xb8000
#define VGA_WIDTH 80

enum VGA_COLORS {VGA_BLACK, VGA_BLUE, VGA_GREEN, VGA_CYAN, VGA_RED, 
   VGA_MAGENTA, VGA_BROWN, VGA_LIGHT_GREY, VGA_DARK_GREY, VGA_LIGHT_BLUE, 
   VGA_LIGHT_GREEN, VGA_LIGHT_CYAN, VGA_LIGHT_RED, VGA_LIGHT_MAGENTA, VGA_YELLOW, 
   VGA_WHITE};

#define FG(fore) (fore) << (0)
#define BG(back) (back) << (4)


static uint16_t *vgaBuff = (unsigned short*)VGA_BASE;
static const int width = 80;
static const int height = 25;
static int cursor = 0;
static unsigned char color = FG(VGA_LIGHT_GREY) | BG(VGA_BLACK);

static void scroll(void);
static void update_cursor(int x, int y);

#define LINE(cur) (cur) / (width)

void VGA_clear(void){
   for (int i = 0; i < width * height; i++)
      VGA_display_char(' ');
   cursor = 0;
}

void VGA_display_char(char c){
   bool enable_ints = are_interrupts_enabled();
   if (enable_ints)
      CLI

   if (c == '\n') {
      cursor = (LINE(cursor) + 1) * width;
      if (cursor >= width*height)
         scroll(); 
   }
   else if (c == '\r')
      cursor = LINE(cursor);
   else if (c == '\b' && ((cursor % width) > 0) && cursor > 0){
      cursor--;
      vgaBuff[cursor] = (color << 8) | ' ';
   }
   else {
      vgaBuff[cursor] = (color << 8) | c;
      cursor++;
      if (cursor >= width * height)
         scroll();
   }

   if (enable_ints)
      STI
}

void VGA_display_str(const char *str){
   while (*str != '\0'){
      VGA_display_char(*str);
      ++str;
   }
}

void VGA_update_cursor(){
   update_cursor(cursor % VGA_WIDTH, cursor / VGA_WIDTH);
}


static void update_cursor(int x, int y){
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void scroll(){
   for (int i = 0; i < (width * (height - 1)); i++){
      vgaBuff[i] = vgaBuff[i + width];
   }

   for (int i = (width * (height - 1)); i < width * height; i++){
      vgaBuff[i] = ((FG(VGA_BLACK) | BG(VGA_BLACK)) << 8) | ' ';
   }
   cursor = width * (height - 1);
}
