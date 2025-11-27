#include <jni.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <android/log.h>

typedef intptr_t intnative_t;

JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runFannkuchReduxBenchmarkC(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 1;

    for (int iter = 0; iter < iterations; iter++) {
        // Create and initialize factorial lookup table
        intnative_t factorial_Lookup_Table[n+1];
        factorial_Lookup_Table[0] = 1;
        for(intnative_t i = 0; ++i <= n;)
            factorial_Lookup_Table[i] = i * factorial_Lookup_Table[i-1];

        // Use single block for simplicity (no parallelization on mobile)
        const intnative_t block_Size = factorial_Lookup_Table[n];

        intnative_t maximum_Flip_Count = 0, checksum = 0;

        // Process all permutations
        for(intnative_t initial_Permutation_Index_For_Block = 0;
            initial_Permutation_Index_For_Block < factorial_Lookup_Table[n];
            initial_Permutation_Index_For_Block += block_Size) {

            intnative_t count[n];
            int8_t temp_Permutation[n], current_Permutation[n];

            // Initialize count and current_Permutation
            count[0] = 0;
            for(intnative_t i = 0; i < n; ++i)
                current_Permutation[i] = i;

            // Generate initial permutation for this block
            for(intnative_t i = n-1,
                        permutation_Index = initial_Permutation_Index_For_Block; i > 0; --i) {
                const intnative_t d = permutation_Index / factorial_Lookup_Table[i];
                permutation_Index = permutation_Index % factorial_Lookup_Table[i];
                count[i] = d;

                for(intnative_t j = 0; j < n; ++j)
                    temp_Permutation[j] = current_Permutation[j];
                for(intnative_t j = 0; j <= i; ++j)
                    current_Permutation[j] = j+d <= i ?
                                             temp_Permutation[j+d] : temp_Permutation[j+d-i-1];
            }

            // Iterate over each permutation in the block
            const intnative_t last_Permutation_Index_In_Block =
                    initial_Permutation_Index_For_Block + block_Size - 1;

            for(intnative_t permutation_Index = initial_Permutation_Index_For_Block; ;
                ++permutation_Index) {

                // If the first value in current_Permutation is not 0
                if(current_Permutation[0] > 0) {

                    // Make a copy to work on
                    for(intnative_t i = 0; ++i < n;)
                        temp_Permutation[i] = current_Permutation[i];

                    intnative_t flip_Count = 1;

                    // Flip until first element is 0
                    for(intnative_t first_Value = current_Permutation[0];
                        temp_Permutation[first_Value] > 0; ++flip_Count) {

                        const int8_t new_First_Value = temp_Permutation[first_Value];
                        temp_Permutation[first_Value] = first_Value;

                        // Flip middle elements if needed
                        if(first_Value > 2) {
                            intnative_t low_Index = 1, high_Index = first_Value - 1;
                            do {
                                const int8_t temp = temp_Permutation[high_Index];
                                temp_Permutation[high_Index] = temp_Permutation[low_Index];
                                temp_Permutation[low_Index] = temp;
                            } while(low_Index++ + 3 <= high_Index-- && low_Index < 16);
                        }

                        first_Value = new_First_Value;
                    }

                    // Update checksum
                    if(permutation_Index % 2 == 0)
                        checksum += flip_Count;
                    else
                        checksum -= flip_Count;

                    // Update maximum flip count
                    if(flip_Count > maximum_Flip_Count)
                        maximum_Flip_Count = flip_Count;
                }

                // Break at last permutation
                if(permutation_Index >= last_Permutation_Index_In_Block)
                    break;

                // Generate next permutation
                int8_t first_Value = current_Permutation[1];
                current_Permutation[1] = current_Permutation[0];
                current_Permutation[0] = first_Value;

                for(intnative_t i = 1; ++count[i] > i;) {
                    count[i++] = 0;
                    const int8_t new_First_Value = current_Permutation[0] =
                                                           current_Permutation[1];

                    for(intnative_t j = 0; ++j < i;)
                        current_Permutation[j] = current_Permutation[j+1];

                    current_Permutation[i] = first_Value;
                    first_Value = new_First_Value;
                }
            }
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "FannkuchRedux C completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "FannkuchRedux C completed: %ldms (%d iterations)",
             duration, iterations);
    return (*env)->NewStringUTF(env, buffer);
}