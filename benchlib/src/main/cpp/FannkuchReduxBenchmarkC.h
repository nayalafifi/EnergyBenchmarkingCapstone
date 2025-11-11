#ifndef FANNKUCHREDUX_H
#define FANNKUCHREDUX_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runFannkuchReduxBenchmarkCpp(JNIEnv* env, jclass clazz, jint n);

#ifdef __cplusplus
}
#endif

#endif // FANNKUCHREDUX_H
