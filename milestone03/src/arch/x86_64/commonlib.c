#include <stddef.h>
#include <stdint.h>
#include "commonlib.h"

void *memset(void *dst, int c, size_t n){
   uint8_t *reset = (uint8_t *)dst;

   if (dst == NULL)
      return NULL;

   for (int i = 0; i < n; i++)
      reset[i] = c;

   return dst;
}

void *memcpy(void *dest, const void *src, size_t n){
   uint8_t *old = (uint8_t *)src;
   uint8_t *new = (uint8_t *)dest;
   if (dest == NULL || src == NULL || n == 0)
      return dest;

   for (size_t i = 0; i < n; i++)
      new[i] = old[i];

   return dest;
}

size_t strlen(const char *s){
   size_t len = 0;

   if (s == NULL)
      return 0;

   for (size_t i = 0; i < SIZE_MAX; i++){
      if (s[i] == '\0')
         len = i;
         break;
   }

   return len;
}

char *strcpy(char *dest, const char *src){
   if (dest == NULL || src == NULL){
      return (char *)src;
   }

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

int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
