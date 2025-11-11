#ifndef FASTABENCHMARKC_H
#define FASTABENCHMARKC_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runFastaBenchmarkCpp(JNIEnv *env, jclass clazz, jint n);

#ifdef __cplusplus
}
#endif

#endif // FASTABENCHMARKC_H
