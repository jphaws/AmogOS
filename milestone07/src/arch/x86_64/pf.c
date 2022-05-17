#include "pf.h"
#include "commonio.h"
#include <stddef.h>

#define PF_STATIC_ENTRIES_MAX 64
#define PAGE_SIZE 4096

#define CEILING(N,C) (((N) / (C) + ((N) % (C) != 0)) * (C))
#define FLOOR(N, C) ((N) / (C) * (C))


static uint64_t start_addrs[PF_STATIC_ENTRIES_MAX];
static uint64_t start_length[PF_STATIC_ENTRIES_MAX];
static uint8_t start_idx = 0;
static uint8_t num_usable_segments = 0;

static uint64_t avoid_addrs[PF_STATIC_ENTRIES_MAX];
static uint64_t avoid_size[PF_STATIC_ENTRIES_MAX];
static uint8_t num_avoid_segments = 0;

static uint64_t *freed = NULL;
static uint64_t *head = NULL;

extern void update_starts(uint64_t addr, uint64_t length){
   if ((num_usable_segments + 1) < PF_STATIC_ENTRIES_MAX){
      start_addrs[num_usable_segments] = addr;
      start_length[num_usable_segments++] = length;
   }
}

extern void update_avoids(uint64_t addr, uint64_t size){
   if ((num_avoid_segments + 1) < PF_STATIC_ENTRIES_MAX){
      avoid_addrs[num_avoid_segments] = addr;
      avoid_size[num_avoid_segments++] = size;
   }
}

// Remove 0x0 from available page frames & select first available segment
extern void initialize_pf(){
   uint64_t start = 0;
   uint64_t low = -1;
   uint64_t high = 0;

   // Assume contiguous memory of ELF addresses to avoid, calc low and high addr
   for (int i = 0; i < num_avoid_segments; i++){
      if (avoid_addrs[i] + avoid_size[i] > high){
         high = CEILING(avoid_addrs[i] + avoid_size[i], PAGE_SIZE);
      }
      if (avoid_addrs[i] < low)
         low = FLOOR(avoid_addrs[i], PAGE_SIZE);
   }

   for (int i = 0; i < num_usable_segments; i++){
      // overlapping on rightmost boundary of start segment
      if (start_addrs[i] < low && low < start_addrs[i] + start_length[i] && high >= start_addrs[i] + start_length[i]){
         uint64_t overlap = start_addrs[i] + start_length[i] - low;
         start_length[i] -= overlap;
         start_length[i] = FLOOR(start_length[i], PAGE_SIZE);
      }
      // overlapping on leftmost boundary of start segment
      else if (low <= start_addrs[i] && high > start_addrs[i] && high < start_addrs[i] + start_length[i]){
         uint64_t overlap = high - start_addrs[i];
         start_addrs[i] += overlap;
         start_addrs[i] = CEILING(start_addrs[i], PAGE_SIZE);
         start_length[i] = FLOOR((start_length[i] - overlap), PAGE_SIZE);
      }
      // overlapping in middle of start segment
      else if (start_addrs[i] < low && high < start_addrs[i] + start_length[i]){
         uint64_t left_bound = start_addrs[i] + start_length[i] - low;

         // shrink left side address space
         start_length[i] -= left_bound;
         start_length[i] = FLOOR(start_length[i], PAGE_SIZE);

         // add right side address space to start_addrs
         if ((num_usable_segments + 1) < PF_STATIC_ENTRIES_MAX){
            start_addrs[++num_usable_segments] = CEILING(high, PAGE_SIZE);
            start_length[num_usable_segments] = start_addrs[i] + start_length[i] - CEILING(high, PAGE_SIZE);
         }
      }
      // start segment is completely covered by reserved addresses
      else if (low <= start_addrs[i] && high >= start_addrs[i] + start_length[i]){
         start_length[i] = 0;
         start_addrs[i] = 0;
      }
      // remove null from addressable segments
      if (start_addrs[i] == 0x0 && start_length[i] > PAGE_SIZE){
         start_addrs[i] += PAGE_SIZE;
         start_length[i] -= PAGE_SIZE;
      }
   }
   start = start_addrs[0];
   start_idx = 0;
   head = (uint64_t*)start;
   printk("Page frame allocator Initalized\n");
}

extern void MMU_pf_free(void *pf){
   uint64_t *new_head = (uint64_t*)pf;
   if (freed == NULL){
      *new_head = 0;
      freed = new_head;
   }
   else {
      *new_head = (uint64_t)freed;
      freed = new_head;
   }
}

extern void *MMU_pf_alloc(void){
   void *ret = NULL;
   // Use freed list if frames are available
   if (freed != NULL){
      ret = (void*)(freed);
      freed = (uint64_t*)(*freed);
      return ret;
   }

   if (start_length[start_idx] == 0 || start_addrs[start_idx] == 0){
      if (start_idx + 1 < num_usable_segments)
         start_idx++;
      else 
         return NULL;
   }

   // if still within a segment
   if (start_idx < num_usable_segments && ((uint64_t)head - start_addrs[start_idx]) < start_length[start_idx]){
      ret = head;

      // increment the head pointer by a page if there's space remaining
      if ((uint64_t)head + PAGE_SIZE < start_addrs[start_idx] + start_length[start_idx])
         head = (uint64_t*)((uint64_t)head + PAGE_SIZE);
      // increment to the next segment and set head pointer
      else if (++start_idx < num_usable_segments){
         head = (uint64_t*)start_addrs[start_idx];
      }
      else
         return NULL;
   }
   else 
      return NULL;

   return ret;
}
