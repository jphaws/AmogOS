#include "vmem.h"

struct PT4_entry{
    uint32_t present:1;
    uint32_t rw:1;
    uint32_t us:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t a:1;
    uint32_t ign:1;              // ignored
    uint32_t mbz:2;              // ignored?
    uint32_t allocated:1;
    uint32_t avl:2;
    uint32_t p3_addr_low:20;
    uint32_t p3_addr_high:20;
    uint32_t available:11;
    uint32_t nx:1;
}__attribute__((packed));
struct PageTable_L4{
    struct PT4_entry entries[512];    
};
struct PT3_entry{
    uint32_t present:1;
    uint32_t rw:1;
    uint32_t us:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t a:1;
    uint32_t ign:1;           // ignored
    uint32_t big_page:1;      // set 0?
    uint32_t mbz:1;           // ignored?
    uint32_t allocated:1;
    uint32_t avl:2;
    uint32_t p2_addr_low:20;

    uint32_t p2_addr_high:20;
    uint32_t available:11;
    uint32_t nx:1;
}__attribute__((packed));
struct PageTable_L3{
    struct PT3_entry entries[512];
};
struct PT2_entry{
    uint32_t present:1;
    uint32_t rw:1;
    uint32_t us:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t a:1;
    uint32_t ign1:1;             // ignored
    uint32_t big_page:1;
    uint32_t ign2:1;             // ignored
    uint32_t allocated:1;
    uint32_t avl:2;
    uint32_t p1_addr_low:20;

    uint32_t p1_addr_high:20;
    uint32_t available:11;
    uint32_t nx:1;
}__attribute__((packed));
struct PageTable_L2{
    struct PT2_entry entries[512];
};
struct PT1_entry{
    uint32_t present:1;
    uint32_t rw:1;
    uint32_t us:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t a:1;
    uint32_t d:1;
    uint32_t pat:1;
    uint32_t g:1;
    uint32_t allocated:1;
    uint32_t avl:2;
    uint32_t phys_addr_low:20;

    uint32_t phys_addr_high:20;
    uint32_t available:11;
    uint132_t nx:1;
}__attribute__((packed));
struct PageTable_L1{
    struct PT1_entry entries[512];
};
