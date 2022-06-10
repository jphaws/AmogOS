#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint-gcc.h>

#define PAGE_SIZE 4096
extern void *MMU_pf_alloc(void);
extern void MMU_pf_free(void *pf);
extern void update_starts(uint64_t addr, uint64_t length);
extern void update_avoids(uint64_t addr, uint64_t size);
extern void initialize_pf();

#endif
