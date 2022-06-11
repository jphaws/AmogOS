#include <stdint-gcc.h>
#include "kmalloc.h"
#include "vmem.h"
#include "commonio.h"
#include "pf.h"

#define XSMALL_BLOCK    32
#define SMALL_BLOCK     64
#define MEDIUM_BLOCK   128
#define LARGE_BLOCK    512
#define XLARGE_BLOCK  1024
#define XXLARGE_BLOCK 2048

#define CEILING(N,C) (((N) / (C) + ((N) % (C) != 0)) * (C))

struct FreeList {
   struct FreeList *next;
};

struct KmallocPool {
   int max_size;
   int avail;
   struct FreeList *head;
};


struct KmallocHeader {
   struct KmallocPool *pool;
   size_t size;
};

struct KmallocPool Pool_XS;        // 32   byte blocks
struct KmallocPool Pool_S;         // 64   byte blocks
struct KmallocPool Pool_M;         // 128  byte blocks
struct KmallocPool Pool_L;         // 512  byte blocks
struct KmallocPool Pool_XL;        // 1024 byte blocks
struct KmallocPool Pool_XXL;       // 2048 byte blocks

extern void kfree(void *addr){
   struct KmallocHeader *head = (struct KmallocHeader*)((uint8_t*)addr - sizeof(struct KmallocHeader));

   if (head->pool != NULL){
      struct KmallocPool *pool = head->pool;
      ((struct FreeList *)(head))->next = pool->head;
      pool->head = ((struct FreeList *)(head));
      pool->avail++;
   }
   // free alloc'd virtual page
   else{
      MMU_free_pages((void*)head, CEILING(head->size, PAGE_SIZE) / PAGE_SIZE);
   }
}

static void *set_header(struct KmallocPool *pool, size_t block_size){
   void *ret = pool->head;
   if (ret != NULL && pool->avail > 0){
      pool->head = pool->head->next;
      pool->avail--;
      ((struct KmallocHeader*)ret)->pool = pool;
      ((struct KmallocHeader*)ret)->size = block_size;
   }
   // allocate a new virtual page for pool
   else{
      void *page = MMU_alloc_page();
      if (page != NULL){
         pool->avail = PAGE_SIZE / block_size;
         pool->head = (struct FreeList*)page;

         uint8_t *block = page;
         for (int i = 0; i < pool->avail; i++){
            ((struct FreeList *)(block + i * block_size))->next = pool->head;
            pool->head = ((struct FreeList *)(block + i * block_size));
         }
         ret = pool->head;
         pool->head = pool->head->next;
         pool->avail--;
         ((struct KmallocHeader*)ret)->pool = pool;
         ((struct KmallocHeader*)ret)->size = block_size;
      }
      else
         return NULL;
   }
   return (uint8_t*)ret + sizeof(struct KmallocHeader);
}

extern void *kmalloc(size_t size){
   if (size <= Pool_XS.max_size){
      return set_header(&Pool_XS, XSMALL_BLOCK);
   }
   else if (size <= Pool_S.max_size){
      return set_header(&Pool_S, SMALL_BLOCK);
   }
   else if (size <= Pool_M.max_size){
      return set_header(&Pool_M, MEDIUM_BLOCK);
   }
   else if (size <= Pool_L.max_size){
      return set_header(&Pool_L, LARGE_BLOCK);
   }
   else if (size <= Pool_XL.max_size){
      return set_header(&Pool_XL, XLARGE_BLOCK);
   }
   else if (size <= Pool_XXL.max_size){
      return set_header(&Pool_XXL, XXLARGE_BLOCK);
   }
   // allocate an entire page outside of a pool
   else{
      size_t num_pages = CEILING(size, PAGE_SIZE) / PAGE_SIZE;
      void *ret = MMU_alloc_pages(num_pages);
      if (ret != NULL){
         ((struct KmallocHeader*)ret)->pool = NULL;
         ((struct KmallocHeader*)ret)->size = CEILING(size, PAGE_SIZE);
         return (uint8_t *)ret + sizeof(struct KmallocHeader);
      }
      else{
         return NULL;
      }
   }
}

static void init_pool(struct KmallocPool *pool, size_t block_size){
   void *page = MMU_alloc_page();
   if (page != NULL){
      pool->max_size = block_size - sizeof(struct KmallocHeader);
      pool->avail = PAGE_SIZE / block_size;
      pool->head = NULL;

      uint8_t *block = page;
      for (int i = 0; i < pool->avail; i++){
         ((struct FreeList *)(block + i * block_size))->next = pool->head;
         pool->head = ((struct FreeList *)(block + i * block_size));
      }
   }
}

extern void init_kmalloc(void){
   init_pool(&Pool_XS, XSMALL_BLOCK);
   init_pool(&Pool_S, SMALL_BLOCK);
   init_pool(&Pool_M, MEDIUM_BLOCK);
   init_pool(&Pool_L, LARGE_BLOCK);
   init_pool(&Pool_XL, XLARGE_BLOCK);
   init_pool(&Pool_XXL, XXLARGE_BLOCK);
   printk("kmalloc initialized\n");
}
