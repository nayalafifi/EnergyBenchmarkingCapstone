package com.example.myapplication;

import android.content.Context;
import android.util.Log;

import androidx.javascriptengine.JavaScriptIsolate;
import androidx.javascriptengine.JavaScriptSandbox;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

import com.example.benchlib.*;   // Java, Kotlin, C, C++, native wrappers

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.function.Supplier;

@RunWith(AndroidJUnit4.class)
public class BenchmarkJUnitTest {

    // ==================== Utilities ====================

    private String loadAssetAsString(String filename) {
        try (InputStream is = InstrumentationRegistry.getInstrumentation()
                .getTargetContext()
                .getAssets()
                .open(filename)) {

            int size = is.available();
            byte[] buffer = new byte[size];
            //noinspection ResultOfMethodCallIgnored
            is.read(buffer);
            return new String(buffer, StandardCharsets.UTF_8);

        } catch (IOException e) {
            Log.e("ASSET", "Error loading asset: " + filename, e);
            return null;
        }
    }

    /**
     * Run a JS benchmark stored as an asset using Jetpack JavaScriptEngine.
     *
     * @param jsFile asset file name, e.g. "MandelbrotBenchmarkJs.js"
     * @param jsCall JS code that calls the global function,
     *               e.g. "runMandelbrotBenchmark(6000)"
     */
    private String runJsBenchmark(String jsFile, String jsCall) {
        String jsSource = loadAssetAsString(jsFile);
        if (jsSource == null) {
            return "ERROR: Could not load " + jsFile;
        }

        Context ctx = InstrumentationRegistry.getInstrumentation().getTargetContext();

        if (!JavaScriptSandbox.isSupported()) {
            Log.e("JS_ENGINE", "JavaScriptSandbox is not supported on this device");
            return "JS engine not supported";
        }

        JavaScriptSandbox sandbox = null;
        JavaScriptIsolate isolate = null;

        try {
            // 1) Create sandbox (out-of-process JS engine)
            Future<JavaScriptSandbox> sandboxFuture =
                    JavaScriptSandbox.createConnectedInstanceAsync(ctx);
            sandbox = sandboxFuture.get(30, TimeUnit.SECONDS);

            // 2) Create an isolate (execution context)
            isolate = sandbox.createIsolate();

            // 3) Load the benchmark source (defines global functions)
            Future<String> loadFuture = isolate.evaluateJavaScriptAsync(jsSource);
            // just wait until it finishes; result usually empty
            loadFuture.get(60, TimeUnit.SECONDS);

            // 4) Call the global benchmark function and get the returned String
            Future<String> callFuture = isolate.evaluateJavaScriptAsync(jsCall);
            String result = callFuture.get(300, TimeUnit.SECONDS);   // allow long JS runs

            return (result != null && !result.isEmpty())
                    ? result
                    : "JS benchmark returned empty result";

        } catch (Exception e) {
            Log.e("JS_ENGINE", "Error running JS benchmark", e);
            return "JS error: " + e.getClass().getSimpleName() + ": " + e.getMessage();
        } finally {
            // 5) Clean up
            try {
                if (isolate != null) isolate.close();
            } catch (Exception ignored) {}
            try {
                if (sandbox != null) sandbox.close();
            } catch (Exception ignored) {}
        }
    }

    // Generic runner for all benchmarks
    private void measure(String name, Supplier<String> task) {
        try {
            Log.d("BENCHMARK", "Starting: " + name);
            long start = System.currentTimeMillis();

            String result = task.get();
            long duration = System.currentTimeMillis() - start;

            Log.d("BENCHMARK_RESULT", name + " completed in " + duration + " ms");
            if (result != null && !result.isEmpty()) {
                Log.d("BENCHMARK", "Result: " + result);
            }
        } catch (Exception e) {
            Log.e("BENCHMARK", name + " failed: " + e.getMessage(), e);
        }
    }

    // ==================== JAVA ====================

    @Test public void runBinaryTreesJava() { measure("BinaryTreesJava", BinaryTreesBenchmark::runBenchmark); }
    @Test public void runFannkuchReduxJava() { measure("FannkuchReduxJava", FannkuchReduxBenchmark::runBenchmark); }
    @Test public void runFastaJava() { measure("FastaJava", FastaBenchmark::runBenchmark); }
    @Test public void runMandelbrotJava() { measure("MandelbrotJava", MandelbrotBenchmark::runBenchmark); }
    @Test public void runNBodyJava() { measure("NBodyJava", NBodyBenchmark::runBenchmark); }
    @Test public void runRevCompJava() { measure("RevCompJava", RevCompBenchmark::runBenchmark); }
    @Test public void runSpectralNormJava() { measure("SpectralNormJava", SpectralNormBenchmark::runBenchmark); }

    // ==================== KOTLIN ====================

