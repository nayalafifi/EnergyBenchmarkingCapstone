#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

// Complement lookup table as per CLBG
#define COMPLEMENT_LOOKUP \
  "                                                                "\
  /*ABCDEFGHIJKLMNOPQRSTUVWXYZ      abcdefghijklmnopqrstuvwxyz    */\
  " TVGH  CD  M KN   YSAABW R       TVGH  CD  M KN   YSAABW R"

static void reverse_complement_sequence(char *seq, int len) {
    // Skip the header line (starts with '>')
    char *sequence_start = strchr(seq, '\n');
    if (!sequence_start) return;
    sequence_start++; // Move past the newline

    // Find the end of the sequence (before next header or end of string)
    char *sequence_end = strchr(sequence_start, '>');
    if (sequence_end) {
        sequence_end--; // Back up before the '>'
    } else {
        sequence_end = seq + len - 1;
    }

    // Skip any trailing newlines
    while (sequence_end > sequence_start && *sequence_end == '\n') {
        sequence_end--;
    }

    // Set up pointers for reversal
    char *front = sequence_start;
    char *back = sequence_end;

    // Skip leading newlines
    while (front <= back && *front == '\n') front++;

    // Reverse and complement the sequence
    while (front <= back) {
        // Skip newlines
        while (front <= back && *front == '\n') front++;
        while (front <= back && *back == '\n') back--;

        if (front <= back) {
            const char temp = COMPLEMENT_LOOKUP[(unsigned char)*front];
            *front = COMPLEMENT_LOOKUP[(unsigned char)*back];
            *back = temp;
            front++;
            back--;
        }
    }
}

// Simplified FASTA processing for benchmarking
static void process_fasta(char *data, int len) {
    char *current = data;
    char *end = data + len;

    while (current < end) {
        // Find start of sequence (header line starting with '>')
        char *header = strchr(current, '>');
        if (!header) break;

        // Find next header or end
        char *next_header = strchr(header + 1, '>');
        int seq_len;
        if (next_header) {
            seq_len = next_header - header;
        } else {
            seq_len = end - header;
        }

        // Process this sequence
        reverse_complement_sequence(header, seq_len);

        current = header + seq_len;
    }
}

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jstring fastaInput) {

    clock_t start = clock();
    int iterations = 1000;

    const char *input = (*env)->GetStringUTFChars(env, fastaInput, NULL);
    int len = strlen(input);

    // Create test FASTA format data if input doesn't look like FASTA
    char *test_data;
    int test_len;

    if (input[0] != '>') {
        // Create simple FASTA format from input
        test_len = len + 20;
        test_data = (char *)malloc(test_len);
        snprintf(test_data, test_len, ">TEST\n%s\n", input);
        test_len = strlen(test_data);
    } else {
        // Use input as-is
        test_len = len;
        test_data = (char *)malloc(test_len + 1);
        strcpy(test_data, input);
    }

    // Benchmark the FASTA processing
    for (int iter = 0; iter < iterations; iter++) {
        // Reset the data for each iteration
        if (input[0] != '>') {
            snprintf(test_data, test_len, ">TEST\n%s\n", input);
        } else {
            strcpy(test_data, input);
        }

        process_fasta(test_data, test_len);
    }

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