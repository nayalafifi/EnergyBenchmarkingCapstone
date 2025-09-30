#include "FastaBenchmarkC.h"
#include <string>
#include <cstdlib>
#include <cstring>
#include <android/log.h>
#include <time.h>

static const char ALU[] =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

static const char IUB_CODES[] = {'a', 'c', 'g', 't', 'B', 'D', 'H', 'K', 'M', 'N', 'R', 'S', 'V', 'W', 'Y'};
static const double IUB_PROBS[] = {0.27, 0.12, 0.12, 0.27, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02};

static const char HOMO_SAPIENS_CODES[] = {'a', 'c', 'g', 't'};
static const double HOMO_SAPIENS_PROBS[] = {0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008};

static char select_random(const char* codes, const double* cumProbs, int size) {
    double r = (double)rand() / RAND_MAX;
    for (int i = 0; i < size; i++) {
        if (r < cumProbs[i]) return codes[i];
    }
    return codes[size - 1];
}

static void generate_repeat(const char* seed, int n) {
    int length = strlen(seed);
    int pos = 0;
    int remaining = n;

    while (remaining > 0) {
        int lineLen = (60 < remaining) ? 60 : remaining;
        if (pos + lineLen < length) {
            pos += lineLen;
        } else {
            pos = (pos + lineLen) % length;
        }
        remaining -= lineLen;
    }
}

static void generate_random(int n, const char* codes, const double* probs, int code_size) {
    double* cumProbs = new double[code_size];
    cumProbs[0] = probs[0];
    for (int i = 1; i < code_size; i++) {
        cumProbs[i] = cumProbs[i - 1] + probs[i];
    }

    int count = 0;
    for (int i = 0; i < n; i++) {
        select_random(codes, cumProbs, code_size);
        count++;
        if (count == 60) {
            count = 0;
        }
    }

    delete[] cumProbs;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runFastaBenchmarkCpp(
        JNIEnv* env, jclass, jint n) {

    clock_t start = clock();

    srand(42);

    // Run 3300 iterations to match Java
    for (int iteration = 0; iteration < 3300; iteration++) {
        generate_repeat(ALU, n * 2);
        generate_random(n * 3, IUB_CODES, IUB_PROBS, 15);
        generate_random(n * 5, HOMO_SAPIENS_CODES, HOMO_SAPIENS_PROBS, 4);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "Fasta C++ duration: %ldms", duration);

    std::string result = "Fasta C++ completed: " + std::to_string(duration) + "ms";
    return env->NewStringUTF(result.c_str());
}