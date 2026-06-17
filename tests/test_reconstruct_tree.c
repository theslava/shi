/*
 *      test_reconstruct_tree.c
 *
 *      Unit tests for reconstruct_tree_from_codes() — verifies that
 *      trees are correctly reconstructed from code tables and that
 *      the reconstructed trees produce the same codes when traversed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_helpers.h"
#include "data_structures/tree.h"
#include "data_structures/node.h"
#include "core/decompress.h"

/* ---------------------------------------------------------------------------
 * Helper: count nodes in a tree via DFS
 * --------------------------------------------------------------------------- */
static int count_nodes(node* n) {
    if (!n)
        return 0;
    return 1 + count_nodes(n->left) + count_nodes(n->right);
}

/* ---------------------------------------------------------------------------
 * Helper: collect leaf byte values from a tree via DFS
 * Returns the number of leaves found.
 * --------------------------------------------------------------------------- */
static int collect_leaves(node* n, int* leaves, int max_leaves) {
    if (!n || max_leaves <= 0)
        return 0;
    if (n->byte >= 0) {
        *leaves++ = n->byte;
        return 1 + collect_leaves(n->left, leaves, max_leaves - 1) +
               collect_leaves(n->right, leaves, max_leaves - 1);
    }
    return collect_leaves(n->left, leaves, max_leaves) +
           collect_leaves(n->right, leaves, max_leaves);
}

/* ---------------------------------------------------------------------------
 * Helper: verify that a code can be decoded in a tree
 * Returns the decoded byte value, or -1 if the code is invalid.
 * --------------------------------------------------------------------------- */
static int decode_code(node* root, unsigned int code, int length) {
    node* current = root;
    for (int bit_pos = length - 1; bit_pos >= 0; bit_pos--) {
        int bit = (code >> bit_pos) & 1;
        if (bit == 0) {
            current = current->left;
        } else {
            current = current->right;
        }
        if (!current)
            return -1;
        if (current->byte >= 0)
            return current->byte;
    }
    return -1;
}

/* ---------------------------------------------------------------------------
 * Test 1: Reconstruct tree from two-symbol codes
 * Codes: A=0(len=1), B=1(len=1)
 * Expected tree: root → left: A, right: B
 * --------------------------------------------------------------------------- */
