package com.example.myapplication;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import androidx.activity.ComponentActivity;
import android.widget.TextView;
import android.widget.ScrollView;
import android.widget.LinearLayout;
import java.io.FileOutputStream;
import com.squareup.duktape.Duktape;
import java.io.InputStream;
import java.io.IOException;

public class MainActivity extends ComponentActivity {

    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ScrollView scrollView = new ScrollView(this);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        TextView textView = new TextView(this);
        layout.addView(textView);
        scrollView.addView(layout);
        setContentView(scrollView);

        String benchmarkFromIntent = getIntent().getStringExtra("BENCHMARK");
        String selectedBenchmark = benchmarkFromIntent != null ? benchmarkFromIntent : "BinaryTreesJava";

        // Run benchmark on BACKGROUND THREAD to avoid ANR
        new Thread(() -> {
            try {
                Thread.sleep(3000); // Wait for everything to settle

                // SELECT WHICH BENCHMARK TO RUN HERE
//                String selectedBenchmark = "FastaCpp";

                Log.d("BENCHMARK", "Starting benchmark: " + selectedBenchmark);

                long startTime = System.currentTimeMillis();
                String result = runSelectedBenchmark(selectedBenchmark);
                long duration = System.currentTimeMillis() - startTime;

                Log.d("BENCHMARK", "Completed in " + duration + "ms");

                // Save results
                saveToFile(result);

                // Update UI on main thread
                runOnUiThread(() -> {
                    textView.setText(result + "\n\nTotal time: " + duration + "ms");
                });

            } catch (Exception e) {
                Log.e("BENCHMARK", "Benchmark failed", e);
                runOnUiThread(() -> {
                    textView.setText("Benchmark failed: " + e.getMessage());
                });
            }
        }).start();
    }

    private String runSelectedBenchmark(String benchmark) {
        switch (benchmark) {
            // ============ JAVA BENCHMARKS ============
            case "BinaryTreesJava":
                return BinaryTreesBenchmark.runBenchmark();

            case "FannkuchReduxJava":
                return FannkuchReduxBenchmark.runBenchmark();

            case "FastaJava":
                return FastaBenchmark.runBenchmark();

            case "MandelbrotJava":
                return MandelbrotBenchmark.runBenchmark();

            case "NBodyJava":
                return NBodyBenchmark.runBenchmark();

            case "RevCompJava":
                return RevCompBenchmark.runBenchmark();

            case "SpectralNormJava":
                return SpectralNormBenchmark.runBenchmark();

            // ============ KOTLIN BENCHMARKS ============
            case "BinaryTreesKotlin":
                return BinaryTreesBenchmarkKotlin.INSTANCE.runBenchmark();

            case "FannkuchReduxKotlin":
                return FannkuchReduxBenchmarkKotlin.INSTANCE.runBenchmark();

            case "FastaKotlin":
                return FastaBenchmarkKotlin.INSTANCE.runBenchmark();

            case "MandelbrotKotlin":
                return MandelbrotBenchmarkKotlin.INSTANCE.runBenchmark();

            case "NBodyKotlin":
                return NBodyBenchmarkKotlin.INSTANCE.runBenchmark();

            case "RevCompKotlin":
                return RevCompBenchmarkKotlin.INSTANCE.runBenchmark();

            case "SpectralNormKotlin":
                return SpectralNormBenchmarkKotlin.INSTANCE.runBenchmark();

            // ============ C BENCHMARKS ============

            case "BinaryTreesC":
                return NativeBenchmarks.runBinaryTreesBenchmarkC(4, 16);

            case "FannkuchReduxC":
                return NativeBenchmarks.runFannkuchReduxBenchmarkC(10);

            case "FastaC":
                return NativeBenchmarks.runFastaBenchmarkC(10000);

            case "MandelbrotC":
                return NativeBenchmarks.runMandelbrotBenchmarkC(6000);

            case "NBodyC":
                return NativeBenchmarks.runNBodyBenchmarkC(500000);

            case "RevCompC":
                return NativeBenchmarks.runRevCompBenchmarkC(generateLargeFasta());

            case "SpectralNormC":
                return NativeBenchmarks.runSpectralNormBenchmarkC(100);

            // ============ C++ BENCHMARKS ============
            case "BinaryTreesCpp":
                return NativeBenchmarks.runBinaryTreesBenchmarkCpp(4, 16);

            case "FannkuchReduxCpp":
                return NativeBenchmarks.runFannkuchReduxBenchmarkCpp(10);

            case "FastaCpp":
                return NativeBenchmarks.runFastaBenchmarkCpp(10000);

            case "MandelbrotCpp":
                return NativeBenchmarks.runMandelbrotBenchmarkCpp(8000);

            case "NBodyCpp":
                return NativeBenchmarks.runNBodyBenchmarkCpp(5000000);

            case "RevCompCpp":
                return NativeBenchmarks.runRevCompBenchmarkCpp(generateLargeFasta());

            case "SpectralNormCpp":
                return NativeBenchmarks.runSpectralNormBenchmarkCpp(5000);

// ============ JAVASCRIPT BENCHMARKS ============
            case "BinaryTreesJs":
                return runJsBenchmark("BinaryTreesBenchmarkJs.js",
                        "runBinaryTreesBenchmark(16, 150)");

            case "FannkuchReduxJs":
                return runJsBenchmark("FannkuchReduxBenchmarkJs.js",
                        "runFannkuchReduxBenchmark(10, 80)");

            case "FastaJs":
                return runJsBenchmark("FastaBenchmarkJs.js",
                        "runFastaBenchmark(10000, 500)");

            case "MandelbrotJs":
                return runJsBenchmark("MandelbrotBenchmarkJs.js",
                        "runMandelbrotBenchmark(6000, 15)");

            case "NBodyJs":
                return runJsBenchmark("NBodyBenchmarkJs.js",
                        "runNBodyBenchmark(500000, 143)");

            case "RevCompJs":
                return runJsBenchmark("RevCompBenchmarkJs.js",
                        "runRevCompBenchmark(generateLargeFasta(), 1100)");

            case "SpectralNormJs":
                return runJsBenchmark("SpectralNormBenchmarkJs.js",
                        "runSpectralNormBenchmark(100, 5000)");

            default:
                return "ERROR: Unknown benchmark '" + benchmark + "'";
        }
    }

    private String runJsBenchmark(String jsFile, String jsCall) {
        String jsSource = loadAssetAsString(jsFile);
        if (jsSource == null) {
            return "ERROR: Could not load " + jsFile;
        }

        try (Duktape duktape = Duktape.create()) {
            duktape.evaluate(jsSource);
            return (String) duktape.evaluate(jsCall);
        } catch (Exception e) {
            Log.e("JS", "Error running JS benchmark", e);
            return "JS error: " + e.getMessage();
        }
    }

    private String loadAssetAsString(String filename) {
        try (InputStream is = getAssets().open(filename)) {
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            return new String(buffer, "UTF-8");
        } catch (IOException e) {
            Log.e("ASSET", "Error loading asset: " + filename, e);
            return null;
        }
    }

    private String generateLargeFasta() //match it to java implementation
    {
        StringBuilder sb = new StringBuilder();
        for (int seq = 0; seq < 1000; seq++) {
            sb.append(">SEQ").append(seq).append("\n");
            for (int line = 0; line < 100; line++) {
                sb.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n");
            }
        }
        return sb.toString();
    }

    private void saveToFile(String text) {
        try (FileOutputStream fos = openFileOutput("benchmark_results.txt", MODE_PRIVATE)) {
            fos.write(text.getBytes());
            Log.d("FILE", "Results saved");
        } catch (Exception e) {
            Log.e("FILE", "Error writing results", e);
        }
    }
}
