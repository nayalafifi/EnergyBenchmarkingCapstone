#include <jni.h>
#include <memory>
#include <algorithm>
#include <time.h>
#include <android/log.h>

struct TreeNode {
    TreeNode *left, *right;

    TreeNode(TreeNode* l, TreeNode* r) : left(l), right(r) {}

    ~TreeNode() {
        delete left;
        delete right;
    }

    int check() const {
        if (left)
            return left->check() + right->check() + 1;
        return 1;
    }
};

TreeNode* make(int depth) {
    if (depth > 0)
        return new TreeNode(make(depth-1), make(depth-1));
    return new TreeNode(nullptr, nullptr);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runBinaryTreesBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint minDepth,
        jint maxDepth) {

    clock_t start = clock();
    int iterations = 35;

    for (int iter = 0; iter < iterations; iter++) {
        int stretchDepth = maxDepth + 1;

        {
            TreeNode* stretchTree = make(stretchDepth);
            stretchTree->check();
            delete stretchTree;
        }

        TreeNode* longLivedTree = make(maxDepth);

        for (int depth = minDepth; depth <= maxDepth; depth += 2) {
            int iterations_inner = 1 << (maxDepth - depth + minDepth);
            int check_sum = 0;

            for (int i = 1; i <= iterations_inner; i++) {
                TreeNode* tree = make(depth);
                check_sum += tree->check();
                delete tree;
            }
        }

        delete longLivedTree;
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "BinaryTrees C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "BinaryTrees C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}