#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>

typedef intptr_t intnative_t;

typedef struct tree_node {
    struct tree_node *left_Node, *right_Node;
} tree_node;

// Create a binary tree of depth tree_Depth
static inline tree_node* create_Tree(const intnative_t tree_Depth) {
    tree_node* const root_Node = (tree_node*)malloc(sizeof(tree_node));

    if (tree_Depth > 0) {
        root_Node->left_Node = create_Tree(tree_Depth - 1);
        root_Node->right_Node = create_Tree(tree_Depth - 1);
    } else {
        root_Node->left_Node = root_Node->right_Node = NULL;
    }

    return root_Node;
}

// Compute and return the checksum for the binary tree
static inline intnative_t compute_Tree_Checksum(const tree_node* const root_Node) {
    if (root_Node->left_Node)
        return compute_Tree_Checksum(root_Node->left_Node) +
               compute_Tree_Checksum(root_Node->right_Node) + 1;
    else
        return 1;
}

// Delete the entire tree
static void delete_Tree(tree_node* tree) {
    if (tree->left_Node != NULL) {
        delete_Tree(tree->left_Node);
        delete_Tree(tree->right_Node);
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
    int iterations = 10;

    for (int iter = 0; iter < iterations; iter++) {
        const intnative_t minimum_Tree_Depth = 4;
        intnative_t maximum_Tree_Depth = maxDepth;
        if (maximum_Tree_Depth < minimum_Tree_Depth + 2)
            maximum_Tree_Depth = minimum_Tree_Depth + 2;

        // Stretch tree
        tree_node* stretch_Tree = create_Tree(maximum_Tree_Depth + 1);
        intnative_t stretch_check = compute_Tree_Checksum(stretch_Tree);
        delete_Tree(stretch_Tree);

        // Long-lived tree
        tree_node* long_Lived_Tree = create_Tree(maximum_Tree_Depth);

        // Create trees of varying depths
        for (intnative_t current_Tree_Depth = minimum_Tree_Depth;
             current_Tree_Depth <= maximum_Tree_Depth;
             current_Tree_Depth += 2) {

            intnative_t iterations_inner = 1 << (maximum_Tree_Depth - current_Tree_Depth +
                                                 minimum_Tree_Depth);
            intnative_t total_Trees_Checksum = 0;

            for (intnative_t i = 1; i <= iterations_inner; ++i) {
                tree_node* const tree_1 = create_Tree(current_Tree_Depth);
                total_Trees_Checksum += compute_Tree_Checksum(tree_1);
                delete_Tree(tree_1);
            }
        }

        // Check long-lived tree
        intnative_t long_lived_check = compute_Tree_Checksum(long_Lived_Tree);
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