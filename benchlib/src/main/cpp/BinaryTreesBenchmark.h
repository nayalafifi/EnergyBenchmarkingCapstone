#ifndef MY_APPLICATION_BINARYTREESBENCHMARK_H
#define MY_APPLICATION_BINARYTREESBENCHMARK_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runBinaryTreesBenchmarkCpp(
        JNIEnv* env, jclass clazz, jint min_depth, jint max_depth);

#ifdef __cplusplus
}
#endif

#endif // MY_APPLICATION_BINARYTREESBENCHMARK_H
