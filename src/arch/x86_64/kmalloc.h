#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <stddef.h>

extern void kfree(void *addr);
extern void *kmalloc(size_t size);
extern void init_kmalloc(void);

#endif
