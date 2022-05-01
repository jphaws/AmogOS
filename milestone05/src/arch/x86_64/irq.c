#include <stdint-gcc.h>
#include "isr.h"
#include "irq.h"
#include "inline.h"
#include "commonio.h"
#include "keyboard.h"

#define PIC1		0x20		/* IO base address for primary PIC */
#define PIC2		0xA0		/* IO base address for chained PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */
#define PIC_MASTER_REMAP 0x20
#define PIC_CHAINED_REMAP 0x28

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_CHAINED	0x08		/* Buffered mode/chained */
#define ICW4_BUF_PRIMARY	0x0C		/* Buffered mode/primary*/
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define IDT_TABLE_SIZE 256
#define IDT_INTERRUPT_GATE 0xE
#define GDT_OFFSET_KERNEL_CODE 0x8

struct IDT_Entry{
   uint16_t target_offset_lower; // Address of Interrupt Service Routine (3 parts)
   uint16_t target_selector;

   uint8_t ist:3;         // offset into the interrupt stack table
   uint8_t reserved_5:5;  // set to 0
                          //
   uint8_t type:4;        // 0xE = Interrupt Gate, 0xF = Trap Gate
   uint8_t zero:1;        // set to 0
   uint8_t dpl:2;         // 0 = kernel privelege
   uint8_t present:1;     // present = 1

   uint16_t target_offset_mid;

   uint32_t target_offset_upper;

   uint32_t reserved_32;
}__attribute__((packed));

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

extern int IRQ_get_mask(int irq);

extern void IRQ_set_mask(int irq);
extern void IRQ_clear_mask(int irq);
extern void IRQ_end_of_interrupt(int irq);
void PIC_remap(int offset1, int offset2);
void PIC_sendEOI(unsigned char irq);

typedef void (*irq_handler_t)(int, int, void*);
extern void IRQ_set_handler(int irq, irq_handler_t handler, void *arg);

static struct IDT_Entry IDT[IDT_TABLE_SIZE];
static idtr_t idtr;

extern void IRQ_init(void){
   CLI
   IRQ_set_mask(0);
   PIC_remap(PIC_MASTER_REMAP, PIC_CHAINED_REMAP);
   idtr.base = (uintptr_t)&IDT[0];
   idtr.limit = (sizeof(IDT) - 1);

   for (int i = 0; i < IDT_TABLE_SIZE; i++){
      struct IDT_Entry *descriptor = &IDT[i];
      uint64_t isr_ptr = (uint64_t)isr_fun_ptrs[i];
      descriptor->target_offset_lower = (uint64_t)isr_ptr & 0xFFFF;
      descriptor->target_selector = GDT_OFFSET_KERNEL_CODE;
      descriptor->ist = 0;
      descriptor->reserved_5 = 0;
      descriptor->type = IDT_INTERRUPT_GATE;
      descriptor->zero = 0;
      descriptor->dpl = 0;
      descriptor->present = 1;
      descriptor->target_offset_mid = ((uint64_t)isr_ptr >> 16) & 0xFFFF;
      descriptor->target_offset_upper = ((uint64_t)isr_ptr >> 32) & 0xFFFFFFFF;
      descriptor->reserved_32 = 0;
   }

   lidt(&IDT[0], sizeof(IDT) - 1);        // load the new IDT
   STI
}

void PIC_sendEOI(unsigned char irq){
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}


void IRQ_end_of_interrupt(int irq){
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);

	outb(PIC1_COMMAND,PIC_EOI);
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

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default 

arguments:
	offset1 - vector offset for primary PIC
		vectors on the primary become offset1..offset1+7
	offset2 - same for chained PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2){
	unsigned char a1, a2;

	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Primary PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Chained PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Primary PIC that there is a chained PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Chained PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

extern void exception_handler(int isr_num, int err_code) {
   if (isr_num == 0x21){
      keyboard_read();
   }
   else{
      printk("Encountered exception #%d (Error code 0x%x). :()\n", isr_num, err_code);
      __asm__ volatile ("cli; hlt"); // Completely hangs the computer
   }
   PIC_sendEOI(isr_num);
}

