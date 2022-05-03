#ifndef __IRQ_H__
#define __IRQ_H__

typedef void (*irq_handler_t)(int, int, void*);

extern void IRQ_init(void);
extern void IRQ_set_mask(int irq);
extern void IRQ_clear_mask(int irq);
extern int IRQ_get_mask(int IRQline);
extern void IRQ_end_of_interrupt(int irq);
extern void IRQ_set_handler(int irq, irq_handler_t handler, void *arg);

extern void exception_handler(int isr_num, int err_code);

#endif
