#include "multiboot.h"
#include "commonio.h"
#include "pf.h"
#include <stdint-gcc.h>

#define TAG_END 0
#define TAG_BOOT_CMD 1
#define TAG_BOOT_LOADER 2
#define TAG_BASIC_MEM_INFO 4
#define TAG_BIOS_BOOT_DEVICE 5
#define TAG_MEMORY_MAP 6
#define TAG_ELF_SYMBOLS 9

struct mem_info_ent{
   uint64_t start_addr;
   uint64_t length;
   uint32_t type;
   uint32_t reserved;
}__attribute__((packed));

struct section_header{     // Section in Tag 9 (ELF Header)
   uint32_t name;          // section name as an index into the string table
   uint32_t type;
   uint64_t flags;
   uint64_t addr;          // address fo segment in memory
   uint64_t offset;        // offset of segment in file on disk
   uint64_t size;          // size of segment
   uint32_t table_index;
   uint32_t extra;
   uint64_t addr_alignment;   // poewrs of 2
   uint64_t fixed;            // fixed entry size IFF section holds fixed entries
}__attribute__((packed));

struct multiboot_header{
   uint32_t len;
   uint32_t reserved;
}__attribute__ ((packed));

struct mb_tag_generic{  // Generic / Tag 0 (End of multiboot header)
   uint32_t tag_type;
   uint32_t tag_size;
}__attribute__ ((packed));

struct mb_tag_1{        // Boot command line
   uint32_t tag_type;
   uint32_t tag_size;
   uint64_t *cmd_line_str;
}__attribute__ ((packed));

struct mb_tag_2{        // Boot loader name
   uint32_t tag_type;
   uint32_t tag_size;
   uint64_t *boot_ldr_str;
}__attribute__ ((packed));

struct mb_tag_4{        // Basic Memory Info
   uint32_t tag_type;
   uint32_t tag_size;
   uint32_t low_mem_size;
   uint32_t high_mem_size;
}__attribute__ ((packed));

struct mb_tag_5{        // BIOS Boot Device
   uint32_t tag_type;
   uint32_t tag_size;
   uint32_t device;
   uint32_t partition;
   uint32_t subpartition;
}__attribute__ ((packed));

struct mb_tag_6{        // Memory Map
   uint32_t tag_type;
   uint32_t tag_size;
   uint32_t mem_info_ent_size;
   uint32_t mem_info_ent_ver;
   struct mem_info_ent mem_info[];
}__attribute__ ((packed));

struct mb_tag_9{        // ELF Symbols
   uint32_t tag_type;
   uint32_t tag_size;
   uint32_t num_sec_header_entries;
   uint32_t size_sec_header_entry;
   uint32_t index_string_table;
   struct section_header sec_header[];
}__attribute__ ((packed));

static uint64_t align8(uint32_t size){
   uint8_t rem = size % 8;
   if (rem != 0)
      size += 8 - rem;
   return size;
}

extern void initialize_multiboot(void *multiboot_ptr){
   void *current = multiboot_ptr;
   struct multiboot_header *mb_header = (struct multiboot_header *)(multiboot_ptr);
   // printk("Multiboot: %p\n", multiboot_ptr);
   // printk("   \t len: 0x%x bytes\n", mb_header->len);
   // printk("   \t res: 0x%x\n", mb_header->reserved);

   current = (void*) (mb_header + 1);
   struct mb_tag_generic *gen = (struct mb_tag_generic*)(current);

   while (gen->tag_type != TAG_END) {
      switch (gen->tag_type){
         case TAG_END:
            // printk("Tag End: %p\n", gen);
            // printk("   \t type: %u\n", gen->tag_type);
            // printk("   \t size: %u\n", gen->tag_size);
            return;
            break;
         case TAG_BOOT_CMD:
            /*
            struct mb_tag_1 *boot_cmd = (struct mb_tag_1*)(current);
            printk("Tag Boot Commandline Args: %p\n", boot_cmd);
            printk("   \t type: %u\n", boot_cmd->tag_type);
            printk("   \t size: %u\n", boot_cmd->tag_size);
            */
            break;
         case TAG_BOOT_LOADER:
            /*
            struct mb_tag_2 *boot_loader = (struct mb_tag_2*)(current);
            printk("Tag Boot Loader: %p\n", boot_loader);
            printk("   \t type: %u\n", boot_loader->tag_type);
            printk("   \t size: %u\n", boot_loader->tag_size);
            */
            break;
         case TAG_BASIC_MEM_INFO:
            /*
            struct mb_tag_4 *mem_info = (struct mb_tag_4*)(current);
            printk("Tag Memory Info: %p\n", mem_info);
            printk("   \t type: %u\n", mem_info->tag_type);
            printk("   \t size: %u\n", mem_info->tag_size);
            */
            break;
         case TAG_BIOS_BOOT_DEVICE:
            /*
            struct mb_tag_5 *boot_device = (struct mb_tag_5*)(current);
            printk("Tag Boot Device: %p\n", boot_device);
            printk("   \t type: %u\n", boot_device->tag_type);
            printk("   \t size: %u\n", boot_device->tag_size);
            */
            break;
         case TAG_MEMORY_MAP:
            struct mb_tag_6 *memory_map = (struct mb_tag_6*)(current);
            // printk("\nTag Memory Map: %p\n", memory_map);
            // printk("   \t type: %u\n", memory_map->tag_type);
            // printk("   \t tag size: %u\n", memory_map->tag_size);
            // printk("   \t entry size: %u \n", memory_map->mem_info_ent_size);
            // printk("   \t entry version: %u \n", memory_map->mem_info_ent_ver);

            for (int i = 0; i < ((memory_map->tag_size - sizeof(struct mb_tag_6)) / sizeof(struct mem_info_ent)); i++){
               struct mem_info_ent *ent = (struct mem_info_ent*)(memory_map->mem_info + i);
               if (ent->type == 1){
                  // printk("   Memory Info Entry: %p\n", ent);
                  // printk("      \t starting address: 0x%lx\n", ent->start_addr);
                  // printk("      \t length: 0x%lx\n", ent->length);
                  // printk("      \t type: %u\n", ent->type);
                  update_starts(ent->start_addr, ent->length);
               }
            }
            break;
         case TAG_ELF_SYMBOLS:
            struct mb_tag_9 *elf_symbols = (struct mb_tag_9*)(current);
            // printk("\nTag ELF Symbols: %p\n", elf_symbols);
            // printk("   \t type: %u\n", elf_symbols->tag_type);
            // printk("   \t size: %u\n", elf_symbols->tag_size);
            for (int i = 0; i < elf_symbols->num_sec_header_entries; i++){
               struct section_header *sec = (struct section_header*)(elf_symbols->sec_header + i);
               if (sec->flags & 2){
                  // printk("   ELF Section Header: %p\n", sec);
                  // printk("      \t starting address: 0x%lx\n", sec->addr);
                  // printk("      \t size: 0x%lx\n", sec->size);
                  // printk("      \t type: %u\n", sec->type);
                  // printk("      \t flags: %lu\n", sec->flags);
                  update_avoids(sec->addr, sec->size);
               }
            }
            break;
         default:
            /*
            printk("Generic Tag: %p\n", gen);
            printk("   \t type: %u\n", gen->tag_type);
            printk("   \t size: %u\n", gen->tag_size);
            */
            break;
      }
      current = (void*) current + align8(gen->tag_size);
      gen = (struct mb_tag_generic*)(current);
   }
   initialize_pf();
}
