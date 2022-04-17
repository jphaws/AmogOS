#include <stddef.h>
#include <stdint-gcc.h>
#include "commonlib.h"


void *memset(void *dst, int c, size_t n){
   uint8_t *reset = (uint8_t *)dst;

   for (int i = 0; i < n; i++)
      reset[i] = c;

   return dst;
}

void *memcpy(void *dest, const void *src, size_t n){
   uint8_t *old = (uint8_t *)src;
   uint8_t *new = (uint8_t *)dest;

   for (size_t i = 0; i < n; i++)
      new[i] = old[i];

   return dest;
}

size_t strlen(const char *s){
   size_t len = 0;

   for (size_t i = 0; i < SIZE_MAX; i++){
      if (s[i] == '\0'){
         len = i;
         break;
      }
   }

   return len;
}

char *strcpy(char *dest, const char *src){
   for (size_t i = 0; i < SIZE_MAX; i++){
      dest[i] = src[i];
      if (src[i] == '\0')
         break;
   }

   return dest;
}

int strcmp(const char *s1, const char *s2);

const char *strchr(const char *s, int c);

char *strdup(const char *s);

/*
 * Converts an int to a string
 */
void itoa(int value, char *str){
   int digits = 12;     // -2147483648 -> 2147483647
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   if (value < 0){
      itoa(value * -1, str+1);
      str[0] = '-';

   }
   else{
      for (int i = 1; value > 0 && i < digits; i++){
         rev[i] = value % 10;
         value /= 10;
         len++;
      }

      for (int i = 0; i < len; i++)
         str[i] = '0' + rev[len - i];
      str[len] = '\0';
   }
}

/*
 * Converts a long to a string
 */
void ltoa(long value, char *str){
   int digits = 21;     // -9223372036854775808 -> 9223372036854775807 	
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   if (value < 0){
      ltoa(value * -1, str+1);
      str[0] = '-';

   }
   else{
      for (int i = 1; value > 0 && i < digits; i++){
         rev[i] = value % 10;
         value /= 10;
         len++;
      }

      for (int i = 0; i < len; i++)
         str[i] = '0' + rev[len - i];
      str[len] = '\0';
   }
}

/*
 * Converts an unsigned long to a string
 */
void lutoa(unsigned long value, char *str){
   int digits = 21;     // 0 -> 18446744073709551615
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   for (int i = 1; value > 0 && i < digits; i++){
      rev[i] = value % 10;
      value /= 10;
      len++;
   }

   for (int i = 0; i < len; i++)
      str[i] = '0' + rev[len - i];
   str[len] = '\0';
}

/* 
 * Converts an unsigned int to a string
 */
void utoa(unsigned char value, char *str){
   int digits = 11; // 0 -> 256
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   for (int i = 1; value > 0 && i < digits; i++){
      rev[i] = value % 10;
      value /= 10;
      len++;
   }

   for (int i = 0; i < len; i++)
      str[i] = '0' + rev[len - i];
   str[len] = '\0';
}

/* 
 * Converts an unsigned short to a string
 */
void hutoa(unsigned char value, char *str){
   int digits = 6; // 0-> 65535
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   for (int i = 1; value > 0 && i < digits; i++){
      rev[i] = value % 10;
      value /= 10;
      len++;
   }

   for (int i = 0; i < len; i++)
      str[i] = '0' + rev[len - i];
   str[len] = '\0';
}

/*
 * Converts a short to a string
 */
void htoa(short value, char *str){
   int digits = 7;     // -32768 -> 32767
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   if (value < 0){
      htoa(value * -1, str+1);
      str[0] = '-';

   }
   else{
      for (int i = 1; value > 0 && i < digits; i++){
         rev[i] = value % 10;
         value /= 10;
         len++;
      }

      for (int i = 0; i < len; i++)
         str[i] = '0' + rev[len - i];
      str[len] = '\0';
   }
}

/*
 * Converts an unisgned int to hex string 
 */
void itohex(unsigned int value, char *str){
   int digits = 9; // 0x0 -> 0xFFFFFFFF
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   for (int i = 1; value > 0 && i < digits; i++){
      rev[i] = value % 16;
      value /= 16;
      len++;
   }

   for (int i = 0; i < len; i++){
      if (rev[len -i] < 10)
         str[i] = '0' + rev[len - i];
      else 
         str[i] = 'a' + (rev[len - i] - 10);
   }
   str[len] = '\0';
}

/*
 * Converts an unisgned long to hex string 
 */
void ltohex(unsigned long value, char *str){
   int digits = 17; // 0x0 -> 0xFFFFFFFFFFFFFFFF
   char rev[digits];
   rev[0] = '\0';
   int len = 0;

   if (value == 0){
      str[0] = '0';
      str[1] = '\0';
      return;
   }

   for (int i = 1; value > 0 && i < digits; i++){
      rev[i] = value % 16;
      value /= 16;
      len++;
   }

   for (int i = 0; i < len; i++){
      if (rev[len -i] < 10)
         str[i] = '0' + rev[len - i];
      else 
         str[i] = 'a' + (rev[len - i] - 10);
   }
   str[len] = '\0';
}
