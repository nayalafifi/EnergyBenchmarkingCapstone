#include <jni.h>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <time.h>
#include <android/log.h>

using namespace std;

static int64_t fact[32];

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
    vector<int> count;
    vector<int8_t> current;
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
        rotate(b, b + d, b + i + 1);
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
    const auto n = current.size();
    auto flips = 0;
    auto first = current[0];

    if (first > 0) {
        flips = 1;

        int8_t temp[n];
        // Make a copy to work on
        for (size_t i = 0; i < n; ++i)
            temp[i] = current[i];

        // Flip until first element is 0
        for (; temp[first] > 0; ++flips) {
            const int8_t newFirst = temp[first];
            temp[first] = first;

            if (first > 2) {
                int64_t low = 1, high = first - 1;
                do {
                    swap(temp[low], temp[high]);
                    if (!(low + 3 <= high && low < 16))
                        break;
                    ++low;
                    --high;
                } while (1);
            }
            first = newFirst;
        }
    }
    return flips;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFannkuchReduxBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 60;

    for (int iter = 0; iter < iterations; iter++) {
        initializeFact(n);

        // Use single block for mobile (no parallelization)
        int64_t blockCount = 1;
        const int64_t blockLength = fact[n] / blockCount;

        int64_t maxFlips = 0, checksum = 0;

        // Iterate over each block
        for (int64_t blockStart = 0;
             blockStart < fact[n];
             blockStart += blockLength) {

            Permutation permutation(n, blockStart);

            auto index = blockStart;
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
                        "FannkuchRedux C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "FannkuchRedux C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}