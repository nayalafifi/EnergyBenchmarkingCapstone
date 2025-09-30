#include <jni.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <android/log.h>

static inline int A(int i, int j) {
    return ((i+j) * (i+j+1) / 2 + i + 1);
}

typedef struct {
    int start;
    int end;
    int n;
    const double* v;
    double* out;
    int transpose;
} thread_data_t;

static void* compute_section(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    for (int i = data->start; i < data->end; i++) {
        double sum = 0.0;
        for (int j = 0; j < data->n; j++) {
            sum += data->v[j] / (data->transpose ? A(j, i) : A(i, j));
        }
        data->out[i] = sum;
    }
    return NULL;
}

static void mult_Av_threaded(const double* v, double* out, int n, int num_threads) {
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));

    int chunk = n / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk;
        thread_data[i].n = n;
        thread_data[i].v = v;
        thread_data[i].out = out;
        thread_data[i].transpose = 0;
        pthread_create(&threads[i], NULL, compute_section, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_data);
}

static void mult_Atv_threaded(const double* v, double* out, int n, int num_threads) {
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));

    int chunk = n / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk;
        thread_data[i].n = n;
        thread_data[i].v = v;
        thread_data[i].out = out;
        thread_data[i].transpose = 1;
        pthread_create(&threads[i], NULL, compute_section, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_data);
}

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runSpectralNormBenchmarkC(
        JNIEnv *env, jclass clazz, jint inputN) {

    clock_t start = clock();

    int n = inputN > 0 ? inputN : 100;
    int num_threads = 8; // Match Java's 2 * cores (assuming 4 cores)

    // Run 5000 iterations to match Java
    for (int iteration = 0; iteration < 5000; iteration++) {
        double* u = malloc(n * sizeof(double));
        double* v = malloc(n * sizeof(double));
        double* tmp = malloc(n * sizeof(double));

        for (int i = 0; i < n; i++) u[i] = 1.0;

        for (int i = 0; i < 10; i++) {
            mult_Av_threaded(u, tmp, n, num_threads);
            mult_Atv_threaded(tmp, v, n, num_threads);
            mult_Av_threaded(v, tmp, n, num_threads);
            mult_Atv_threaded(tmp, u, n, num_threads);
        }

        free(u);
        free(v);
        free(tmp);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "SpectralNorm C duration: %ldms", duration);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "SpectralNorm C completed: %ldms", duration);
    return (*env)->NewStringUTF(env, buffer);
}