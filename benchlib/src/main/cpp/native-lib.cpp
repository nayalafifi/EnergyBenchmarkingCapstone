#include <jni.h>
#include <string>
//
//// ===== C Benchmarks =====
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runBinaryTreesBenchmarkC(
//        JNIEnv *env, jclass, jint minDepth, jint maxDepth) {
//    std::string result = "Running BinaryTrees C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runFannkuchReduxBenchmarkC(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running FannkuchRedux C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkC(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running Fasta C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runMandelbrotBenchmarkC(
//        JNIEnv *env, jclass, jint size) {
//    std::string result = "Running Mandelbrot C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runNBodyBenchmarkC(
//        JNIEnv *env, jclass, jint iterations) {
//    std::string result = "Running NBody C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkC(
//        JNIEnv *env, jclass, jstring fasta) {
//    std::string result = "Running RevComp C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runSpectralNormBenchmarkC(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running SpectralNorm C benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//// ===== C++ Benchmarks =====
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runBinaryTreesBenchmarkCpp(
//        JNIEnv *env, jclass, jint minDepth, jint maxDepth) {
//    std::string result = "Running BinaryTrees C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runFannkuchReduxBenchmarkCpp(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running FannkuchRedux C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runFastaBenchmarkCpp(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running Fasta C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runMandelbrotBenchmarkCpp(
//        JNIEnv *env, jclass, jint size) {
//    std::string result = "Running Mandelbrot C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runNBodyBenchmarkCpp(
//        JNIEnv *env, jclass, jint iterations) {
//    std::string result = "Running NBody C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runRevCompBenchmarkCpp(
//        JNIEnv *env, jclass, jstring fasta) {
//    std::string result = "Running RevComp C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_benchlib_NativeBenchmarks_runSpectralNormBenchmarkCpp(
//        JNIEnv *env, jclass, jint n) {
//    std::string result = "Running SpectralNorm C++ benchmark...";
//    return env->NewStringUTF(result.c_str());
//}
