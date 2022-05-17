#include <stdbool.h>
#include <stdint-gcc.h>
#include "serial.h"
#include "boundedbuffer.h"
#include "inline.h"
#include "commonio.h"


extern void SER_init(void);
extern int SER_write(const char *buff, int len);
extern void SER_handler(int interrupt_num, int error_code, void *arg);
static void config_hw(void);
static void start_tx(void);
static bool buff_full(void);
static bool buff_empty(void);
static uint8_t tx_idle(void);

static struct State state;


extern void SER_init(void){
   BB_init(&state);
   config_hw();
   printk("Serial Initialized\n");
}

// Called when ISR 0x24 is registered
extern void SER_handler(int interrupt_num, int error_code, void *arg){
   uint8_t interrupt = inb(SER_PORT_IIR);
   
   if ((interrupt & IIR_LINE_INT) == IIR_LINE_INT)
      inb(SER_PORT_LSR);

   else if ((interrupt & IIR_TX_INT) == IIR_TX_INT)
      start_tx();
   // printk("Serial interrupt\n");
}

extern int SER_write(const char *buff, int len){
   int i = 0;
   bool interrupts = are_interrupts_enabled();
   if (interrupts)
      CLI

   while (! buff_full() && i < len)
      BB_enqueue(&state, buff[i++]);

   if (! buff_empty() && tx_idle())
      start_tx();

   if (interrupts)
      STI
   return i;
}

// Returns true if buff is full
static bool buff_full(void){
   if (state.producer == state.consumer - 1 || 
         (state.consumer == &state.buff[0] && state.producer == &state.buff[BUF_SIZE - 1])){
      return true;
   }
   return false;
}

// Returns true if buff is empty
static bool buff_empty(void){
   return state.consumer == state.producer;
}

/*
// Checks if hardware buffer is empty
static uint8_t tx_empty(void){
   return inb(SER_PORT_LSR) & LSR_THRE;
}
*/

// Checks if hardware is idle
static uint8_t tx_idle(void){
   return inb(SER_PORT_LSR) & LSR_TEMT;
}

static void start_tx(void){
   bool interrupts = are_interrupts_enabled();
   if (interrupts)
      CLI
   // buffer is empty
   if (state.consumer == state.producer){
      if (interrupts)
         STI
      return;
   }

   if (tx_idle()){
      char c = BB_dequeue(&state);
      outb(SER_PORT_COM1, c);
   }
   // update_config()??
   if (interrupts)
      STI
}
 
static void config_hw(void) {
   outb(SER_PORT_COM1 + 1, 0x00);    // Disable all interrupts
   outb(SER_PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(SER_PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud 
   outb(SER_PORT_COM1 + 1, 0x00);    //                  (hi byte)
   outb(SER_PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit (DLAB disabled)
   outb(SER_PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(SER_PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(SER_PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(SER_PORT_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(SER_PORT_COM1 + 0) != 0xAE)
      printk("Serial Initialization FAILED\n");
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#2 bits enabled)
   outb(SER_PORT_COM1 + 4, 0x0B);
   outb(SER_PORT_IER, IER_TXE);      // Enable interrupts for transmitter empty
}
