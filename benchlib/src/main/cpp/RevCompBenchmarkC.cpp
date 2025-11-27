#include <jni.h>
#include <string>
#include <cstring>
#include <time.h>
#include <android/log.h>

// Pre-computed lookup tables (like TCLBG)
static unsigned char byte_lookup[256];
static unsigned short word_lookup[65536];

// Initialize lookup tables once
static void init_lookup_tables() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    // Initialize byte lookup with identity
    for (int i = 0; i < 256; i++) {
        byte_lookup[i] = i;
    }

    // Set up complement mappings (matches TCLBG's "acbdghkmnsrutwvy" -> "TGVHCDMKNSYAAWBR")
    const char *from = "acbdghkmnsrutwvyACBDGHKMNSRUTWVY";
    const char *to   = "TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR";

    for (int i = 0; from[i] && to[i]; i++) {
        byte_lookup[(unsigned char)from[i]] = to[i];
    }

    // Build word lookup table (process 2 bytes at once)
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            word_lookup[(i << 8) | j] = (byte_lookup[j] << 8) | byte_lookup[i];
        }
    }
}

static void reverse_complement(char* data, size_t size) {
    // Process 2 bytes at a time using shorts (like TCLBG)
    short *bot = (short *)data;
    short *top = (short *)(data + size - 2);

    while (bot < top) {
        short tmp = word_lookup[*(unsigned short*)bot];
        *(unsigned short*)bot = word_lookup[*(unsigned short*)top];
        *(unsigned short*)top = tmp;
        bot++;
        top--;
    }

    // Handle odd byte in the middle if size is odd
    if (size & 1) {
        size_t mid = size / 2;
        data[mid] = byte_lookup[(unsigned char)data[mid]];
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jstring fastaInput) {

    init_lookup_tables();

    clock_t start = clock();
    int iterations = 1;  // FIXED: Match TCLBG (runs once)

    const char *input = env->GetStringUTFChars(fastaInput, nullptr);
    size_t len = strlen(input);

    // Allocate working buffer
    char *buffer = new char[len + 1];

    for (int iter = 0; iter < iterations; iter++) {
        // Copy input to working buffer
        memcpy(buffer, input, len);
        buffer[len] = '\0';

        reverse_complement(buffer, len);
    }

    delete[] buffer;
    env->ReleaseStringUTFChars(fastaInput, input);

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "RevComp C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char result[256];
    snprintf(result, sizeof(result),
             "RevComp C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(result);
}