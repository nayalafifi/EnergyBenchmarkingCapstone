#include <jni.h>
#include <vector>
#include <cmath>
#include <time.h>
#include <android/log.h>

// Match TCLBG's index calculation (bit shift optimization)
template <bool modei>
static inline int Index(int i, int j) {
    return (((i + j) * (i + j + 1)) >> 1) + (modei ? i : j) + 1;
}

// Evaluate A times u
static void eval_A_times_u(int N, const double* u, double* Au) {
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) {
            sum += u[j] / double(Index<true>(i, j));
        }
        Au[i] = sum;
    }
}

// Evaluate A^T times u
static void eval_At_times_u(int N, const double* u, double* Au) {
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) {
            sum += u[j] / double(Index<false>(i, j));
        }
        Au[i] = sum;
    }
}

// Evaluate A^T*A times u
static void eval_AtA_times_u(int N, const double* u, double* AtAu, double* tmp) {
    eval_A_times_u(N, u, tmp);
    eval_At_times_u(N, tmp, AtAu);
}

static double spectral_norm(int N) {
    // Use aligned arrays for better performance
    std::vector<double> u(N, 1.0);
    std::vector<double> v(N);
    std::vector<double> tmp(N);

    // Power iteration: 10 times
    for (int i = 0; i < 10; i++) {
        eval_AtA_times_u(N, u.data(), v.data(), tmp.data());
        eval_AtA_times_u(N, v.data(), u.data(), tmp.data());
    }

    // Calculate eigenvalue
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
    int iterations = 1;

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