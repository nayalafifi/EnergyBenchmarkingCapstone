#include <jni.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <android/log.h>

typedef intptr_t intnative_t;
#define PREFERRED_NUMBER_OF_BLOCKS_TO_USE 12

// Original fannkuchredux_run implementation
intnative_t fannkuchredux_run(intnative_t n) {
    intnative_t factorial_Lookup_Table[n + 1];
    factorial_Lookup_Table[0] = 1;
    for (intnative_t i = 1; i <= n; ++i) {
        factorial_Lookup_Table[i] = i * factorial_Lookup_Table[i - 1];
    }

    const intnative_t total_Permutations = factorial_Lookup_Table[n];
    const intnative_t block_Size =
            factorial_Lookup_Table[n] < PREFERRED_NUMBER_OF_BLOCKS_TO_USE ?
            total_Permutations : total_Permutations / PREFERRED_NUMBER_OF_BLOCKS_TO_USE;

    intnative_t checksum = 0;
    intnative_t maximum_Flip_Count = 0;

#pragma omp parallel for reduction(max:maximum_Flip_Count) reduction(+:checksum)
    for (intnative_t block_Start_Index = 0; block_Start_Index < total_Permutations; block_Start_Index += block_Size) {
        intnative_t count[n];
        int8_t temp_Permutation[n], current_Permutation[n];

        count[0] = 0;
        for (intnative_t i = 0; i < n; ++i) current_Permutation[i] = i;

        for (intnative_t i = n - 1, permutation_Index = block_Start_Index; i > 0; --i) {
            const intnative_t d = permutation_Index / factorial_Lookup_Table[i];
            permutation_Index %= factorial_Lookup_Table[i];
            count[i] = d;
            for (intnative_t j = 0; j < n; ++j) temp_Permutation[j] = current_Permutation[j];
            for (intnative_t j = 0; j <= i; ++j)
                current_Permutation[j] = j + d <= i ? temp_Permutation[j + d] : temp_Permutation[j + d - i - 1];
        }

        const intnative_t last_Permutation_Index_In_Block = block_Start_Index + block_Size - 1;
        for (intnative_t permutation_Index = block_Start_Index;; ++permutation_Index) {
            if (current_Permutation[0] > 0) {
                for (intnative_t i = 0; i < n; ++i) temp_Permutation[i] = current_Permutation[i];

                intnative_t flip_Count = 1;
                for (intnative_t first_Value = current_Permutation[0]; temp_Permutation[first_Value] > 0; ++flip_Count) {
                    const int8_t new_First_Value = temp_Permutation[first_Value];
                    temp_Permutation[first_Value] = first_Value;

                    if (first_Value > 2) {
                        intnative_t low_Index = 1, high_Index = first_Value - 1;
                        do {
                            const int8_t temp = temp_Permutation[high_Index];
                            temp_Permutation[high_Index] = temp_Permutation[low_Index];
                            temp_Permutation[low_Index] = temp;
                        } while (low_Index++ + 3 <= high_Index-- && low_Index < 16);
                    }

                    first_Value = new_First_Value;
                }

                if (permutation_Index % 2 == 0)
                    checksum += flip_Count;
                else
                    checksum -= flip_Count;

                if (flip_Count > maximum_Flip_Count)
                    maximum_Flip_Count = flip_Count;
            }

            if (permutation_Index >= last_Permutation_Index_In_Block)
                break;

            int8_t first_Value = current_Permutation[1];
            current_Permutation[1] = current_Permutation[0];
            current_Permutation[0] = first_Value;
            for (intnative_t i = 1; ++count[i] > i;) {
                count[i++] = 0;
                const int8_t new_First_Value = current_Permutation[0] = current_Permutation[1];
                for (intnative_t j = 0; ++j < i;)
                    current_Permutation[j] = current_Permutation[j + 1];
                current_Permutation[i] = first_Value;
                first_Value = new_First_Value;
            }
        }
    }

    return (intnative_t)(((intmax_t)checksum << 32) | maximum_Flip_Count);
}

JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runFannkuchReduxBenchmarkC(
        JNIEnv *env, jclass clazz, jint n
) {
    clock_t start = clock();

    intmax_t checksum = 0;
    intmax_t max_flip = 0;

    // Run multiple iterations to increase runtime (e.g., 20)
    for (int iteration = 0; iteration < 80; iteration++) {
        intnative_t result = fannkuchredux_run((intnative_t) n);
        checksum = (intmax_t) (result >> 32);
        max_flip = (intmax_t) (result & 0xFFFFFFFF);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "FannkuchRedux C finished: checksum=%jd, Pfannkuchen(%d)=%jd, duration=%ld ms",
                        checksum, n, max_flip, duration);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "FannkuchRedux C completed in %ld ms (20 iterations)", duration);

    return (*env)->NewStringUTF(env, buffer);
}
