#include <stdbool.h>
#include "boundedbuffer.h"
#include "inline.h"


extern void BB_init(struct State *state){
   state->consumer = &state->buff[0];
   state->producer = &state->buff[0];
}

extern char BB_dequeue(struct State *state){
   bool interrupts = are_interrupts_enabled();
   if (interrupts)
      CLI
   // buffer is empty
   if (state->consumer == state->producer){
      if (interrupts)
         STI
      return -1;
   }

   char c = *(state->consumer++);

   if (state->consumer >= &state->buff[BUF_SIZE])
      state->consumer = &state->buff[0];

   if (interrupts)
      STI
   return c;
}

extern void BB_enqueue(struct State *state, char toAdd){
   bool interrupts = are_interrupts_enabled();
   if (interrupts)
      CLI

   // buffer is full
   if (state->producer == state->consumer - 1 || 
         (state->consumer == &state->buff[0] && state->producer == &state->buff[BUF_SIZE - 1])){
      if (interrupts)
         STI
      return;
   }
   *state->producer++ = toAdd;
   if (state->producer >= &state->buff[BUF_SIZE])
      state->producer = &state->buff[0];

   if (interrupts)
      STI
}
