#include "BinaryTreesBenchmark.h"
#include <string>
#include <sstream>
#include <android/log.h>
#include <time.h>

struct Node {
    Node* left;
    Node* right;
    Node() : left(nullptr), right(nullptr) {}

    int check() const {
        if (left) return left->check() + 1 + right->check();
        else return 1;
    }
};

Node* make(int d) {
    Node* node = new Node();
    if (d > 0) {
        node->left = make(d - 1);
        node->right = make(d - 1);
    }
    return node;
}

void deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runBinaryTreesBenchmarkCpp(
        JNIEnv* env, jclass clazz, jint min_depth, jint max_depth) {

    clock_t start = clock();

    // Run 150 iterations to match Java
    for (int iteration = 0; iteration < 50; iteration++) {
        int stretch_depth = max_depth + 1;
        Node* stretch_tree = make(stretch_depth);
        stretch_tree->check();
        deleteTree(stretch_tree);

        Node* long_lived_tree = make(max_depth);

        for (int depth = min_depth; depth <= max_depth; depth += 2) {
            int iterations = 1 << (max_depth - depth + min_depth);
            int check = 0;
            for (int i = 0; i < iterations; ++i) {
                Node* tree = make(depth);
                check += tree->check();
                deleteTree(tree);
            }
        }

        deleteTree(long_lived_tree);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "BinaryTrees C++ duration: %ldms", duration);

    std::stringstream output;
    output << "BinaryTrees C++ completed: " << duration << "ms";
    return env->NewStringUTF(output.str().c_str());
}