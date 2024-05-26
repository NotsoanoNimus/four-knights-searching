/*
 * list.h
 *
 *  Definitions for linked list structures.
 */

#ifndef FOURKNIGHTS_LIST_H
#define FOURKNIGHTS_LIST_H


typedef void * node_t;

typedef struct list_node list_node_t;

struct list_node
{
    node_t       node;
    list_node_t *next;
};

typedef struct
{
    list_node_t *head;
    list_node_t *tail;
} list_t;


list_t *
list__create();

void
list__destroy(
    list_t **p_list,
    int deep
);

void
list__reverse(
    list_t *list
);

void
list__insert(
    list_t *list,
    node_t value
);

void
list__merge(
    list_t *left,
    list_t *right
);

int
list__contains(
    list_t *list,
    node_t value,
    unsigned long long length
);


#endif   /* FOURKNIGHTS_LIST_H */
