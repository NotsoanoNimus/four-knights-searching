/*
 * list.c
 *
 *  Implementation for linked list items.
 */

#include "list.h"

#include <stdlib.h>
#include <string.h>


list_t *
list__create()
{
    list_t *new_list = calloc(1, sizeof(list_t));
    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}


void
list__destroy(list_t **p_list,
              int deep)
{
    if (NULL == p_list || NULL == *p_list) return;

    list_t *list = *p_list;
    if (!deep) goto finalize;

    list_node_t *node = list->head;
    while (NULL != node) {
        list_node_t *last_node = node;
        free(node->node);
        node = node->next;
        free(last_node);
    }

finalize:
    free(list);
    *p_list = NULL;
    return;
}


void
list__reverse(list_t *list)
{
    if (NULL == list || NULL == list->head) return;

    list_node_t *prev = NULL, *curr = list->head, *next = NULL;
    while (NULL != curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    list->head = prev;
}


void
list__insert(list_t *list,
             node_t value)
{
    list_node_t *new_node = (list_node_t *)calloc(1, sizeof(list_node_t));
    new_node->node = value;
    new_node->next = NULL;

    if (NULL == list->head) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }

    list->tail->next = new_node;
    list->tail = new_node;
}


void
list__merge(list_t *left,
            list_t *right)
{
    left->tail->next = right->head;
    left->tail = right->tail;
}


int
list__contains(list_t *list,
               node_t value,
               unsigned long long length)
{
    if (NULL == list) return 0;

    list_node_t *node = list->head;
    while (NULL != node) {
        if (value == node->node) return 1;
        if (0 == memcmp(value, node->node, length)) return 1;

        node = node->next;
    }

    return 0;
}
