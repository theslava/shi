#include "utils/sort.h"
#include "data_structures/node.h"

/* Sort an array of node pointers by weight (ascending) */
void sort_nodes_by_weight(node** nodes, int count) {
    if (!nodes || count <= 0)
        return;

    int i, j;
    /* Simple insertion sort (sufficient for count <= 256) */
    for (i = 1; i < count; i++) {
        node* key = nodes[i];
        j = i - 1;
        while (j >= 0 && compare_nodes(nodes[j], key) == 1) {
            nodes[j + 1] = nodes[j];
            j--;
        }
        nodes[j + 1] = key;
    }
}
