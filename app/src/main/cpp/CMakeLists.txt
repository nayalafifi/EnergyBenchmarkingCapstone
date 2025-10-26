cmake_minimum_required(VERSION 3.22.1)
project("myapplication" LANGUAGES C CXX)

# Group C sources
set(NATIVE_C_SOURCES
        BinaryTreesBenchmark.c
        FannkuchReduxBenchmark.c
        FastaBenchmark.c
        MandelbrotBenchmark.c
        NbodyBenchmark.c
        RevCompBenchmark.c
        SpectralNormBenchmark.c
)

# Group C++ sources
set(NATIVE_CPP_SOURCES
        BinaryTreesBenchmarkC.cpp
        FannkuchReduxBenchmarkC.cpp
        FastaBenchmarkC.cpp
        MandelbrotBenchmarkC.cpp
        NBodyBenchmarkC.cpp
        RevCompBenchmarkC.cpp
        SpectralNormBenchmarkC.cpp
)

add_library(nativebenchmarks SHARED
        ${NATIVE_C_SOURCES}
        ${NATIVE_CPP_SOURCES}
)

# Link required Android system libraries
target_link_libraries(nativebenchmarks
        android
        log
)
