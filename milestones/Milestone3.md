# Milestone 3

## VGA Text Mode and printk
The objective of this milestone is to actually be able to produce some form of debugging output from your kernel. This involves two overarching steps:

  1. Pass control from assembly to C!
      - Create a new C source file with a "kernel_main" or "kmain" function.
      - Make sure the kmain function never returns (e.g., call HLT in an infinite loop).
      - Compile your new .c file into an object (.o) file with your cross-compiler. Use the -c and -g options. Add it to your makefile.
      - In your long mode asm file, declare the kmain function as extern with the line "extern kmain"
      - In your long mode asm file, use the "call" instruction to call your kmain function.
      - Link your kmain.o object file with all the other object files to form your kernel. Add it to your makefile.
      - Now you can write most of the rest of your kernel in C!
  2. VGA Console Driver. There are many ways to design your VGA API, but at a minimum you need to support functionally equivalent to the following:

      ```C
      extern void VGA_clear(void);
      extern void VGA_display_char(char);
      extern void VGA_display_str(const char *);
      ```

      The VGA console is memory-mapped at address 0xb8000. Each character is stored as one 16 bit value in [row-major order](https://en.wikipedia.org/wiki/Row-_and_column-major_order). The entire screen is 80x25.

      Refer to the [Wikipedia article on VGA-compatible text mode](https://en.wikipedia.org/wiki/VGA-compatible_text_mode) for details on displaying a particular character at a particular spot on the screen. You will need to manually scroll the text as well as return the cursor to the beginning of the line when you encounter a newline character.

      You may find is useful to implement some of the basic string and memory manipulation functions from the C standard library:

      ```C
      extern void *memset(void *dst, int c, size_t n);
      extern void *memcpy(void *dest, const void *src, size_t n);
      extern size_t strlen(const char *s);
      extern char *strcpy(char *dest, const char *src);
      extern int strcmp(const char *s1, const char *s2);
      extern const char *strchr(const char *s, int c);
      extern char *strdup(const char *s);
      ```

  3. Call VGA_display_char() from kmain to test it!
  4. Implement printk, which works just like printf except inside the kernel. The full set of format specifiers you support is up to you, but you must support:

      ```C
      %% %d %u %x %c %p %h[dux] %l[dux] %q[dux] %s
      ```

      If you include the gcc specific printf format attribute in your function declaration you will get all the warnings about incorrect format specifiers (I strongly suggest doing this):

      ```C
      extern int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
      ```

      Lastly, I suggest creating individual functions to print each type of data and let printk focus on parsing the format string. For example:

      ```C
      extern void print_char(char);
      extern void print_str(const char *);
      extern void print_uchar(unsigned char);
      extern void print_short(short);
      extern void print_long_hex(long);
      ```
