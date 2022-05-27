#include "vmem.h"
#include <stdint-gcc.h>

struct PT4_entry{
    uint64_t present:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign:1;              // ignored
    uint64_t mbz:2;              // ignored?
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p3_addr:40;
    uint64_t available:11;
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L4{
    struct PT4_entry entries[512];    
};
struct PT3_entry{
    uint64_t present:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign:1;           // ignored
    uint64_t big_page:1;      // set 0?
    uint64_t mbz:1;           // ignored?
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p2_addr:40;
    uint64_t available:11;
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L3{
    struct PT3_entry entries[512];
};
struct PT2_entry{
    uint64_t present:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ign1:1;             // ignored
    uint64_t big_page:1;
    uint64_t ign2:1;             // ignored
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t p1_addr:40;
    uint64_t available:11;
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L2{
    struct PT2_entry entries[512];
};
struct PT1_entry{
    uint64_t present:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t d:1;
    uint64_t pat:1;
    uint64_t g:1;
    uint64_t allocated:1;
    uint64_t avl:2;
    uint64_t phys_addr:40;
    uint64_t available:11;
    uint64_t nx:1;
}__attribute__((packed));
struct PageTable_L1{
    struct PT1_entry entries[512];
};
