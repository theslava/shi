/* Released to the public domain — No rights reserved. */

#include <stdio.h>
#include <stdlib.h>

#include "data_structures/tree.h"
#include "utils/metric.h"
#include "utils/sort.h"

/* Tree creation / destruction */
tree* new_tree(void) {
    tree* ret = (tree*)malloc(sizeof(tree));
    if (ret != NULL) {
        ret->root = NULL;
        /* Initialize all nodes in the array as empty */
        for (int i = 0; i < MAX_NODES; i++) {
            ret->nodes[i].left = NULL;
            ret->nodes[i].right = NULL;
            ret->nodes[i].byte = -1;
            ret->nodes[i].weight = 0;
            ret->nodes[i].next = NULL;
        }
    }
    return ret;
}

void delete_tree(tree* del) {
    if (del != NULL) {
        free(del);
    }
}

/*
 * Insert a new node into a sorted linked list.
 * The list is sorted in ascending order by weight.
 * For nodes with equal weight, leaf nodes (byte >= 0) come before parent nodes (byte < 0).
 *
 * Returns the new head of the list.
 */
node* insert_sorted(node* head, node* new_node) {
    /* Empty list */
    if (head == NULL) {
        new_node->next = NULL;
        return new_node;
    }

    /* If new node should come before head, insert at front */
    /* Leaf nodes come before parent nodes of same weight */
    int new_is_leaf = (new_node->byte >= 0);
    int head_is_leaf = (head->byte >= 0);

    if (new_node->weight < head->weight ||
        (new_node->weight == head->weight && new_is_leaf && !head_is_leaf)) {
        new_node->next = head;
        return new_node;
    }

    /* Find the correct position */
    node* current = head;
    while (current->next != NULL) {
        int next_is_leaf = (current->next->byte >= 0);

        if (new_node->weight < current->next->weight ||
            (new_node->weight == current->next->weight && new_is_leaf && !next_is_leaf)) {
            break;
        }
        current = current->next;
    }

    /* Insert after current */
    new_node->next = current->next;
    current->next = new_node;

    return head;
}

/*
 * Remove and return the first node from the list.
 * Returns NULL if the list is empty.
 */
static node* pop_front(node** head) {
    if (*head == NULL) {
        return NULL;
    }
    node* first = *head;
    *head = first->next;
    first->next = NULL;
    return first;
}

tree* new_tree_from_metric(metric* met) {
    if (!met)
        return NULL;

    /* Count distinct symbols with non-zero frequency */
    int num_distinct = 0;
    for (int i = 0; i < 256; i++) {
        if (met->characters[i] > 0) {
            num_distinct++;
        }
    }

    /* Check if all frequencies are zero */
    if (num_distinct == 0) {
        /* Create a tree with a single null leaf node */
        tree* ret = new_tree();
        if (!ret)
            return NULL;
        ret->nodes[0].left = NULL;
        ret->nodes[0].right = NULL;
        ret->nodes[0].byte = -1;
        ret->nodes[0].weight = 0;
        ret->root = &(ret->nodes[0]);
        return ret;
    }

    tree* ret = new_tree();
    if (!ret)
        return NULL;

    /* Step 1: Create leaf nodes and build sorted linked list */
    node* list_head = NULL;
    int leaf_count = 0;

    for (int i = 0; i < 256; i++) {
        if (met->characters[i] > 0) {
            ret->nodes[leaf_count].byte = i;
            ret->nodes[leaf_count].weight = met->characters[i];
            ret->nodes[leaf_count].left = NULL;
            ret->nodes[leaf_count].right = NULL;
            ret->nodes[leaf_count].next = NULL;
            list_head = insert_sorted(list_head, &(ret->nodes[leaf_count]));
            leaf_count++;
        }
    }

    /* Step 2: Build the Huffman tree using the linked list */
    int next_pnode = leaf_count;

    while (list_head != NULL && list_head->next != NULL) {
        /* Pop the two smallest nodes */
        node* left = pop_front(&list_head);
        node* right = pop_front(&list_head);

        /* Create a parent node */
        ret->nodes[next_pnode].byte = -1;
        ret->nodes[next_pnode].weight = left->weight + right->weight;
        ret->nodes[next_pnode].left = left;
        ret->nodes[next_pnode].right = right;
        ret->nodes[next_pnode].next = NULL;

        /* Insert the parent back into the sorted list */
        list_head = insert_sorted(list_head, &(ret->nodes[next_pnode]));
        next_pnode++;
    }

    /* The remaining node is the root */
    if (list_head != NULL) {
        ret->root = list_head;
    }

    return ret;
}

/* Free dynamically allocated nodes in the tree (post-order traversal).
 * Note: nodes from new_tree_from_metric() are embedded in tree->nodes[] and
 * must NOT be freed. This function is only used for trees built by
 * reconstruct_tree_from_codes() which allocates nodes via new_node(). */
void free_tree_nodes(node* root) {
    if (root == NULL)
        return;
    free_tree_nodes(root->left);
    free_tree_nodes(root->right);
    free(root);
}

/* Helper: recursive DFS to generate Huffman codes */
static void generate_codes_recursive(node* n,
                                     unsigned int current_code,
                                     int current_length,
                                     unsigned int codes[256],
                                     int code_lengths[256]) {
    if (n == NULL)
        return;

    /* Leaf node: store the code */
    if (n->byte >= 0) {
        codes[n->byte] = current_code;
        code_lengths[n->byte] = current_length;
        return;
    }

    /* Traverse left: append 0 */
    generate_codes_recursive(n->left, (current_code << 1) | 0, current_length + 1, codes,
                             code_lengths);
    /* Traverse right: append 1 */
    generate_codes_recursive(n->right, (current_code << 1) | 1, current_length + 1, codes,
                             code_lengths);
}

int generate_codes(tree* t, unsigned int codes[256], int code_lengths[256]) {
    if (!t || !t->root)
        return 0;

    /* Initialize arrays */
    for (int i = 0; i < 256; i++) {
        codes[i] = 0;
        code_lengths[i] = 0;
    }

    /* Handle single-symbol tree (root is a leaf) */
    if (t->root->byte >= 0) {
        codes[t->root->byte] = 0;
        code_lengths[t->root->byte] = 1;
        return 1;
    }
    /* Traverse the tree and assign codes */
    generate_codes_recursive(t->root, 0, 0, codes, code_lengths);

    /* Count symbols with non-zero frequency */
    int num_symbols = 0;
    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] > 0) {
            num_symbols++;
        }
    }
    return num_symbols;
}