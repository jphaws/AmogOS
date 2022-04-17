#include <stdarg.h>
#include "vga.h"
#include "commonlib.h"

void print_percent(void);                             //done
void print_int(int i);                                //done
void print_uint(unsigned char);                      //done
void print_unsigned_hexadecimal(unsigned int);        //done
void print_char(char);                                //done
void print_pointer(void *);                           //done
void print_short(short);                              //done
void print_long(long);                                //done
void print_long_long(long long);                      //done
void print_str(const char *);                         //done
void print_ulong(unsigned long v);                    //done
void print_ushort(short h);                           //done
void print_ulong_long(long long v);                   //done
void print_xlong(unsigned long v);                    //done
void print_xshort(short h);                           //done
void print_xlong_long(unsigned long long v);          //done

int printk(const char *fmt, ...){
   size_t i = 0;
   va_list va;
   va_start(va, fmt);

   while(fmt[i] != '\0'){
      if (fmt[i] == '%'){
         if (fmt[i + 1] == '%')
            print_percent();
         else if (fmt[i + 1] == 'd'){
            int int_val = va_arg(va, int);
            print_int(int_val);
         }
         else if (fmt[i + 1] == 'u'){
            unsigned int uint_val = va_arg(va, unsigned int);
            print_uint(uint_val);
         }
         else if (fmt[i + 1] == 'x'){
            unsigned int int_val = va_arg(va, unsigned int);
            print_unsigned_hexadecimal(int_val);
         }
         else if (fmt[i + 1] == 'c'){
            char char_val = (char)va_arg(va, int);
            print_char(char_val);
         }
         else if (fmt[i + 1] == 'p'){
            void *ptr_val = va_arg(va, void*);
            print_pointer(ptr_val);
         }
         else if (fmt[i + 1] == 'h'){
            i++;
            if (fmt[i + 1] == 'd'){
               short short_val = (short)va_arg(va, int);
               print_short(short_val);
            }
            else if (fmt[i + 1] == 'u'){
               unsigned short short_val = (unsigned short)va_arg(va, int);
               print_ushort(short_val);
            }
            else if (fmt[i + 1] == 'x'){
               unsigned short short_val = (unsigned short)va_arg(va, int);
               print_xshort(short_val);
            }
         }
         else if (fmt[i + 1] == 'l'){
            i++;
            if (fmt[i + 1] == 'd'){
               long long_val = va_arg(va, long);
               print_long(long_val);
            }
            else if (fmt[i + 1] == 'u'){
               unsigned long long_val = va_arg(va, unsigned long);
               print_ulong(long_val);
            }
            else if (fmt[i + 1] == 'x'){
               unsigned long long_val = va_arg(va, unsigned long);
               print_xlong(long_val);
            }
         }
         else if (fmt[i + 1] == 'q'){
            i++;
            if (fmt[i + 1] == 'd'){
               long long long_val = va_arg(va, long long);
               print_long_long(long_val);
            }
            else if (fmt[i + 1] == 'u'){
               unsigned long long long_val = va_arg(va, unsigned long long);
               print_ulong_long(long_val);
            }
            else if (fmt[i + 1] == 'x'){
               unsigned long long long_val = va_arg(va, unsigned long long);
               print_xlong_long(long_val);
            }
         }
         else if (fmt[i + 1] == 's'){
            char *str_val = va_arg(va, char*);
            print_str(str_val);
         }
         ++i;
      }
      else{
         VGA_display_char(fmt[i]);
      }
      i++;
   }
   va_end(va);
   return i;
}


/* 
 * Prints %% format
 */
void print_percent(void){
   VGA_display_char('%');
}

/* 
 * Prints %d format
 */
void print_int(int i){
   char str[12];           // -2147483648 -> 2147483647
   itoa(i, str);
   VGA_display_str((const char*)str);
}

/* 
 * Prints %u format
 */
void print_uint(unsigned char u){
   char str[11];
   utoa(u, str);
   VGA_display_str((const char*)str);
}


/* 
 * Prints %x format
 */
void print_unsigned_hexadecimal(unsigned int x){
   char str[9];         // 0x0 -> 0xFFFFFFFF (doesn't store 0x)
   itohex(x, str);
   VGA_display_str(str);
}

/* 
 * Prints %c format
 */
void print_char(char c){
   VGA_display_char(c);
}

/* 
 * Prints %p format
 */
void print_pointer(void *ptr){
   char str[19];         // 0x0 -> 0xFFFFFFFF
   str[0] = '0';
   str[1] = 'x';
   ltohex((unsigned long)&ptr, str+2);
   VGA_display_str(str);
}

/* 
 * Prints %hd format
 */
void print_short(short h){
   char str[7];            // -32768 -> 32767
   htoa(h, str);
   VGA_display_str((const char*) str);
}

/* 
 * Prints %hu format
 */
void print_ushort(short h){
   char str[7];            // 0 -> 65535
   hutoa(h, str);
   VGA_display_str((const char*) str);
}

/* 
 * Prints %hx format
 */
void print_xshort(short h){
   char str[5];            // 0 -> 0xFFFF
   hutoa(h, str);
   VGA_display_str((const char*) str);
}

/* 
 * Prints %ld format
 */
void print_long(long v){
   char str[21];           // -9223372036854775808 -> 9223372036854775807 	
   ltoa(v, str);
   VGA_display_str((const char*) str);
}

/* 
 * Prints %lu format
 */
void print_ulong(unsigned long v){
   char str[21];           // 0 -> 18446744073709551615 	
   lutoa(v, str);
   VGA_display_str((const char*) str);
}

/* 
 * Prints %lx format
 */
void print_xlong(unsigned long v){
   char str[17];           // 0 -> 0xFFFFFFFFFFFFFFFF
   ltohex(v, str);
   VGA_display_str((const char*) str);
}


/* 
 * Prints %qd format
 */
void print_long_long(long long v){
   print_long((long) v);
}

/* 
 * Prints %qu format
 */
void print_ulong_long(long long v){
   print_ulong((long) v);
}

/* 
 * Prints %qx format
 */
void print_xlong_long(unsigned long long v){
   print_xlong((long) v);
}

/* 
 * Prints %s format
 */
void print_str(const char *s){
   VGA_display_str(s);
}
