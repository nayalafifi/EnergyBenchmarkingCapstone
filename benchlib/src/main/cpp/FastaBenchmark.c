#include <jni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

#define IM 139968
#define IA 3877
#define IC 29573
#define SEED 42

#define LINELEN 60

static uint32_t seed = SEED;
#define uint32_rand() (seed = (seed * IA + IC) % IM)

static const char *alu =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG"
        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA"
        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT"
        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA"
        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG"
        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC"
        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

static const char *iub = "acgtBDHKMNRSVWY";
static const float iub_p[] = {
        0.27f, 0.12f, 0.12f, 0.27f, 0.02f, 0.02f, 0.02f, 0.02f,
        0.02f, 0.02f, 0.02f, 0.02f, 0.02f, 0.02f, 0.02f
};

static const char *homosapiens = "acgt";
static const float homosapiens_p[] = {
        0.3029549426680f,
        0.1979883004921f,
        0.1975473066391f,
        0.3015094502008f
};

// Generate repeated sequence
static void repeat_fasta(const char *seq, const int n) {
    const int len = strlen(seq);
    int pos = 0;

    for (int remaining = n; remaining > 0;) {
        int line_len = remaining < LINELEN ? remaining : LINELEN;

        for (int i = 0; i < line_len; i++) {
            // Would output seq[pos] here
            volatile char dummy = seq[pos];
            (void)dummy;
            pos = (pos + 1) % len;
        }

        remaining -= line_len;
    }
}

// Generate random sequence using TCLBG algorithm
static void random_fasta(const char *symb, const float *probability, const int n) {
    const int len = strlen(symb);

    // Build cumulative probability table (like TCLBG)
    uint32_t cumul_p[len];
    float cumul_acc = 0.0f;

    for (int i = 0; i < len; i++) {
        cumul_acc += probability[i];
        cumul_p[i] = 1 + (uint32_t)(cumul_acc * (float)IM);
    }

    int col = 0;
    for (int remaining = n; remaining > 0; remaining--) {
        uint32_t r = uint32_rand();

        // Find the nucleotide using cumulative probabilities
        int ncnt = 0;
        for (int nid = 0; nid < len; nid++) {
            if (cumul_p[nid] <= r) {
                ncnt++;
            }
        }

        char c = symb[ncnt];

        // Would output character here
        volatile char dummy = c;
        (void)dummy;

        if (++col >= LINELEN) {
            col = 0;
        }
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 1;

    for (int iter = 0; iter < iterations; iter++) {
        // Reset seed for each iteration
        seed = SEED;

        // Generate sequences as per CLBG
        repeat_fasta(alu, n * 2);
        random_fasta(iub, iub_p, n * 3);
        random_fasta(homosapiens, homosapiens_p, n * 5);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "Fasta C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Fasta C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}