static int test_two_symbols(void) {
    TEST_START("reconstruct_tree: two symbols (A=0/1bit, B=1/1bit)");

    unsigned int codes[256];
    int code_lengths[256];
    memset(codes, 0, sizeof(codes));
    memset(code_lengths, 0, sizeof(code_lengths));

    codes['A'] = 0;
    code_lengths['A'] = 1;
    codes['B'] = 1;
    code_lengths['B'] = 1;

    node* root = reconstruct_tree_from_codes(codes, code_lengths, 2);
    TEST_ASSERT(root != NULL, "tree reconstructed successfully");

    /* Root should be a parent (byte < 0) */
    TEST_ASSERT(root->byte < 0, "root is a parent node");

    /* Root should have exactly 2 children */
    TEST_ASSERT(root->left != NULL, "root has left child");
    TEST_ASSERT(root->right != NULL, "root has right child");

    /* Left child should be leaf 'A' */
    TEST_ASSERT(root->left->byte == 'A', "left child is 'A'");
    TEST_ASSERT(root->left->byte >= 0, "left child is a leaf");

    /* Right child should be leaf 'B' */
    TEST_ASSERT(root->right->byte == 'B', "right child is 'B'");
    TEST_ASSERT(root->right->byte >= 0, "right child is a leaf");

    /* Total nodes should be 3 */
    TEST_ASSERT(count_nodes(root) == 3, "total nodes == 3");

    free_tree_nodes(root);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 2: Reconstruct tree from three-symbol codes
 * Codes: A=0(1bit), C=10(2bit), B=11(2bit)
 * Expected tree: root → left: A, right: parent → left: C, right: B
 * --------------------------------------------------------------------------- */
static int test_three_symbols(void) {
    TEST_START("reconstruct_tree: three symbols (A=0/1bit, C=10/2bit, B=11/2bit)");

    unsigned int codes[256];
    int code_lengths[256];
    memset(codes, 0, sizeof(codes));
    memset(code_lengths, 0, sizeof(code_lengths));

    codes['A'] = 0;
    code_lengths['A'] = 1;
    codes['C'] = 2; /* binary 10 */
    code_lengths['C'] = 2;
    codes['B'] = 3; /* binary 11 */
    code_lengths['B'] = 2;

    node* root = reconstruct_tree_from_codes(codes, code_lengths, 3);
    TEST_ASSERT(root != NULL, "tree reconstructed successfully");

    /* Root is a parent */
    TEST_ASSERT(root->byte < 0, "root is a parent node");

    /* Root has left child (A) and right child (internal) */
    TEST_ASSERT(root->left != NULL, "root has left child");
    TEST_ASSERT(root->left->byte == 'A', "left child is 'A'");

    TEST_ASSERT(root->right != NULL, "root has right child");
    TEST_ASSERT(root->right->byte < 0, "right child is internal");

    /* Right child has two leaf children: C and B */
    TEST_ASSERT(root->right->left != NULL, "right child has left");
    TEST_ASSERT(root->right->right != NULL, "right child has right");
    TEST_ASSERT(root->right->left->byte == 'C', "right-left is 'C'");
    TEST_ASSERT(root->right->right->byte == 'B', "right-right is 'B'");

    /* Total nodes should be 5 */
    TEST_ASSERT(count_nodes(root) == 5, "total nodes == 5");

    free_tree_nodes(root);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 3: Reconstruct tree from single-symbol code
 * Code: X=0(1bit)
 * Expected: root is a leaf with byte='X'
 * --------------------------------------------------------------------------- */
static int test_single_symbol(void) {
    TEST_START("reconstruct_tree: single symbol (X=0/1bit)");

    unsigned int codes[256];
    int code_lengths[256];
    memset(codes, 0, sizeof(codes));
    memset(code_lengths, 0, sizeof(code_lengths));

    codes['X'] = 0;
    code_lengths['X'] = 1;

    node* root = reconstruct_tree_from_codes(codes, code_lengths, 1);
    TEST_ASSERT(root != NULL, "tree reconstructed successfully");

    /* Root is a parent (byte < 0) with a single leaf child */
    TEST_ASSERT(root->byte < 0, "root is a parent node");
    TEST_ASSERT(root->left != NULL, "root has left child");
    TEST_ASSERT(root->left->byte == 'X', "left child is leaf 'X'");
    TEST_ASSERT(root->right == NULL, "no right child");

    free_tree_nodes(root);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 4: Roundtrip — generate_codes then reconstruct should produce
 * a tree that decodes the same codes.
 * --------------------------------------------------------------------------- */
static int test_roundtrip(void) {
    TEST_START("reconstruct_tree: roundtrip (generate → reconstruct → decode)");

    /* Build a tree with known frequencies */
    unsigned int freq[256];
    memset(freq, 0, sizeof(freq));
    freq['A'] = 10;
    freq['B'] = 5;
    freq['C'] = 3;
    freq['D'] = 2;

    tree* t = new_tree_from_metric((metric*)NULL);
    /* Build properly using metric */
    metric* met = (metric*)malloc(sizeof(metric));
    memset(met->characters, 0, sizeof(met->characters));
    memcpy(met->characters, freq, sizeof(freq));
    delete_tree(t);
    t = new_tree_from_metric(met);
    free(met);
    TEST_ASSERT(t != NULL, "tree built");
    TEST_ASSERT(t->root != NULL, "tree has root");

    /* Generate codes from the tree */
    unsigned int original_codes[256];
    int original_lengths[256];
    int num_symbols = generate_codes(t, original_codes, original_lengths);
    TEST_ASSERT(num_symbols > 0, "codes generated");

    /* Reconstruct a tree from those codes */
    node* reconstructed =
        reconstruct_tree_from_codes(original_codes, original_lengths, num_symbols);
    TEST_ASSERT(reconstructed != NULL, "tree reconstructed from codes");

    /* Verify: decoding each original code in the reconstructed tree
     * should yield the same byte value */
    for (int i = 0; i < 256; i++) {
        if (original_lengths[i] > 0) {
            int decoded = decode_code(reconstructed, original_codes[i], original_lengths[i]);
            TEST_ASSERT(decoded == i, "decoded byte matches original for symbol");
        }
    }

    free_tree_nodes(reconstructed);
    delete_tree(t);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 5: NULL and edge case handling
 * --------------------------------------------------------------------------- */
static int test_null_handling(void) {
    TEST_START("reconstruct_tree: NULL handling");

    unsigned int codes[256];
    int code_lengths[256];
    memset(codes, 0, sizeof(codes));
    memset(code_lengths, 0, sizeof(code_lengths));

    /* NULL codes → should return NULL */
    node* result = reconstruct_tree_from_codes(NULL, code_lengths, 2);
    TEST_ASSERT(result == NULL, "NULL codes returns NULL");

    /* NULL code_lengths → should return NULL */
    result = reconstruct_tree_from_codes(codes, NULL, 2);
    TEST_ASSERT(result == NULL, "NULL code_lengths returns NULL");

    /* num_symbols <= 0 → should return NULL */
    result = reconstruct_tree_from_codes(codes, code_lengths, 0);
    TEST_ASSERT(result == NULL, "num_symbols=0 returns NULL");

    result = reconstruct_tree_from_codes(codes, code_lengths, -1);
    TEST_ASSERT(result == NULL, "num_symbols=-1 returns NULL");

    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 6: Reconstruct from codes that produce a deep tree (5 symbols)
 * --------------------------------------------------------------------------- */
static int test_deep_tree(void) {
    TEST_START("reconstruct_tree: deep tree (5 symbols)");

    unsigned int codes[256];
    int code_lengths[256];
    memset(codes, 0, sizeof(codes));
    memset(code_lengths, 0, sizeof(code_lengths));

    /* Manually define codes that produce a deep tree:
     * A=0(1bit), B=10(2bit), C=110(3bit), D=1110(4bit), E=1111(4bit) */
    codes['A'] = 0;
    code_lengths['A'] = 1;
    codes['B'] = 2; /* 10 */
    code_lengths['B'] = 2;
    codes['C'] = 6; /* 110 */
    code_lengths['C'] = 3;
    codes['D'] = 14; /* 1110 */
    code_lengths['D'] = 4;
    codes['E'] = 15; /* 1111 */
    code_lengths['E'] = 4;

    node* root = reconstruct_tree_from_codes(codes, code_lengths, 5);
    TEST_ASSERT(root != NULL, "deep tree reconstructed");

    /* Verify all leaves are correct */
    int leaves[10];
    int num_leaves = collect_leaves(root, leaves, 10);
    TEST_ASSERT(num_leaves == 5, "exactly 5 leaves");

    /* Verify decoding works for each code */
    TEST_ASSERT(decode_code(root, 0, 1) == 'A', "decode code 0/1bit → 'A'");
    TEST_ASSERT(decode_code(root, 2, 2) == 'B', "decode code 2/2bit → 'B'");
    TEST_ASSERT(decode_code(root, 6, 3) == 'C', "decode code 6/3bit → 'C'");
    TEST_ASSERT(decode_code(root, 14, 4) == 'D', "decode code 14/4bit → 'D'");
    TEST_ASSERT(decode_code(root, 15, 4) == 'E', "decode code 15/4bit → 'E'");

    free_tree_nodes(root);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Test 7: Reconstruct from all 256 symbols (equal frequency tree)
 * --------------------------------------------------------------------------- */
static int test_all_256(void) {
    TEST_START("reconstruct_tree: all 256 symbols");

    /* First build a tree to get valid codes */
    unsigned int freq[256];
    for (int i = 0; i < 256; i++)
        freq[i] = 1;

    metric* met = (metric*)malloc(sizeof(metric));
    memset(met->characters, 0, sizeof(met->characters));
    memcpy(met->characters, freq, sizeof(freq));
    tree* t = new_tree_from_metric(met);
    free(met);
    TEST_ASSERT(t != NULL, "tree built for 256 symbols");

    unsigned int codes[256];
    int code_lengths[256];
    int num_symbols = generate_codes(t, codes, code_lengths);
    TEST_ASSERT(num_symbols > 0, "symbols generated");

    delete_tree(t);

    /* Now reconstruct from those codes */
    node* root = reconstruct_tree_from_codes(codes, code_lengths, num_symbols);
    TEST_ASSERT(root != NULL, "256-symbol tree reconstructed");

    /* Verify all symbols with codes can be decoded */
    int decoded_count = 0;
    for (int i = 0; i < 256; i++) {
        if (code_lengths[i] > 0) {
            int decoded = decode_code(root, codes[i], code_lengths[i]);
            TEST_ASSERT(decoded == i, "decoded symbol matches");
            decoded_count++;
        }
    }
    TEST_ASSERT(decoded_count > 0, "at least some symbols decoded");

    free_tree_nodes(root);
    TEST_END;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Main
 * --------------------------------------------------------------------------- */
int main(void) {
    printf("=== reconstruct_tree_from_codes() Unit Test Suite ===\n\n");

    int failures = 0;

    failures += test_two_symbols();
    failures += test_three_symbols();
    failures += test_single_symbol();
    failures += test_roundtrip();
    failures += test_null_handling();
    failures += test_deep_tree();
    failures += test_all_256();

    printf("\n=== Results: %s test(s) ===", failures == 0 ? "ALL PASSED" : "SOME FAILED");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
