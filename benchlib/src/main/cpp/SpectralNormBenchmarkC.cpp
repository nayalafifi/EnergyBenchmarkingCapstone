#include <jni.h>
#include <vector>
#include <cmath>
#include <time.h>
#include <android/log.h>

static double eval_A(int i, int j) {
    return 1.0 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

static void eval_A_times_u(int N, const std::vector<double>& u, std::vector<double>& Au) {
    for (int i = 0; i < N; i++) {
        Au[i] = 0;
        for (int j = 0; j < N; j++) {
            Au[i] += eval_A(i, j) * u[j];
        }
    }
}

static void eval_At_times_u(int N, const std::vector<double>& u, std::vector<double>& Au) {
    for (int i = 0; i < N; i++) {
        Au[i] = 0;
        for (int j = 0; j < N; j++) {
            Au[i] += eval_A(j, i) * u[j];
        }
    }
}

static void eval_AtA_times_u(int N, const std::vector<double>& u, std::vector<double>& AtAu) {
    std::vector<double> v(N);
    eval_A_times_u(N, u, v);
    eval_At_times_u(N, v, AtAu);
}

static double spectral_norm(int N) {
    std::vector<double> u(N, 1.0);
    std::vector<double> v(N);

    for (int i = 0; i < 10; i++) {
        eval_AtA_times_u(N, u, v);
        eval_AtA_times_u(N, v, u);
    }

    double vBv = 0.0, vv = 0.0;
    for (int i = 0; i < N; i++) {
        vBv += u[i] * v[i];
        vv += v[i] * v[i];
    }

    return std::sqrt(vBv / vv);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runSpectralNormBenchmarkCpp(
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
                        "SpectralNorm C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "SpectralNorm C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}