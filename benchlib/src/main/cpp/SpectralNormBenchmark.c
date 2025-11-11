#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <android/log.h>

static double eval_A(int i, int j) {
    return 1.0 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

static void eval_A_times_u(int N, const double *u, double *Au) {
    int i, j;
    for (i = 0; i < N; i++) {
        Au[i] = 0;
        for (j = 0; j < N; j++) {
            Au[i] += eval_A(i, j) * u[j];
        }
    }
}

static void eval_At_times_u(int N, const double *u, double *Au) {
    int i, j;
    for (i = 0; i < N; i++) {
        Au[i] = 0;
        for (j = 0; j < N; j++) {
            Au[i] += eval_A(j, i) * u[j];
        }
    }
}

static void eval_AtA_times_u(int N, const double *u, double *AtAu) {
    double *v = (double *)malloc(N * sizeof(double));
    eval_A_times_u(N, u, v);
    eval_At_times_u(N, v, AtAu);
    free(v);
}

static double spectral_norm(int N) {
    int i;
    double *u = (double *)malloc(N * sizeof(double));
    double *v = (double *)malloc(N * sizeof(double));
    double vBv, vv;

    for (i = 0; i < N; i++) u[i] = 1;

    for (i = 0; i < 10; i++) {
        eval_AtA_times_u(N, u, v);
        eval_AtA_times_u(N, v, u);
    }

    vBv = vv = 0;
    for (i = 0; i < N; i++) {
        vBv += u[i] * v[i];
        vv += v[i] * v[i];
    }

    double result = sqrt(vBv / vv);

    free(u);
    free(v);

    return result;
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runSpectralNormBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 400;

    for (int iter = 0; iter < iterations; iter++) {
        spectral_norm(n);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "SpectralNorm C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "SpectralNorm C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}