#include "irq.h"
#include "inline.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for chained PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

extern int IRQ_get_mask(int irq);
extern void IRQ_end_of_interrupt(int irq);

typedef void (*irq_handler_t)(int, int, void*);
extern void IRQ_set_handler(int irq, irq_handler_t handler, void *arg);

extern void IRQ_init(void){
   CLI

   STI
}

extern void IRQ_set_mask(int irq) {
    uint16_t port;
    uint8_t value;

    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

extern void IRQ_clear_mask(int irq) {
    uint16_t port;
    uint8_t value;

    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}
