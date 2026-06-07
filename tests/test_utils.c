/*
 *      test_utils.c
 *
 *      Tests for utility functions (metric, sort).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "utils/metric.h"
#include "utils/sort.h"
#include "data_structures/node.h"

/* Test: Calculate metric
static int test_metric(void) {
    TEST_START("metric_calculate");

    unsigned int freq[256];
    memset(freq, 0, sizeof(freq));
    freq['A'] = 10;
    freq['B'] = 5;
    freq['C'] = 3;

    int result = metric_calculate(freq, 'A', 10);
    TEST_ASSERT(result >= 0, "metric returns non-negative");

    TEST_END;
    return 0;
}
*/

/* Test: Sort nodes */
static int test_sort_nodes(void) {
    TEST_START("sort_nodes");

    node *nodes[5];
    for (int i = 0; i < 5; i++) {
        nodes[i] = new_node(i, 'A' + i);
        TEST_ASSERT(nodes[i] != NULL, "node created");
    }

    int counts[] = {30, 10, 20, 5, 15};
    for (int i = 0; i < 5; i++) {
        nodes[i]->weight = counts[i];
    }

    sort_nodes_by_weight(nodes, 5);

    /* After sorting, nodes should be in descending order by weight */
    TEST_ASSERT(nodes[0]->weight >= nodes[4]->weight, "sorted descending");

    for (int i = 0; i < 5; i++) {
        delete_node(nodes[i]);
    }

    TEST_END;
    return 0;
}

/* Test: NULL pointer handling */
static int test_utils_null(void) {
    TEST_START("NULL pointer handling");

    sort_nodes_by_weight(NULL, 0);

    TEST_END;
    return 0;
}

int main(void) {
    printf("=== Utils Test Suite ===\n\n");

    int failures = 0;

    // failures += test_metric();
    failures += test_sort_nodes();
    failures += test_utils_null();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}