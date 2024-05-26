/*
 * queue.h
 *
 *  Definitions for managing a queue data structure.
 */

#ifndef FOURKNIGHTS_QUEUE_H
#define FOURKNIGHTS_QUEUE_H


typedef void * item_t;
typedef struct
{
    item_t item;      /* Pointer to the state. */
    unsigned int F;   /* F(x) = G(x) + H(x) */
    unsigned int G;   /* G(x) */
    unsigned int H;   /* H(x) */
} queue_object_t;

typedef struct
{
    queue_object_t *items;
    unsigned int capacity;
    unsigned int current_size;
} queue_t;


queue_t *
queue__create(
    unsigned int capacity
);

void
queue__destroy(
    queue_t **queue
);

int
queue__contains(
    queue_t *queue,
    item_t   item,
    unsigned long long length
);

void
queue__insert(
    queue_t *queue,
    item_t item,
    unsigned int F,
    unsigned int G,
    unsigned int H
);

queue_object_t
queue__get_min(
    queue_t *queue
);


#endif   /* FOURKNIGHTS_QUEUE_H */
