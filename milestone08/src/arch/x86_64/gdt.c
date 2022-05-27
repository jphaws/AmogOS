#include "gdt.h"
#include "inline.h"
#include "commonlib.h"
#include "commonio.h"

#define TSS_64_BIT 0x9
#define STACK_SIZE 4096
#define TSS_GDT_OFFSET 0x2

#define PF_INDEX 1
#define DF_INDEX 2
#define GP_INDEX 3

//  External assembly function
extern void reloadSegments();

struct TSS {
   uint32_t ign1;       // ignored/reserved
   uint64_t rsp[3];     // Stack pointers
   uint64_t ign2;       // ignored/reserved uint64_t ist[7];     
   uint64_t ist[7];     // Interrupt stack table
   uint64_t ign3;       // ignored/reserved
   uint16_t ign4;       // ignored/reserved
   uint16_t iopb;       // I/O Map Base Address Field
} __attribute__((packed));
// most fields are ignored in 64 bit mode
typedef struct GDTEntry {
   uint16_t limit1;      // ignored in CS & DS
   uint16_t base1;       // ignored in CS & DS

   uint8_t base2;        // ignored in CS & DS
                       // begin ACCESS BYTE
   uint8_t a:1;        // accessed (leave 0)
   uint8_t rw:1;       // CS 1 = readable | DS 1 = writeable
   uint8_t dc:1;       // Direction/Conforming - DS 1 = segment grows down, CS 1 = execute from equal or lower privelege
   uint8_t ex:1;        // DS = 0 , CS = 1 (executable)
   uint8_t s:1;        // 1 == DS || CS, 0 == TSS (Descriptor type bit)
   uint8_t dpl:2;      // 0 = kernel priv, 3 = user priv (descrip. priv lvl)
   uint8_t p:1;        // 1 = valid segment (present)
                       // end ACCESS BYTE
   uint8_t limit2:4;          // ignored in CS & DS
   uint8_t avl:1;             // ignored in CS & DS
   uint8_t l:1;               // 1 = CS, ignored if DS
   uint8_t db:1;              // ignored in DS (16 bit = 0, 32 bit = 1)
   uint8_t g:1;               // ignored in CS & DS (granularity)
   
   uint8_t base3;        //ignored
} __attribute__ ((packed)) GDTEntry;

typedef struct TSSEntry {
   uint16_t limit1;
   uint16_t base1; 

   uint8_t base2;   
                       // begin ACCESS BYTE
   uint8_t type:4;     // (3 types in long mode)
   uint8_t s:1;        // 1 == DS || CS, 0 == TSS
   uint8_t dpl:2;      // 0 = kernel priv, 3 = user priv (descrip. priv lvl)
   uint8_t p:1;        // 1 = valid segment (present)
                       // end ACCESS BYTE

   uint8_t limit2:4;    
   uint8_t avl:1;             
   uint8_t ign:2;             // ignored in TSS
   uint8_t g:1;               // ignored in CS & DS (granularity)
   
   uint8_t base3;        
   uint32_t base4;
   uint32_t reserved;      // most bits ignored or zeroed
} __attribute__ ((packed)) TSSEntry;

struct GDT_Segments {
   GDTEntry Null;
   GDTEntry CodeSegment;
   TSSEntry TSS;
} __attribute__((packed));

static struct GDT_Segments GDT;
static struct TSS TSS;

static uint8_t *tss_stack_gp[STACK_SIZE];
static uint8_t *tss_stack_df[STACK_SIZE];
static uint8_t *tss_stack_pf[STACK_SIZE];

extern void initialize_gdt(void){
   memset(&GDT, 0, sizeof(GDT));

   // null entry set to all 0
   //set the code segment bits
   GDT.CodeSegment.s = 1; 
   GDT.CodeSegment.ex = 1; 
   GDT.CodeSegment.rw = 1; 
   GDT.CodeSegment.p = 1; 
   GDT.CodeSegment.l = 1;
   GDT.CodeSegment.g = 1;

   //set the TSS bits
   GDT.TSS.base1 = ((uint64_t)(&TSS)) & 0xFFFF;
   GDT.TSS.base2 = (((uint64_t)(&TSS)) >> 16) & 0xFF;
   GDT.TSS.base3 = (((uint64_t)(&TSS)) >> 24) & 0xFF;
   GDT.TSS.base4 = (((uint64_t)(&TSS)) >> 32) & 0xFFFFFFFF;
   GDT.TSS.limit1 = (sizeof(TSS) - 1) & 0xFFFF;
   GDT.TSS.limit2 = ((sizeof(TSS) - 1) >> 16) & 0xF;
   GDT.TSS.type = TSS_64_BIT;
   GDT.TSS.p = 1;

   //load the updated GDT
   lgdt(&GDT, sizeof (GDT));
   // load the updated code segment (assembly function)
   reloadSegments();

   printk("GDT Initialized\n");
}

// Initialization function to set Task State Segment to support seperate 
// stacks for General Protection Faults, Double Faults, & Page Faults
extern void initialize_tss(void){
   // Point TSS Interrupt Stack Tables to new stack locations
   TSS.ist[GP_INDEX] = (uint64_t) &(tss_stack_gp[STACK_SIZE - 1]);
   TSS.ist[DF_INDEX] = (uint64_t) &(tss_stack_df[STACK_SIZE - 1]);
   TSS.ist[PF_INDEX] = (uint64_t) &(tss_stack_pf[STACK_SIZE - 1]);

   //Load the new TSS
   ltr(TSS_GDT_OFFSET);

   printk("TSS Initialized\n");
}

// Returns TSS Interrupt Stack Table Offset for use when setting the IDT
extern uint8_t get_DF_stack_offset(void){
   return DF_INDEX;
}

// Returns TSS Interrupt Stack Table Offset for use when setting the IDT
extern uint8_t get_GP_stack_offset(void){
   return GP_INDEX;
}

// Returns TSS Interrupt Stack Table Offset for use when setting the IDT
extern uint8_t get_PF_stack_offset(void){
   return PF_INDEX;
}

