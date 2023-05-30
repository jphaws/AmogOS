#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SER_PORT_COM1   0x3f8
#define SER_PORT_IER    SER_PORT_COM1 + 1    // Interrupt Enable register
#define SER_PORT_IIR    SER_PORT_COM1 + 2
#define SER_PORT_LSR    SER_PORT_COM1 + 5    // Line Status register

#define IER_TXE         0x2      // Transmitter empty 
                                 
#define IIR_LINE_INT    0x6      // Line status change
#define IIR_TX_INT      0x2      // Transmitter holding register empty	IIR read or THR write

#define LSR_THRE        0x20     // Set if the transmission buffer is empty (i.e. data can be sent) 
#define LSR_TEMT        0x40     // Set if the transmitter is not doing anything 


extern void SER_init(void);
extern int SER_write(const char* str, int len);
extern void SER_handler(int interrupt_num, int error_code, void *arg);


#endif
