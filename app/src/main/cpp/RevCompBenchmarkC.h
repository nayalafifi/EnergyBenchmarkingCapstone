//
// Created by Naya Alafifi on 15/09/2025.
//
#ifndef REVCOMPBENCHMARKC_H
#define REVCOMPBENCHMARKC_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runRevCompBenchmarkCpp(JNIEnv* env, jclass clazz, jstring jInput);

#ifdef __cplusplus
}
#endif

#endif // REVCOMPBENCHMARKC_H
