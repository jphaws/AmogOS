#ifndef __BB_H__
#define __BB_H__

#define BUF_SIZE 16

struct State {
   char buff[BUF_SIZE];
   char *consumer;
   char *producer;
};

extern void BB_enqueue(struct State* state, char toAdd);
extern char BB_dequeue(struct State* state);
extern void BB_init(struct State* state);

#endif
