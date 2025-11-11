//
// Created by Naya Alafifi on 15/09/2025.
//

#ifndef SPECTRALNORMBENCHMARKC_H
#define SPECTRALNORMBENCHMARKC_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runSpectralNormBenchmarkCpp(JNIEnv* env, jclass clazz, jint n);

#ifdef __cplusplus
}
#endif

#endif // SPECTRALNORMBENCHMARKC_H
