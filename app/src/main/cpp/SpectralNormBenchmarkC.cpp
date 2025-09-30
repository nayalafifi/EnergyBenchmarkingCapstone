#include "SpectralNormBenchmarkC.h"
#include <vector>
#include <cmath>
#include <thread>
#include <android/log.h>
#include <time.h>

static inline double eval_A(int i, int j) {
    return 1.0 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

struct thread_data {
    int start;
    int end;
    int n;
    const std::vector<double>* v;
    std::vector<double>* out;
    bool transpose;
};

static void compute_section(thread_data* data) {
    for (int i = data->start; i < data->end; i++) {
        double sum = 0.0;
        for (int j = 0; j < data->n; j++) {
            sum += (*data->v)[j] * (data->transpose ? eval_A(j, i) : eval_A(i, j));
        }
        (*data->out)[i] = sum;
    }
}

static void mult_Av_threaded(const std::vector<double>& v, std::vector<double>& out, int num_threads) {
    int n = v.size();
    std::vector<std::thread> threads;
    std::vector<thread_data> thread_datas(num_threads);

    int chunk = n / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_datas[i].start = i * chunk;
        thread_datas[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk;
        thread_datas[i].n = n;
        thread_datas[i].v = &v;
        thread_datas[i].out = &out;
        thread_datas[i].transpose = false;
        threads.emplace_back(compute_section, &thread_datas[i]);
    }

    for (auto& t : threads) t.join();
}

static void mult_Atv_threaded(const std::vector<double>& v, std::vector<double>& out, int num_threads) {
    int n = v.size();
    std::vector<std::thread> threads;
    std::vector<thread_data> thread_datas(num_threads);

    int chunk = n / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_datas[i].start = i * chunk;
        thread_datas[i].end = (i == num_threads - 1) ? n : (i + 1) * chunk;
        thread_datas[i].n = n;
        thread_datas[i].v = &v;
        thread_datas[i].out = &out;
        thread_datas[i].transpose = true;
        threads.emplace_back(compute_section, &thread_datas[i]);
    }

    for (auto& t : threads) t.join();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runSpectralNormBenchmarkCpp(
        JNIEnv* env, jclass, jint n) {

    clock_t start = clock();

    int N = n > 0 ? n : 100;
    int num_threads = 2 * std::thread::hardware_concurrency();

    // Run 5000 iterations to match Java
    for (int iteration = 0; iteration < 5000; iteration++) {
        std::vector<double> u(N, 1.0);
        std::vector<double> v(N);
        std::vector<double> tmp(N);

        for (int i = 0; i < 10; i++) {
            mult_Av_threaded(u, tmp, num_threads);
            mult_Atv_threaded(tmp, v, num_threads);
            mult_Av_threaded(v, tmp, num_threads);
            mult_Atv_threaded(tmp, u, num_threads);
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "SpectralNorm C++ duration: %ldms", duration);

    std::string result = "SpectralNorm C++ completed: " + std::to_string(duration) + "ms";
    return env->NewStringUTF(result.c_str());
}