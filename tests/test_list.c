/*
 *      test_list.c
 *
 *      Tests for the linked list data structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "list.h"
#include "node.h"

/* Test: Create and destroy a list */
static int test_list_new(void) {
    TEST_START("list_new / list_done");

    list *l = list_new();
    TEST_ASSERT(l != NULL, "list_new returns non-NULL");

    list_done(l);

    TEST_END;
    return 0;
}

/* Test: Append to list */
static int test_list_append(void) {
    TEST_START("list_append");

    list *l = list_new();
    node *n1 = node_new(1, 'A');
    node *n2 = node_new(2, 'B');
    TEST_ASSERT(n1 != NULL && n2 != NULL, "nodes created");

    list_append(l, n1);
    list_append(l, n2);

    TEST_ASSERT(list_count(l) == 2, "list has 2 elements");

    list_done(l);
    node_done(n1);
    node_done(n2);

    TEST_END;
    return 0;
}

/* Test: Pop from list */
static int test_list_pop(void) {
    TEST_START("list_pop");

    list *l = list_new();
    node *n = node_new(1, 'A');
    TEST_ASSERT(n != NULL, "node created");

    list_append(l, n);
    TEST_ASSERT(list_count(l) == 1, "list has 1 element");

    node *popped = list_pop(l);
    TEST_ASSERT(popped != NULL, "list_pop returns non-NULL");
    TEST_ASSERT(list_count(l) == 0, "list is empty after pop");

    list_done(l);
    node_done(popped);

    TEST_END;
    return 0;
}

/* Test: Pop from empty list */
static int test_list_pop_empty(void) {
    TEST_START("list_pop from empty list");

    list *l = list_new();
    node *popped = list_pop(l);
    TEST_ASSERT(popped == NULL, "list_pop returns NULL on empty list");

    list_done(l);

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_list_null(void) {
    TEST_START("NULL pointer handling");

    list_done(NULL);  /* Should not crash */

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