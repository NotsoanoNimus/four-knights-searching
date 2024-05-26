/*
 * queue.c
 *
 *  Implementation of priority queue stuffs.
 */

#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static inline
void
swap(queue_object_t *left,
     queue_object_t *right)
{
    queue_object_t temp = *left;
    *left = *right;
    *right = temp;
}

static
void
min_heapify(queue_t *queue,
            int i)
{
    int smallest = i;
    int left_sub = 2 * i + 1;
    int right_sub = 2 * i + 2;

    if (left_sub < queue->current_size &&
            queue->items[left_sub].F < queue->items[smallest].F)
        smallest = left_sub;

    if (right_sub < queue->current_size &&
            queue->items[right_sub].F < queue->items[smallest].F)
        smallest = right_sub;

    if (smallest != i) {
        swap(&queue->items[i], &queue->items[smallest]);
        min_heapify(queue, smallest);
    }
}


queue_t *
queue__create(unsigned int capacity)
{
    queue_t *q = calloc(1, sizeof(queue_t));
    q->capacity = capacity;
    q->items = (queue_object_t *)calloc(capacity, sizeof(queue_object_t));
    return q;
}


void
queue__destroy(queue_t **queue)
{
    if (NULL == queue || NULL == *queue) return;

    free((*queue)->items);
    *queue = NULL;
}


int
queue__contains(queue_t* queue,
                item_t item,
                unsigned long long length)
{
    for (int i = 0; i < queue->current_size; i++)
        if (0 == memcmp(queue->items[i].item, item, length))
            return 1;

    return 0;
}


void
queue__insert(queue_t *queue,
              item_t item,
              unsigned int F,
              unsigned int G,
              unsigned int H)
{
    if (queue->current_size == queue->capacity) {
        fprintf(stderr, "The queue exceeded its limit. Consider increasing it.\n");
        exit(1);
    }

    queue->items[queue->current_size].item = item;
    queue->items[queue->current_size].F = F;
    queue->items[queue->current_size].G = G;
    queue->items[queue->current_size].H = H;

    ++queue->current_size;

    int i = queue->current_size - 1;
    while (i != 0 &&
        queue->items[(i - 1) / 2].F > queue->items[i].F)
    {
        swap(&queue->items[i], &queue->items[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}


queue_object_t
queue__get_min(queue_t *queue)
{
    queue_object_t q = {
        .item = NULL,
        .F = 0,
        .G = 0,
        .H = 0,
    };
    if (queue->current_size <= 0) return q;

    if (queue->current_size == 1) {
        --queue->current_size;
        return queue->items[0];
    }

    queue_object_t root = queue->items[0];

    queue->items[0] = queue->items[queue->current_size - 1];
    --queue->current_size;

    min_heapify(queue, 0);

    return root;
}
