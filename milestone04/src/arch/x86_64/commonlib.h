#include <stddef.h>

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
const char *strchr(const char *s, int c);
char *strdup(const char *s);
void utoa(unsigned char value, char *str);
void itoa(int value, char *str);
void htoa(short value, char *str);
void hutoa(unsigned char value, char *str);
void lutoa(unsigned long value, char *str);
void ltoa(long value, char *str);
void itohex(unsigned int value, char *str);
void ltohex(unsigned long value, char *str);
