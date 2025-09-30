#include <jni.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <android/log.h>

typedef intptr_t intnative_t;

typedef struct tree_node {
    struct tree_node *left_Node, *right_Node;
} tree_node;

static inline tree_node *create_Tree(const intnative_t depth) {
    tree_node *root_Node = malloc(sizeof(tree_node));
    if (depth > 0) {
        root_Node->left_Node = create_Tree(depth - 1);
        root_Node->right_Node = create_Tree(depth - 1);
    } else {
        root_Node->left_Node = root_Node->right_Node = NULL;
    }
    return root_Node;
}

static inline intnative_t compute_Tree_Checksum(const tree_node *root_Node) {
    if (root_Node->left_Node)
        return compute_Tree_Checksum(root_Node->left_Node) + compute_Tree_Checksum(root_Node->right_Node) + 1;
    else
        return 1;
}

static void free_Tree(tree_node *root_Node) {
    if (root_Node->left_Node) {
        free_Tree(root_Node->left_Node);
        free_Tree(root_Node->right_Node);
    }
    free(root_Node);
}

JNIEXPORT jstring JNICALL Java_com_example_myapplication_NativeBenchmarks_runBinaryTreesBenchmarkC(
        JNIEnv *env, jclass clazz, jint minDepth, jint maxDepth
) {
    clock_t start = clock();

    const intnative_t minimum_Tree_Depth = (minDepth < 4) ? 4 : (intnative_t)minDepth;
    intnative_t maximum_Tree_Depth = (maxDepth < minimum_Tree_Depth + 2) ? (minimum_Tree_Depth + 2) : (intnative_t)maxDepth;

    char output_buffer[1024] = {0};

    // Run 150 iterations to match Java
    for (int iteration = 0; iteration < 50; iteration++) {
        // Stretch tree
        tree_node *stretch_Tree = create_Tree(maximum_Tree_Depth + 1);
        compute_Tree_Checksum(stretch_Tree);
        free_Tree(stretch_Tree);

        // Long-lived tree
        tree_node *long_Lived_Tree = create_Tree(maximum_Tree_Depth);

        for (intnative_t depth = minimum_Tree_Depth; depth <= maximum_Tree_Depth; depth += 2) {
            intnative_t iterations = 1 << (maximum_Tree_Depth - depth + minimum_Tree_Depth);
            for (intnative_t i = 0; i < iterations; i++) {
                tree_node *t = create_Tree(depth);
                compute_Tree_Checksum(t);
                free_Tree(t);
            }
        }

        free_Tree(long_Lived_Tree);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "BinaryTrees C duration: %ldms", duration);

    snprintf(output_buffer, sizeof(output_buffer), "BinaryTrees C completed: %ldms", duration);
    return (*env)->NewStringUTF(env, output_buffer);
}