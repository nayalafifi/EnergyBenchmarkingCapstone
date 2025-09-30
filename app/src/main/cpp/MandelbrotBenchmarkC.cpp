#include "MandelbrotBenchmarkC.h"
#include <vector>
#include <thread>
#include <sstream>
#include <android/log.h>
#include <ctime>

static int get_byte(int x, int y, const double* Crb, const double* Cib, int n) {
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

struct thread_data {
    int n;
    int y_start;
    int y_end;
    std::vector<std::vector<unsigned char>>* out;
    double* Crb;
    double* Cib;
};

void compute_rows(thread_data* data) {
    for (int y = data->y_start; y < data->y_end; y++) {
        for (int xb = 0; xb < (data->n + 7) / 8; xb++) {
            (*data->out)[y][xb] = (unsigned char)get_byte(xb * 8, y, data->Crb, data->Cib, data->n);
        }
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runMandelbrotBenchmarkCpp(
        JNIEnv* env, jclass, jint n) {

    clock_t start = clock();

    int num_threads = 2 * std::thread::hardware_concurrency();

    // Run 100 iterations to match Java
    for (int iteration = 0; iteration < 100; iteration++) {
        std::vector<double> Crb(n + 7);
        std::vector<double> Cib(n + 7);
        double invN = 2.0 / n;

        for (int i = 0; i < n; i++) {
            Cib[i] = i * invN - 1.0;
            Crb[i] = i * invN - 1.5;
        }

        std::vector<std::vector<unsigned char>> out(n, std::vector<unsigned char>((n + 7) / 8));

        std::vector<std::thread> threads;
        std::vector<thread_data> thread_datas(num_threads);

        int rows_per_thread = n / num_threads;
        for (int i = 0; i < num_threads; i++) {
            thread_datas[i].n = n;
            thread_datas[i].y_start = i * rows_per_thread;
            thread_datas[i].y_end = (i == num_threads - 1) ? n : (i + 1) * rows_per_thread;
            thread_datas[i].out = &out;
            thread_datas[i].Crb = Crb.data();
            thread_datas[i].Cib = Cib.data();
            threads.emplace_back(compute_rows, &thread_datas[i]);
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "Mandelbrot C++ duration: %ldms", duration);

    std::ostringstream oss;
    oss << "Mandelbrot C++ completed: " << duration << "ms";
    return env->NewStringUTF(oss.str().c_str());
}