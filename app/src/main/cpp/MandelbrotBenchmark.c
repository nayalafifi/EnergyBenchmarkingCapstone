#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    int n;
    int y_start;
    int y_end;
    unsigned char** out;
    double* Crb;
    double* Cib;
} thread_data_t;

static int get_byte(int x, int y, double* Crb, double* Cib, int n) {
    int res = 0;
    for (int i = 0; i < 8; i += 2) {
        double Zr1 = Crb[x + i];
        double Zi1 = Cib[y];
        double Zr2 = Crb[x + i + 1];
        double Zi2 = Cib[y];

        int b = 0;
        int j = 49;
        do {
            double nZr1 = Zr1 * Zr1 - Zi1 * Zi1 + Crb[x + i];
            double nZi1 = Zr1 * Zi1 + Zr1 * Zi1 + Cib[y];
            Zr1 = nZr1;
            Zi1 = nZi1;

            double nZr2 = Zr2 * Zr2 - Zi2 * Zi2 + Crb[x + i + 1];
            double nZi2 = Zr2 * Zi2 + Zr2 * Zi2 + Cib[y];
            Zr2 = nZr2;
            Zi2 = nZi2;

            if (Zr1 * Zr1 + Zi1 * Zi1 > 4) {
                b |= 2;
                if (b == 3) break;
            }
            if (Zr2 * Zr2 + Zi2 * Zi2 > 4) {
                b |= 1;
                if (b == 3) break;
            }
        } while (--j > 0);
        res = (res << 2) + b;
    }
    return res ^ -1;
}

void* compute_rows(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    for (int y = data->y_start; y < data->y_end; y++) {
        for (int xb = 0; xb < (data->n + 7) / 8; xb++) {
            data->out[y][xb] = (unsigned char)get_byte(xb * 8, y, data->Crb, data->Cib, data->n);
        }
    }
    return NULL;
}

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runMandelbrotBenchmarkC(
        JNIEnv *env, jclass clazz, jint n) {

    clock_t start = clock();

    int num_threads = 8; // Match Java's thread count

    // Run 100 iterations to match Java
    for (int iteration = 0; iteration < 100; iteration++) {
        double* Crb = (double*)malloc((n + 7) * sizeof(double));
        double* Cib = (double*)malloc((n + 7) * sizeof(double));
        double invN = 2.0 / n;

        for (int i = 0; i < n; i++) {
            Cib[i] = i * invN - 1.0;
            Crb[i] = i * invN - 1.5;
        }

        unsigned char** out = (unsigned char**)malloc(n * sizeof(unsigned char*));
        for (int i = 0; i < n; i++) {
            out[i] = (unsigned char*)malloc((n + 7) / 8);
        }

        pthread_t threads[num_threads];
        thread_data_t thread_data[num_threads];

        int rows_per_thread = n / num_threads;
        for (int i = 0; i < num_threads; i++) {
            thread_data[i].n = n;
            thread_data[i].y_start = i * rows_per_thread;
            thread_data[i].y_end = (i == num_threads - 1) ? n : (i + 1) * rows_per_thread;
            thread_data[i].out = out;
            thread_data[i].Crb = Crb;
            thread_data[i].Cib = Cib;
            pthread_create(&threads[i], NULL, compute_rows, &thread_data[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // Cleanup
        for (int i = 0; i < n; i++) {
            free(out[i]);
        }
        free(out);
        free(Crb);
        free(Cib);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "Mandelbrot C duration: %ldms", duration);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Mandelbrot C completed: %ldms", duration);
    return (*env)->NewStringUTF(env, buffer);
}