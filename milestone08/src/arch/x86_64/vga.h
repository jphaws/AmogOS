#ifndef __VGA_H__
#define __VGA_H__

void VGA_clear(void);
void VGA_display_char(char);
void VGA_display_str(const char *);
void VGA_update_cursor();

#endif
