#include <jni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

#define IM 139968
#define IA   3877
#define IC  29573
#define SEED   42

#ifndef BUFLINES
#define BUFLINES 100
#endif

#define LINELEN 60

static uint32_t seed = SEED;
#define uint32_rand() ( seed = (seed * IA + IC ) % IM )

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

// Generate repeated sequence (no actual output, just computation)
static void repeat_fasta(const char *seq, const int n) {
    const int len = strlen(seq);
    int buflen1 = len + LINELEN;
    char *buffer1 = malloc(buflen1);

    if (!buffer1) return;

    // Fill buffer with repeated sequence
    if (LINELEN < len) {
        memcpy(buffer1, seq, len);
        memcpy(buffer1 + len, seq, LINELEN);
    } else {
        int i;
        for (i = 0; i < LINELEN/len; i++) {
            memcpy(buffer1 + i*len, seq, len);
        }
        if (i * len < LINELEN) {
            memcpy(buffer1 + i*len, seq, LINELEN - i*len);
        }
    }

    // Process the data (computation only, no output)
    int total_lines = n / LINELEN;
    int remaining = n % LINELEN;

    // Simulate processing
    for (int i = 0; i < total_lines; i++) {
        // Would output LINELEN characters here
        volatile char dummy = buffer1[i % buflen1];
        (void)dummy;
    }

    if (remaining > 0) {
        // Would output remaining characters here
        volatile char dummy = buffer1[0];
        (void)dummy;
    }

    free(buffer1);
}

// Build hash table for random generation
static char * build_hash(const char *symb, const float *probability) {
    char *hash = malloc(IM);
    if (!hash) return NULL;

    float sum = 0.0f;
    const int len = strlen(symb);
    int j = 0;

    for (int i = 0; i < IM; i++) {
        float r = (float)i / IM;

        while (j < len - 1 && r >= sum + probability[j]) {
            sum += probability[j];
            j++;
        }

        hash[i] = symb[j];
    }

    return hash;
}

// Generate random sequence (computation only)
static void random_fasta(const char *symb, const float *probability, const int n) {
    char *hash = build_hash(symb, probability);
    if (!hash) return;

    // Allocate buffer for one line
    char *buffer = malloc(LINELEN + 1);
    if (!buffer) {
        free(hash);
        return;
    }

    // Generate sequences
    int total_lines = n / LINELEN;
    int remaining = n % LINELEN;

    // Full lines
    for (int i = 0; i < total_lines; i++) {
        for (int k = 0; k < LINELEN; k++) {
            uint32_t v = uint32_rand();
            buffer[k] = hash[v];
        }
        // Process the line (no actual output)
        volatile char dummy = buffer[0];
        (void)dummy;
    }

    // Partial line
    if (remaining > 0) {
        for (int k = 0; k < remaining; k++) {
            uint32_t v = uint32_rand();
            buffer[k] = hash[v];
        }
        // Process the partial line
        volatile char dummy = buffer[0];
        (void)dummy;
    }

    free(buffer);
    free(hash);
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 23000;

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