#include <jni.h>
#include <string>
#include <algorithm>
#include <time.h>
#include <android/log.h>

static char complement(char c) {
    switch (c) {
        case 'A': case 'a': return 'T';
        case 'T': case 't': return 'A';
        case 'G': case 'g': return 'C';
        case 'C': case 'c': return 'G';
        case 'M': case 'm': return 'K';
        case 'R': case 'r': return 'Y';
        case 'W': case 'w': return 'W';
        case 'S': case 's': return 'S';
        case 'Y': case 'y': return 'R';
        case 'K': case 'k': return 'M';
        case 'V': case 'v': return 'B';
        case 'H': case 'h': return 'D';
        case 'D': case 'd': return 'H';
        case 'B': case 'b': return 'V';
        case 'N': case 'n': return 'N';
        default: return c;
    }
}

static void reverse_complement(std::string& seq) {
    int len = seq.length();
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char temp = complement(seq[i]);
        seq[i] = complement(seq[j]);
        seq[j] = temp;
    }
    if (len % 2 == 1) {
        seq[len / 2] = complement(seq[len / 2]);
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jstring fastaInput) {

    clock_t start = clock();
    int iterations = 1000;

    const char *input = env->GetStringUTFChars(fastaInput, nullptr);
    std::string original(input);
    env->ReleaseStringUTFChars(fastaInput, input);

    for (int iter = 0; iter < iterations; iter++) {
        std::string seq = original;
        reverse_complement(seq);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "RevComp C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "RevComp C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}