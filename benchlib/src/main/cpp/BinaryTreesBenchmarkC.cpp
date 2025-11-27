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
        else
            return 1;
    }
};

TreeNode* make(int d) {
    if (d > 0)
        return new TreeNode(make(d - 1), make(d - 1));
    else
        return new TreeNode(nullptr, nullptr);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runBinaryTreesBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint minDepth,
        jint maxDepth) {

    clock_t start = clock();
    int iterations = 1;

    for (int iter = 0; iter < iterations; iter++) {
        int min_depth = 4;
        int max_depth = std::max(min_depth + 2, (int)maxDepth);
        int stretch_depth = max_depth + 1;

        // Stretch tree
        {
            TreeNode* stretch_tree = make(stretch_depth);
            int check = stretch_tree->check();
            delete stretch_tree;
        }

        // Long-lived tree
        TreeNode* long_lived_tree = make(max_depth);

        // Create trees of varying depths
        for (int d = min_depth; d <= max_depth; d += 2) {
            int iters = 1 << (max_depth - d + min_depth);
            int c = 0;

            for (int i = 1; i <= iters; ++i) {
                TreeNode* a = make(d);
                c += a->check();
                delete a;
            }
        }

        // Check long-lived tree
        int long_lived_check = long_lived_tree->check();
        delete long_lived_tree;
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