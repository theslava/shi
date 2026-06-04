/*
 *      test_tree.c
 *
 *      Tests for the Huffman tree data structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "tree.h"
#include "node.h"

/* Test: Create and destroy a tree */
static int test_tree_new(void) {
    TEST_START("tree_new / tree_done");

    tree *t = tree_new();
    TEST_ASSERT(t != NULL, "tree_new returns non-NULL");

    tree_done(t);

    TEST_END;
    return 0;
}

/* Test: Insert a node */
static int test_tree_insert(void) {
    TEST_START("tree_insert");

    tree *t = tree_new();
    node *n = node_new(0, 'A');
    TEST_ASSERT(n != NULL, "node_new returns non-NULL");

    int result = tree_insert(t, n);
    TEST_ASSERT(result == 0, "tree_insert succeeds");

    tree_done(t);
    node_done(n);

    TEST_END;
    return 0;
}

/* Test: Build tree from frequencies */
static int test_tree_build(void) {
    TEST_START("tree_build from frequencies");

    unsigned int freq[256];
    memset(freq, 0, sizeof(freq));

    /* Simulate frequencies: 'A' = 10, 'B' = 5, 'C' = 3 */
    freq['A'] = 10;
    freq['B'] = 5;
    freq['C'] = 3;

    tree *t = tree_build(freq);
    TEST_ASSERT(t != NULL, "tree_build returns non-NULL");

    /* Root should not be NULL */
    TEST_ASSERT(t->root != NULL, "tree has root");

    tree_done(t);

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_tree_null(void) {
    TEST_START("NULL pointer handling");

    tree_done(NULL);  /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== Tree Test Suite ===\n\n");

    int failures = 0;

    failures += test_tree_new();
    failures += test_tree_insert();
    failures += test_tree_build();
    failures += test_tree_null();

    printf("\n=== Results: %d test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}