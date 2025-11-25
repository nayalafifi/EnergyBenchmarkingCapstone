#include <jni.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <android/log.h>

#define IM 139968
#define IA   3877
#define IC  29573
#define SEED   42

static uint32_t seed = SEED;
#define uint32_rand() ( seed = (seed * IA + IC ) % IM )

#ifndef BUFLINES
#define BUFLINES 100
#endif

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

#define LINELEN 60

static void repeat_fasta(const char *seq, const int n) {
    const int len = strlen(seq);
    int buflen1 = len + LINELEN;
    char *buffer1 = new char[buflen1];
    int i;

    if (LINELEN < len) {
        memcpy(buffer1, seq, len);
        memcpy(buffer1+len, seq, LINELEN);
    } else {
        for (i=0; i < LINELEN/len; i++) memcpy(buffer1+i*len, seq, len);
        memcpy(buffer1+i*len, seq, n - i*n);
    }

    int buflen2 = (LINELEN+1) * len;
    char *buffer2 = new char[buflen2];
    for (i=0; i<len; i++) {
        memcpy(buffer2+i*(LINELEN+1), buffer1+((i*LINELEN)%len), LINELEN);
        buffer2[(i+1)*(LINELEN+1)-1] = '\n';
    }

    delete[] buffer1;
    delete[] buffer2;
}

static char * build_hash(const char *symb, const float *probability) {
    int i, j;
    char *hash = new char[IM];

    float sum = 0.0f;
    const int len = strlen(symb);
    sum = probability[0];

    for (i=0, j=0; i<IM && j<len; i++) {
        float r = 1.0f * i / IM;
        if (r >= sum) {
            j++;
            sum += probability[j];
        }
        hash[i] = symb[j];
    }
    return hash;
}

static char * buffer_with_linebreaks(const int lines) {
    char *buffer = new char[(LINELEN+1)*lines];
    for (int i=0; i<lines; i++) {
        buffer[i*(LINELEN+1)+LINELEN] = '\n';
    }
    return buffer;
}

static void random_fasta(const char *symb, const float *probability, const int n) {
    int i, j, k;
    char *hash = build_hash(symb, probability);
    char *buffer = buffer_with_linebreaks(BUFLINES);

    int buffers = n/LINELEN/BUFLINES;
    for (i=0; i<buffers; i++) {
        for (j=0; j<BUFLINES; j++) {
            for (k=0; k<LINELEN; k++) {
                uint32_t v = uint32_rand();
                buffer[j*(LINELEN+1)+k] = hash[v];
            }
        }
    }

    int lines = n/LINELEN - buffers*BUFLINES;
    for (j=0; j<lines; j++) {
        for (k=0; k<LINELEN; k++) {
            uint32_t v = uint32_rand();
            buffer[j*(LINELEN+1)+k] = hash[v];
        }
    }

    int partials = n - LINELEN*lines - buffers*BUFLINES*LINELEN;
    for (k=0; k<partials; k++) {
        uint32_t v = uint32_rand();
        buffer[lines*(LINELEN+1)+k] = hash[v];
    }

    delete[] buffer;
    delete[] hash;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 14200;

    for (int it = 0; it < iterations; it++) {
        seed = SEED;
        repeat_fasta(alu, n*2);
        random_fasta(iub, iub_p, n*3);
        random_fasta(homosapiens, homosapiens_p, n*5);
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