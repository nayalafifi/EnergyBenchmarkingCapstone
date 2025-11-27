#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>

// Mandelbrot for 8 pixels packed into one byte
static unsigned char mandelbrot_calc_byte(double Cr_base, double Ci, int w) {
    unsigned char byte = 0;

    for (int bit = 0; bit < 8; bit++) {
        double Cr = Cr_base + (2.0 * bit / w);
        double Zi = 0.0;
        double Zr = 0.0;
        double Tr, Ti;

        int escaped = 0;

        // Match TCLBG C structure: 6 batches of 8 iterations
        for (int batch = 0; batch < 6; batch++) {
            for (int k = 0; k < 8; k++) {
                Tr = Zr * Zr;
                Ti = Zi * Zi;
                Zi = 2.0 * Zr * Zi + Ci;
                Zr = Tr - Ti + Cr;
            }
            // Check escape after every 8 iterations
            Tr = Zr * Zr;
            Ti = Zi * Zi;
            if (Tr + Ti > 4.0) {
                escaped = 1;
                break;
            }
        }

        // If not escaped, do 2 more iterations (48 + 2 = 50 total)
        if (!escaped) {
            Tr = Zr * Zr;
            Ti = Zi * Zi;
            Zi = 2.0 * Zr * Zi + Ci;
            Zr = Tr - Ti + Cr;

            Tr = Zr * Zr;
            Ti = Zi * Zi;
            Zi = 2.0 * Zr * Zi + Ci;
            Zr = Tr - Ti + Cr;

            Tr = Zr * Zr;
            Ti = Zi * Zi;
            if (Tr + Ti > 4.0) {
                escaped = 1;
            }
        }

        byte <<= 1;
        if (!escaped) {
            byte |= 1;
        }
    }

    return byte;
}

// Full image computation
static void mandelbrot_compute(int size, unsigned char *output) {
    int w = (size + 7) & ~7;  // Round up to multiple of 8
    int h = w;

    long long byte_index = 0;

    for (int y = 0; y < h; y++) {
        double Ci = 2.0 * y / h - 1.0;

        for (int x = 0; x < w; x += 8) {
            double Cr_base = 2.0 * x / w - 1.5;
            output[byte_index++] = mandelbrot_calc_byte(Cr_base, Ci, w);
        }
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runMandelbrotBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    const int iterations = 1;

    // Width aligned to multiple of 8
    int size = (n + 7) & ~7;

    // Compute required bytes
    long long bytes_needed = ((long long)size * (long long)size) / 8;

    unsigned char *output = (unsigned char *)malloc(bytes_needed);
    if (!output) {
        __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                            "Mandelbrot C: memory allocation failed");
        return (*env)->NewStringUTF(env, "Memory allocation failed");
    }

    clock_t start = clock();

    for (int iter = 0; iter < iterations; iter++) {
        mandelbrot_compute(size, output);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    free(output);

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "Mandelbrot C completed: %ldms (%d iterations, size=%d)",
                        duration, iterations, size);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Mandelbrot C completed: %ldms (%d iterations, size=%d)",
             duration, iterations, size);

    return (*env)->NewStringUTF(env, buffer);
}