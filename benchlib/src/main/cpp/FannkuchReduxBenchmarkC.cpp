#include <jni.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <android/log.h>

static int fannkuch(int n) {
    std::vector<int> perm(n);
    std::vector<int> perm1(n);
    std::vector<int> count(n);

    int maxFlipsCount = 0;
    int permCount = 0;
    int checksum = 0;

    for (int i = 0; i < n; i++) perm1[i] = i;

    int r = n;
    while (true) {
        while (r != 1) {
            count[r - 1] = r;
            r--;
        }

        for (int i = 0; i < n; i++) perm[i] = perm1[i];

        int flipsCount = 0;
        int k;
        while (!((k = perm[0]) == 0)) {
            int k2 = (k + 1) >> 1;
            for (int i = 0; i < k2; i++) {
                std::swap(perm[i], perm[k - i]);
            }
            flipsCount++;
        }

        maxFlipsCount = std::max(maxFlipsCount, flipsCount);
        checksum += permCount % 2 == 0 ? flipsCount : -flipsCount;

        while (true) {
            if (r == n) return maxFlipsCount;

            int perm0 = perm1[0];
            int i = 0;
            while (i < r) {
                int j = i + 1;
                perm1[i] = perm1[j];
                i = j;
            }
            perm1[r] = perm0;

            count[r]--;
            if (count[r] > 0) break;
            r++;
        }
        permCount++;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFannkuchReduxBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 80;

    for (int iter = 0; iter < iterations; iter++) {
        fannkuch(n);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "FannkuchRedux C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "FannkuchRedux C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}