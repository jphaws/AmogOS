#ifndef __COMMONIO_H__
#define __COMMONIO_H__

int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
void print_uchar(unsigned char);
void print_int(int);
void print_short(short);
void print_long(long);
void print_unsigned_hexadecimal(unsigned int);
void print_pointer(void *ptr);

#endif
