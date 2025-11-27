#include <jni.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <time.h>
#include <android/log.h>

#define IM 139968
#define IA 3877
#define IC 29573
#define SEED 42

const int LINELEN = 60;

static const char *alu =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

struct IUB {
    float p;
    char c;
};

static IUB iub[] = {
        {0.27f, 'a'}, {0.12f, 'c'}, {0.12f, 'g'}, {0.27f, 't'},
        {0.02f, 'B'}, {0.02f, 'D'}, {0.02f, 'H'}, {0.02f, 'K'},
        {0.02f, 'M'}, {0.02f, 'N'}, {0.02f, 'R'}, {0.02f, 'S'},
        {0.02f, 'V'}, {0.02f, 'W'}, {0.02f, 'Y'}
};

static IUB homosapiens[] = {
        {0.3029549426680f, 'a'},
        {0.1979883004921f, 'c'},
        {0.1975473066391f, 'g'},
        {0.3015094502008f, 't'}
};

const float IM_RECIPROCAL = 1.0f / IM;

class Random {
public:
    uint32_t gen() {
        last = (last * IA + IC) % IM;
        return last;
    }
    void reset() { last = SEED; }
private:
    int last = SEED;
};

static void make_cumulative(IUB *table, int len) {
    float cp = 0.0f;
    for (int i = 0; i < len; i++) {
        cp += table[i].p;
        table[i].p = cp;
    }
}

static void repeat_fasta(const char *seq, int n) {
    const int len = strlen(seq);
    int pos = 0;
    int remaining = n;

    while (remaining > 0) {
        int line_len = std::min(LINELEN, remaining);

        for (int i = 0; i < line_len; i++) {
            // Would output seq[pos] here
            volatile char dummy = seq[pos];
            (void)dummy;
            pos = (pos + 1) % len;
        }

        remaining -= line_len;
    }
}

static void random_fasta(IUB *table, int len, Random &rng, int n) {
    int remaining = n;
    int col = 0;

    while (remaining > 0) {
        uint32_t r = rng.gen();
        float p = r * IM_RECIPROCAL;

        // Find character using cumulative probabilities
        char c = table[len - 1].c;
        for (int i = 0; i < len; i++) {
            if (p <= table[i].p) {
                c = table[i].c;
                break;
            }
        }

        // Would output character here
        volatile char dummy = c;
        (void)dummy;

        if (++col >= LINELEN) {
            col = 0;
        }

        remaining--;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 1;

    // Make cumulative probabilities once
    make_cumulative(iub, 15);
    make_cumulative(homosapiens, 4);

    for (int it = 0; it < iterations; it++) {
        Random rng;

        repeat_fasta(alu, n * 2);
        random_fasta(iub, 15, rng, n * 3);
        random_fasta(homosapiens, 4, rng, n * 5);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "Fasta C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Fasta C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}