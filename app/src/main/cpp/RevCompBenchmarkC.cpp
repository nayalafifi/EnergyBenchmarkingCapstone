#include "RevCompBenchmarkC.h"
#include <string>
#include <cstring>
#include <android/log.h>
#include <time.h>

static char map[256];
static bool map_initialized = false;

static void initialize_map() {
    if (map_initialized) return;

    const char* from = "ACBDGHK\nMNSRUTWVYacbdghkmnsrutwvy";
    const char* to = "TGVHCDM\nKNSYAAWBRTGVHCDMKNSYAAWBR";

    for (int i = 0; from[i] != '\0'; i++) {
        map[(unsigned char)from[i]] = to[i];
    }
    map_initialized = true;
}

static void reverse_section(char* buf, int start, int end) {
    while (start < end) {
        if (buf[start] == '\n') {
            start++;
            continue;
        }
        if (buf[end] == '\n') {
            end--;
            continue;
        }
        char temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runRevCompBenchmarkCpp(
        JNIEnv* env, jclass, jstring jinput) {

    clock_t start = clock();

    initialize_map();

    const char* input = env->GetStringUTFChars(jinput, 0);
    int input_len = strlen(input);

    // Run 1100 iterations to match Java
    for (int iteration = 0; iteration < 1100; iteration++) {
        char* buf = new char[input_len + 1];
        memcpy(buf, input, input_len + 1);

        // Map complement
        for (int i = 0; i < input_len; i++) {
            char b = buf[i];
            if (b != '\n' && b != '>') {
                buf[i] = map[(unsigned char)b];
            }
        }

        // Reverse each sequence
        int seqStart = 0;
        for (int i = 0; i < input_len; i++) {
            if (buf[i] == '>') {
                if (i > seqStart) {
                    reverse_section(buf, seqStart, i - 1);
                }
                seqStart = i;
                while (i < input_len && buf[i] != '\n') i++;
                seqStart = i + 1;
            }
        }
        if (seqStart < input_len) {
            reverse_section(buf, seqStart, input_len - 1);
        }

        delete[] buf;
    }

    env->ReleaseStringUTFChars(jinput, input);

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "RevComp C++ duration: %ldms", duration);

    std::string result = "RevComp C++ completed: " + std::to_string(duration) + "ms";
    return env->NewStringUTF(result.c_str());
}