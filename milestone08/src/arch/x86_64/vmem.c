#include "vmem.h"
#include "pf.h"
#include "commonlib.h"
#include <stdint-gcc.h>

struct PT4_entry{
    uint64_t present:1;
    uint64_t rw:1;               // read/write
    uint64_t us:1;               // user/supervisor
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign:1;              // ignored
    uint64_t mbz:2;              // must be zero
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p3_addr:40;
    uint64_t available:11;       // unused bits
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L4{
    struct PT4_entry entries[512];    
};
struct PT3_entry{
    uint64_t present:1;
    uint64_t rw:1;            // read/write
    uint64_t us:1;            // user/supervisor
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign:1;           // ignored
    uint64_t big_page:1;      // set 0?
    uint64_t mbz:1;           // must be zero
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p2_addr:40;
    uint64_t available:11;    // unused bits
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L3{
    struct PT3_entry entries[512];
};
struct PT2_entry{
    uint64_t present:1;
    uint64_t rw:1;               // read/write
    uint64_t us:1;               // user/supervisor
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign1:1;             // ignored
    uint64_t big_page:1;
    uint64_t ign2:1;             // ignored
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p1_addr:40;
    uint64_t available:11;       // unused bits
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L2{
    struct PT2_entry entries[512];
};
struct PT1_entry{
    uint64_t present:1;
    uint64_t rw:1;               // read/write
    uint64_t us:1;               // user/supervisor
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t d:1;
    uint64_t pat:1;
    uint64_t g:1;
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t phys_addr:40;       // page addr
    uint64_t available:11;       // unused bits
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L1{
    struct PT1_entry entries[512];
};

struct virt_addr{
   uint64_t sign_extension:16;
   uint64_t l4_offset:9;
   uint64_t l3_offset:9;
   uint64_t l2_offset:9;
   uint64_t l1_offset:9;
   uint64_t phys_offset:12;
}__attribute__((packed));

static void map_null(struct PageTable_L3 *pdp3, struct PageTable_L2 *pd2, struct PageTable_L1 *pt1);
//static void set_up_kernel_stack();
//static void set_up_kernel_heap();

extern void *MMU_alloc_page();
extern void *MMU_alloc_pages(int num);
extern void MMU_free_page(void *);
extern void MMU_free_pages(void *, int num);

struct PageTable_L4 *pml4;

extern void init_virtual_memory(){
   // identity map first 512 GB to L4[0]
   identity_map();
   // set up kernel stacks at L4[1]
   // set_up_kernel_stack();
   // set up kernel heap at L4[15]
   // set_up_kernel_heap();
}

static void map_null(struct PageTable_L3 *pdp3, struct PageTable_L2 *pd2, struct PageTable_L1 *pt1){
   pdp3->entries[0].p2_addr = (uint64_t)pd2;
   pdp3->entries[0].present = 1;

   pd2->entries[0].p1_addr = (uint64_t)pt1;
   pd2->entries[0].present = 1;

   pt1->entries[0].present = 0;
   pt1->entries[0].nx = 1;
}

static void identity_map(){
   // alloc a page for L4 table & set to zero
   pml4 = MMU_pf_alloc();
   memset(pml4, 0, PAGE_SIZE);
   struct PageTable_L3* pdp3 = MMU_pf_alloc();
   memset(pdp3, 0, PAGE_SIZE);
   struct PageTable_L2* pd2 = MMU_pf_alloc();
   memset(pd2, 0, PAGE_SIZE);
   struct PageTable_L1* pt1 = MMU_pf_alloc();
   memset(pt1, 0, PAGE_SIZE);

   map_null(pdp3, pd2, pt1);
   // map_vga();

   /*
   for (int i = 0; i < 512; i++){
      pml4->entries[0]
      for (int j = 0; j < 512; j++){
         for (int k = 0; k < 512; k++){
         }
      }
   }
   */
}

