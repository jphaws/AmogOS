#ifndef __GDT_H__
#define __GDT_H__

#include <stdint-gcc.h>

extern void initialize_gdt(void);
extern void initialize_tss(void);

extern uint8_t get_DF_stack_offset(void);
extern uint8_t get_GP_stack_offset(void);
extern uint8_t get_PF_stack_offset(void);

#endif
