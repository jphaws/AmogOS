#include <stdint-gcc.h>
#include <stdbool.h>
#include "vmem.h"
#include "pf.h"
#include "commonlib.h"

#define VGA_BASE 0xb8000

#define L1_BITS 12
#define L2_BITS 9
#define L3_BITS 9
#define L4_BITS 9

#define PML4_OFFSET(addr) (((addr) >> ((L1_BITS) + (L2_BITS) + (L3_BITS))) & (0x1FF))
#define PDP3_OFFSET(addr) (((addr) >> ((L1_BITS) + (L2_BITS))) & (0x1FF))
#define PD2_OFFSET(addr) (((addr) >> (L1_BITS)) & (0x1FF))
#define PT1_OFFSET(addr) ((addr) & (0xFFF))

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

static void map_null();
static void map_vga();
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


static void map_addr(uint64_t addr, bool present, bool nx){
   struct PageTable_L3* pdp3;
   struct PageTable_L2* pd2;
   struct PageTable_L1* pt1;
   uint16_t l4_offset = PML4_OFFSET(addr);
   uint16_t l3_offset = PDP3_OFFSET(addr);
   uint16_t l2_offset = PD2_OFFSET(addr);
   uint16_t l1_offset = PT1_OFFSET(addr);

   // create new pdp3 entry if not present 
   if(!pml4->entries[l4_offset].present){
      pdp3 = MMU_pf_alloc();
      memset(pdp3, 0, PAGE_SIZE);
      pml4->entries[l4_offset].p3_addr = (uint64_t)pdp3;
      pml4->entries[l4_offset].present = 1;
   }
   pdp3 = (struct PageTable_L3*)((uint64_t)(pml4->entries[l4_offset].p3_addr));

   // create new pd2 entry if not present 
   if(!pdp3->entries[l3_offset].present){
      pd2 = MMU_pf_alloc();
      memset(pd2, 0, PAGE_SIZE);
      pdp3->entries[l3_offset].p2_addr = (uint64_t)pd2;
      pdp3->entries[l3_offset].present = 1;
   }
   pd2 = (struct PageTable_L2*)((uint64_t)(pdp3->entries[l3_offset].p2_addr));

   // create new pt1 entry if not present 
   if(!pd2->entries[l2_offset].present){
      pt1 = MMU_pf_alloc();
      memset(pt1, 0, PAGE_SIZE);
      pd2->entries[l2_offset].p1_addr = (uint64_t)pt1;
      pd2->entries[l2_offset].present = 1;
   }
   pt1 = (struct PageTable_L1*)((uint64_t)(pd2->entries[l2_offset].p1_addr));
   pt1->entries[l1_offset].phys_addr = addr;
   if (present)
      pt1->entries[l1_offset].present = 1;
   if (nx)
      pt1->entries[l1_offset].nx = 1;
}

static void map_null(){
   map_addr(0, false, true);
}

static void map_vga(){
   map_addr(VGA_BASE, true, true);
}

static void identity_map(){
   // alloc a page for L4 table & set to zero
   pml4 = MMU_pf_alloc();
   memset(pml4, 0, PAGE_SIZE);

   /*
   struct PageTable_L3* pdp3 = MMU_pf_alloc();
   memset(pdp3, 0, PAGE_SIZE);
   struct PageTable_L2* pd2 = MMU_pf_alloc();
   memset(pd2, 0, PAGE_SIZE);
   struct PageTable_L1* pt1 = MMU_pf_alloc();
   memset(pt1, 0, PAGE_SIZE);
   */

   map_null();
   map_vga();

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

