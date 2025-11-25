#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>

// Mandelbrot calculation for 8 pixels at once (byte)
static unsigned char mandelbrot_calc_byte(double Cr_base, double Ci, int w) {
    unsigned char byte = 0;
    int bit;

    for(bit = 0; bit < 8; bit++) {
        double Cr = Cr_base + (2.0 * bit / w);
        double Zr = 0.0, Zi = 0.0;
        double Tr = 0.0, Ti = 0.0;

        int iter;
        // CLBG uses 50 iterations max (6*8 + 2 in their optimized version)
        for(iter = 0; iter < 50 && (Tr + Ti <= 4.0); iter++) {
            Zi = 2.0 * Zr * Zi + Ci;
            Zr = Tr - Ti + Cr;
            Tr = Zr * Zr;
            Ti = Zi * Zi;
        }

        byte = (byte << 1);
        if(Tr + Ti <= 4.0) {
            byte |= 0x01;
        }
    }

    return byte;
}

static void mandelbrot(int size, unsigned char *output) {
    // Ensure width is multiple of 8 for byte alignment
    int w = (size + 7) & ~7;
    int h = w;

    int y, x;
    int byte_index = 0;

    for(y = 0; y < h; y++) {
        double Ci = 2.0 * y / h - 1.0;

        for(x = 0; x < w; x += 8) {
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

    clock_t start = clock();
    int iterations = 40;

    // Align to 8 pixels as per CLBG
    int size = (n + 7) & ~7;
    int bytes_needed = (size * size) / 8;
    unsigned char *output = (unsigned char *)malloc(bytes_needed);

    if (!output) {
        return (*env)->NewStringUTF(env, "Memory allocation failed");
    }

    for (int iter = 0; iter < iterations; iter++) {
        mandelbrot(size, output);
    }

    free(output);

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "Mandelbrot C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Mandelbrot C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}