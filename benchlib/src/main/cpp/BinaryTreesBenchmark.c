#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>
#include <inttypes.h>

typedef struct tree_node {
    struct tree_node *left;
    struct tree_node *right;
} tree_node;

// Create a binary tree of given depth
static tree_node* create_Tree(intptr_t depth) {
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));

    if (depth > 0) {
        node->left = create_Tree(depth - 1);
        node->right = create_Tree(depth - 1);
    } else {
        node->left = NULL;
        node->right = NULL;
    }

    return node;
}

// Compute checksum for the tree
static intptr_t compute_Tree_Checksum(const tree_node* root) {
    if (root->left != NULL) {
        return compute_Tree_Checksum(root->left) +
               compute_Tree_Checksum(root->right) + 1;
    } else {
        return 1;
    }
}

// Delete the entire tree
static void delete_Tree(tree_node* tree) {
    if (tree->left != NULL) {
        delete_Tree(tree->left);
        delete_Tree(tree->right);
    }
    free(tree);
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runBinaryTreesBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint minDepth,
        jint maxDepth) {

    clock_t start = clock();
    int iterations = 30;

    for (int iter = 0; iter < iterations; iter++) {
        // Set minimum and maximum depths according to CLBG algorithm
        const intptr_t minimum_Tree_Depth = 4;
        intptr_t maximum_Tree_Depth = maxDepth;
        if (maximum_Tree_Depth < minimum_Tree_Depth + 2) {
            maximum_Tree_Depth = minimum_Tree_Depth + 2;
        }

        // Create stretch tree of depth maximum_Tree_Depth + 1
        intptr_t stretch_depth = maximum_Tree_Depth + 1;
        tree_node* stretch_Tree = create_Tree(stretch_depth);
        intptr_t stretch_check = compute_Tree_Checksum(stretch_Tree);
        delete_Tree(stretch_Tree);

        // Create long-lived tree
        tree_node* long_Lived_Tree = create_Tree(maximum_Tree_Depth);

        // Create many trees of varying depths
        for (intptr_t current_Tree_Depth = minimum_Tree_Depth;
             current_Tree_Depth <= maximum_Tree_Depth;
             current_Tree_Depth += 2) {

            intptr_t iterations_inner = 1 << (maximum_Tree_Depth - current_Tree_Depth +
                                              minimum_Tree_Depth);
            intptr_t total_Trees_Checksum = 0;

            for (intptr_t i = 1; i <= iterations_inner; i++) {
                tree_node* temp_tree = create_Tree(current_Tree_Depth);
                total_Trees_Checksum += compute_Tree_Checksum(temp_tree);
                delete_Tree(temp_tree);
            }
        }

        // Check long-lived tree
        intptr_t long_lived_check = compute_Tree_Checksum(long_Lived_Tree);
        delete_Tree(long_Lived_Tree);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "BinaryTrees C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "BinaryTrees C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}
