#include <jni.h>
#include <vector>
#include <time.h>
#include <android/log.h>

static void mandelbrot(int w, int h, std::vector<unsigned char>& output) {
    int bit_num = 0;
    unsigned char byte_acc = 0;
    const int iter = 50;
    const double limit = 4.0;
    double Zr, Zi, Cr, Ci, Tr, Ti;

    int index = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Zr = Zi = 0.0;
            Cr = (2.0 * x / w - 1.5);
            Ci = (2.0 * y / h - 1.0);

            bool escaped = false;
            for (int batch = 0; batch < 10; batch++) {
                for (int k = 0; k < 5; k++) {
                    Tr = Zr * Zr;
                    Ti = Zi * Zi;
                    Zi = 2.0 * Zr * Zi + Ci;
                    Zr = Tr - Ti + Cr;
                }
                // Check escape condition after every 5 iterations
                Tr = Zr * Zr;
                Ti = Zi * Zi;
                if (Tr + Ti > limit) {
                    escaped = true;
                    break;
                }
            }

            byte_acc <<= 1;
            if (!escaped) byte_acc |= 0x01;

            bit_num++;

            if (bit_num == 8) {
                output[index++] = byte_acc;
                byte_acc = 0;
                bit_num = 0;
            } else if (x == w - 1) {
                byte_acc <<= (8 - w % 8);
                output[index++] = byte_acc;
                byte_acc = 0;
                bit_num = 0;
            }
        }
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runMandelbrotBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 1;

    int size = (n * n / 8) + 1;
    std::vector<unsigned char> output(size);

    for (int iter = 0; iter < iterations; iter++) {
        mandelbrot(n, n, output);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "Mandelbrot C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Mandelbrot C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}