    @Test public void runBinaryTreesKotlin() { measure("BinaryTreesKotlin", BinaryTreesBenchmarkKotlin.INSTANCE::runBenchmark); }
    @Test public void runFannkuchReduxKotlin() { measure("FannkuchReduxKotlin", FannkuchReduxBenchmarkKotlin::runBenchmark); }
    @Test public void runFastaKotlin() { measure("FastaKotlin", FastaBenchmarkKotlin.INSTANCE::runBenchmark); }
    @Test public void runMandelbrotKotlin() { measure("MandelbrotKotlin", MandelbrotBenchmarkKotlin::runBenchmark); }
    @Test public void runNBodyKotlin() { measure("NBodyKotlin", NBodyBenchmarkKotlin::runBenchmark); }
    @Test public void runRevCompKotlin() { measure("RevCompKotlin", RevCompBenchmarkKotlin.INSTANCE::runBenchmark); }
    @Test public void runSpectralNormKotlin() { measure("SpectralNormKotlin", SpectralNormBenchmarkKotlin.INSTANCE::runBenchmark); }

    // ==================== C ====================

    @Test public void runBinaryTreesC() { measure("BinaryTreesC", () -> NativeBenchmarks.runBinaryTreesBenchmarkC(4, 18)); }
    @Test public void runFannkuchReduxC() { measure("FannkuchReduxC", () -> NativeBenchmarks.runFannkuchReduxBenchmarkC(10)); }
    @Test public void runFastaC() { measure("FastaC", () -> NativeBenchmarks.runFastaBenchmarkC(10000)); }
    @Test public void runMandelbrotC() { measure("MandelbrotC", () -> NativeBenchmarks.runMandelbrotBenchmarkC(6000)); }
    @Test public void runNBodyC() { measure("NBodyC", () -> NativeBenchmarks.runNBodyBenchmarkC(500000)); }
    @Test public void runRevCompC() { measure("RevCompC", () -> NativeBenchmarks.runRevCompBenchmarkC(generateLargeFasta())); }
    @Test public void runSpectralNormC() { measure("SpectralNormC", () -> NativeBenchmarks.runSpectralNormBenchmarkC(100)); }

    // ==================== C++ ====================

    @Test public void runBinaryTreesCpp() { measure("BinaryTreesCpp", () -> NativeBenchmarks.runBinaryTreesBenchmarkCpp(4, 16)); }
    @Test public void runFannkuchReduxCpp() { measure("FannkuchReduxCpp", () -> NativeBenchmarks.runFannkuchReduxBenchmarkCpp(10)); }
    @Test public void runFastaCpp() { measure("FastaCpp", () -> NativeBenchmarks.runFastaBenchmarkCpp(10000)); }
    @Test public void runMandelbrotCpp() { measure("MandelbrotCpp", () -> NativeBenchmarks.runMandelbrotBenchmarkCpp(6000)); }
    @Test public void runNBodyCpp() { measure("NBodyCpp", () -> NativeBenchmarks.runNBodyBenchmarkCpp(500000)); }
    @Test public void runRevCompCpp() { measure("RevCompCpp", () -> NativeBenchmarks.runRevCompBenchmarkCpp(generateLargeFasta())); }
    @Test public void runSpectralNormCpp() { measure("SpectralNormCpp", () -> NativeBenchmarks.runSpectralNormBenchmarkCpp(100)); }

    // ==================== JAVASCRIPT (Jetpack JavaScriptEngine) ====================

    @Test public void runBinaryTreesJS() {
        measure("BinaryTreesJS",
                () -> runJsBenchmark("BinaryTreesBenchmarkJs.js",
                        "runBinaryTreesBenchmark(16)"));
    }

    @Test public void runFannkuchReduxJS() {
        measure("FannkuchReduxJS",
                () -> runJsBenchmark("FannkuchReduxBenchmarkJs.js",
                        "runFannkuchReduxBenchmark(10)"));
    }

    @Test public void runFastaJS() {
        measure("FastaJS",
                () -> runJsBenchmark("FastaBenchmarkJs.js",
                        "runFastaBenchmark(10000)"));
    }

    @Test public void runMandelbrotJS() {
        measure("MandelbrotJS",
                () -> runJsBenchmark("MandelbrotBenchmarkJs.js",
                        "runMandelbrotBenchmark(6000)"));
    }

    @Test public void runNBodyJS() {
        measure("NBodyJS",
                () -> runJsBenchmark("NBodyBenchmarkJs.js",
                        "runNBodyBenchmark(5000000)"));
    }

    @Test public void runRevCompJS() {
        measure("RevCompJS",
                () -> runJsBenchmark("RevCompBenchmarkJs.js",
                        "runRevCompBenchmark(generateLargeFasta())"));
    }

    @Test public void runSpectralNormJS() {
        measure("SpectralNormJS",
                () -> runJsBenchmark("SpectralNormBenchmarkJs.js",
                        "runSpectralNormBenchmark(5000)"));
    }

    // ==================== Helper ====================

    private String generateLargeFasta() {
        StringBuilder sb = new StringBuilder();
        for (int seq = 0; seq < 1000; seq++) {
            sb.append(">SEQ").append(seq).append("\n");
            for (int line = 0; line < 100; line++) {
                sb.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n");
            }
        }
        return sb.toString();
    }
}

