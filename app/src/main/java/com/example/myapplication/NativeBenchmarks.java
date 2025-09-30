package com.example.myapplication;

public class NativeBenchmarks {
    static {
        System.loadLibrary("nativebenchmarks");
    }

    // BinaryTrees C and C++ versions
    public static native String runBinaryTreesBenchmarkC(int minDepth, int maxDepth);
    public static native String runBinaryTreesBenchmarkCpp(int minDepth, int maxDepth);

    // FannkuchRedux C and C++ versions
    public static native String runFannkuchReduxBenchmarkC(int n);
    public static native String runFannkuchReduxBenchmarkCpp(int n);

    // Fasta C and C++ versions
    public static native String runFastaBenchmarkC(int n);
    public static native String runFastaBenchmarkCpp(int n);

    // Mandelbrot C and C++ versions
    public static native String runMandelbrotBenchmarkC(int n);
    public static native String runMandelbrotBenchmarkCpp(int n);

    // NBody C and C++ versions
    public static native String runNBodyBenchmarkC(int n);
    public static native String runNBodyBenchmarkCpp(int n);

    // RevComp C and C++ versions
    public static native String runRevCompBenchmarkC(String fastaInput);
    public static native String runRevCompBenchmarkCpp(String fastaInput);

    // SpectralNorm C and C++ versions
    public static native String runSpectralNormBenchmarkC(int n);
    public static native String runSpectralNormBenchmarkCpp(int n);
}
