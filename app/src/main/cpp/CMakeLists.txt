cmake_minimum_required(VERSION 3.22.1)

project("myapplication")

# Define nativebenchmarks shared library with your native source file(s)
add_library(nativebenchmarks SHARED
        BinaryTreesBenchmarkC.cpp
        FannkuchReduxBenchmarkC.cpp
        FastaBenchmarkC.cpp
        MandelbrotBenchmarkC.cpp
        NBodyBenchmarkC.cpp
        RevCompBenchmarkC.cpp
        SpectralNormBenchmarkC.cpp
        BinaryTreesBenchmark.c
        FannkuchReduxBenchmark.c
        FastaBenchmark.c
        MandelbrotBenchmark.c
        NbodyBenchmark.c
        RevCompBenchmark.c
        SpectralNormBenchmark.c


        # Add other C++ source files here (do NOT add header files)
)

# Link libraries to nativebenchmarks library
target_link_libraries(nativebenchmarks
        android
        log
)

