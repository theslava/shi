/* Released to the public domain — No rights reserved. */

#ifndef __list_h__
#define __list_h__

#include "data_structures/node.h"
#include "utils/metric.h"
typedef struct _list {
    node* head;
    int count;
} list;

/* List creation / destruction */
list* new_list(void);
void delete_list(list* l);
list* new_list_from_array(node** array, int count);
void insert_node(list* l, node* n, metric* m);
node* remove_node(list* l);

/* Iteration helpers */
int list_size(const list* l);
node* list_get_head(const list* l);
list* list_append(list* l, node* n);

#endif
