#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

// Complement lookup table - matches TCLBG reference exactly
#define COMPLEMENT_LOOKUP \
  "                                                                "\
  /*ABCDEFGHIJKLMNOPQRSTUVWXYZ      abcdefghijklmnopqrstuvwxyz    */\
  " TVGH  CD  M KN   YSAABW R       TVGH  CD  M KN   YSAABW R"

typedef intptr_t intnative_t;

static void process_Sequence(char *sequence, const intnative_t sequence_Size) {
    // Set up pointers to front and back
    char *front_Pos = sequence, *back_Pos = sequence + sequence_Size - 1;

    // Advance front_Pos to first character on next line (after header)
    while (*front_Pos++ != '\n');

    // Skip leading/trailing newlines
    while (*front_Pos == '\n' && front_Pos <= back_Pos) front_Pos++;
    while (*back_Pos == '\n' && front_Pos <= back_Pos) back_Pos--;

    // Reverse and complement the sequence
    while (front_Pos <= back_Pos) {
        const char temp = COMPLEMENT_LOOKUP[(unsigned char)*front_Pos];
        *front_Pos = COMPLEMENT_LOOKUP[(unsigned char)*back_Pos];
        *back_Pos = temp;

        // Skip over line feeds
        while (*++front_Pos == '\n');
        while (*--back_Pos == '\n');
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jstring fastaInput) {

    clock_t start = clock();
    int iterations = 1;  // Match Java iterations

    const char *input = (*env)->GetStringUTFChars(env, fastaInput, NULL);
    int len = strlen(input);

    // Create test FASTA format data
    char *test_data;
    int test_len;

    if (input[0] != '>') {
        // Create FASTA format from input
        test_len = len + 100;  // Extra space for header
        test_data = (char *)malloc(test_len);
        snprintf(test_data, test_len, ">TEST\n%s\n", input);
        test_len = strlen(test_data);
    } else {
        // Use input as-is
        test_len = len;
        test_data = (char *)malloc(test_len + 1);
        strcpy(test_data, input);
    }

    // Allocate working buffer
    char *work_buffer = (char *)malloc(test_len + 1);

    // Benchmark the FASTA processing
    for (int iter = 0; iter < iterations; iter++) {
        // Reset data for each iteration
        memcpy(work_buffer, test_data, test_len + 1);

        // Process the sequence
        process_Sequence(work_buffer, test_len);
    }

    free(work_buffer);
    free(test_data);
    (*env)->ReleaseStringUTFChars(env, fastaInput, input);

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "RevComp C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "RevComp C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}