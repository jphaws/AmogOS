#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

void initialize_keyboard(void);
void keyboard_read(int irq, int err, void *data);

#endif
