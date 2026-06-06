/*
 *      test_list.c
 *
 *      Tests for the linked list data structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "data_structures/list.h"
#include "data_structures/node.h"

/* Test: Create and destroy a list */
static int test_list_new(void) {
    TEST_START("new_list / list_done");

    list *l = new_list();
    TEST_ASSERT(l != NULL, "new_list returns non-NULL");

    delete_list(l);

    TEST_END;
    return 0;
}

/* Test: Append to list */
static int test_list_append(void) {
    TEST_START("list_append");

    list *l = new_list();
    node *n1 = new_node(1, 'A');
    node *n2 = new_node(2, 'B');
    TEST_ASSERT(n1 != NULL && n2 != NULL, "nodes created");

    list_append(l, n1);
    list_append(l, n2);

    TEST_ASSERT(list_size(l) == 2, "list has 2 elements");

    delete_list(l);
    delete_node(n1);
    delete_node(n2);

    TEST_END;
    return 0;
}

/* Test: Pop from list */
static int test_list_pop(void) {
    TEST_START("list_pop");

    list *l = new_list();
    node *n = new_node(1, 'A');
    TEST_ASSERT(n != NULL, "node created");

    list_append(l, n);
    TEST_ASSERT(list_size(l) == 1, "list has 1 element");

    node *popped = remove_node(l);
    TEST_ASSERT(popped != NULL, "remove_node returns non-NULL");
    TEST_ASSERT(list_size(l) == 0, "list is empty after pop");

    delete_list(l);
    delete_node(popped);

    TEST_END;
    return 0;
}

/* Test: Pop from empty list */
static int test_list_pop_empty(void) {
    TEST_START("list_pop from empty list");

    list *l = new_list();
    node *popped = remove_node(l);
    TEST_ASSERT(popped == NULL, "remove_node returns NULL on empty list");

    delete_list(l);

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_list_null(void) {
    TEST_START("NULL pointer handling");

    delete_list(NULL);  /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== List Test Suite ===\n\n");

    int failures = 0;

    failures += test_list_new();
    failures += test_list_append();
    failures += test_list_pop();
    failures += test_list_pop_empty();
    failures += test_list_null();

    printf("\n=== Results: %d test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}