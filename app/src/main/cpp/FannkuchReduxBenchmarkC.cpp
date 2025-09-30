#include "FannkuchReduxBenchmarkC.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <jni.h>
#include <time.h>
#include <android/log.h>

static int64_t fact[32];

// Initialize factorial lookup
void initializeFact(int n) {
    fact[0] = 1;
    for (int i = 1; i <= n; ++i)
        fact[i] = i * fact[i - 1];
}

class Permutation {
public:
    Permutation(int n, int64_t start);
    void advance();
    int64_t countFlips() const;
private:
    std::vector<int> count;
    std::vector<int8_t> current;
};

Permutation::Permutation(int n, int64_t start) {
    count.resize(n);
    current.resize(n);
    // Initialize count
    for (int i = n - 1; i >= 0; --i) {
        auto d = start / fact[i];
        start = start % fact[i];
        count[i] = d;
    }
    // Initialize current
    for (int i = 0; i < n; ++i)
        current[i] = i;
    for (int i = n - 1; i >= 0; --i) {
        auto d = count[i];
        auto b = current.begin();
        std::rotate(b, b + d, b + i + 1);
    }
}

void Permutation::advance() {
    for (int i = 1; ; ++i) {
        auto first = current[0];
        for (int j = 0; j < i; ++j)
            current[j] = current[j + 1];
        current[i] = first;
        ++(count[i]);
        if (count[i] <= i)
            break;
        count[i] = 0;
    }
}

inline int64_t Permutation::countFlips() const {
    const int n = (int)current.size();
    int flips = 0;
    int first = current[0];
    if (first > 0) {
        flips = 1;
        int8_t temp[32];
        for (int i = 0; i < n; ++i)
            temp[i] = current[i];
        while (temp[first] > 0) {
            int8_t newFirst = temp[first];
            temp[first] = first;
            if (first > 2) {
                int64_t low = 1, high = first - 1;
                while (true) {
                    std::swap(temp[low], temp[high]);
                    if (!(low + 3 <= high && low < 16))
                        break;
                    ++low; --high;
                }
            }
            first = newFirst;
            ++flips;
        }
    }
    return flips;
}

// JNI wrapper
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runFannkuchReduxBenchmarkCpp(JNIEnv* env, jclass, jint n) {
    clock_t start = clock();

    initializeFact(n);

    int64_t checksum = 0, maxFlips = 0;

    // Run multiple iterations to stretch runtime
    const int iterations = 80;
    for (int iter = 0; iter < iterations; ++iter) {
        int blockCount = 1;  // keep single-threaded
        if (blockCount > fact[n])
            blockCount = 1;
        const int64_t blockLength = fact[n] / blockCount;

        for (int64_t blockStart = 0; blockStart < fact[n]; blockStart += blockLength) {
            Permutation permutation(n, blockStart);
            int64_t index = blockStart;
            while (1) {
                const auto flips = permutation.countFlips();
                if (flips) {
                    if (index % 2 == 0)
                        checksum += flips;
                    else
                        checksum -= flips;
                    if (flips > maxFlips)
                        maxFlips = flips;
                }
                if (++index == blockStart + blockLength)
                    break;
                permutation.advance();
            }
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "FannkuchRedux C++ finished: checksum=%lld, Pfannkuchen(%d)=%lld, duration=%ld ms",
                        (long long)checksum, n, (long long)maxFlips, duration);

    std::ostringstream output;
    output << "FannkuchRedux C++ completed in " << duration << " ms ("
           << iterations << " iterations)";

    return env->NewStringUTF(output.str().c_str());
}
