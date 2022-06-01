#ifndef __VMEM_H__
#define __VMEM_H__

extern void init_virtual_memory();
extern void *MMU_alloc_page();
extern void *MMU_alloc_pages(int num);
extern void MMU_free_page(void *);
extern void MMU_free_pages(void *, int num);

#endif
