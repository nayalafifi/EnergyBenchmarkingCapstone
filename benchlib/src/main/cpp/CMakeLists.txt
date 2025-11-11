cmake_minimum_required(VERSION 3.22.1)
project(benchlib LANGUAGES C CXX)

set(BENCHLIB_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(NATIVE_C_SOURCES
        ${BENCHLIB_CPP_DIR}/BinaryTreesBenchmark.c
        ${BENCHLIB_CPP_DIR}/FannkuchReduxBenchmark.c
        ${BENCHLIB_CPP_DIR}/FastaBenchmark.c
        ${BENCHLIB_CPP_DIR}/MandelbrotBenchmark.c
        ${BENCHLIB_CPP_DIR}/NBodyBenchmark.c
        ${BENCHLIB_CPP_DIR}/RevCompBenchmark.c
        ${BENCHLIB_CPP_DIR}/SpectralNormBenchmark.c
)

set(NATIVE_CPP_SOURCES
        ${BENCHLIB_CPP_DIR}/BinaryTreesBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/FannkuchReduxBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/FastaBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/MandelbrotBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/NBodyBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/RevCompBenchmarkC.cpp
        ${BENCHLIB_CPP_DIR}/SpectralNormBenchmarkC.cpp
)

set(JNI_BRIDGE
        ${BENCHLIB_CPP_DIR}/native-lib.cpp
)

add_library(nativebenchmarks SHARED
        ${JNI_BRIDGE}
        ${NATIVE_C_SOURCES}
        ${NATIVE_CPP_SOURCES}
)

include_directories(${BENCHLIB_CPP_DIR})

find_library(log-lib log)
target_link_libraries(nativebenchmarks android ${log-lib})
