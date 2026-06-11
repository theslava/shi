#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "data_structures/tree.h"
#include "data_structures/node.h"

/* Test: Create and destroy a tree */
static int test_tree_new(void) {
    TEST_START("tree_new / tree_done");

    tree *t = new_tree();
    TEST_ASSERT(t != NULL, "tree_new returns non-NULL");

    delete_tree(t);

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

    metric *met = new_metric();
    TEST_ASSERT(met != NULL, "new_metric returns non-NULL");

    memcpy(met->characters, freq, sizeof(freq));

    tree *t = new_tree_from_metric(met);
    TEST_ASSERT(t != NULL, "new_tree_from_metric returns non-NULL");
    TEST_ASSERT(t->root != NULL, "tree has root");

    delete_tree(t);
    delete_metric(met);

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_tree_null(void) {
    TEST_START("NULL pointer handling");

    delete_tree(NULL);  /* Should not crash */

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== Tree Test Suite ===\n\n");

    int failures = 0;

    failures += test_tree_new();
    failures += test_tree_build();
    failures += test_tree_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}