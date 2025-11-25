//package com.example.myapplication;
//
//import android.annotation.SuppressLint;
//import android.content.Intent;
//import android.os.Bundle;
//import android.util.Log;
//import androidx.activity.ComponentActivity;
//import android.widget.TextView;
//import android.widget.ScrollView;
//import android.widget.LinearLayout;
//import java.io.FileOutputStream;
//import com.squareup.duktape.Duktape;
//import java.io.InputStream;
//import java.io.IOException;
//import java.nio.charset.StandardCharsets;
//
//import com.example.benchlib.NativeBenchmarks;
//import com.example.benchlib.BinaryTreesBenchmark;
//import com.example.benchlib.FannkuchReduxBenchmark;
//import com.example.benchlib.FastaBenchmark;
//import com.example.benchlib.MandelbrotBenchmark;
//import com.example.benchlib.NBodyBenchmark;
//import com.example.benchlib.RevCompBenchmark;
//import com.example.benchlib.SpectralNormBenchmark;
//
//import com.example.benchlib.BinaryTreesBenchmarkKotlin;
//import com.example.benchlib.FannkuchReduxBenchmarkKotlin;
//import com.example.benchlib.FastaBenchmarkKotlin;
//import com.example.benchlib.MandelbrotBenchmarkKotlin;
//import com.example.benchlib.NBodyBenchmarkKotlin;
//import com.example.benchlib.RevCompBenchmarkKotlin;
//import com.example.benchlib.SpectralNormBenchmarkKotlin;
//
//public class MainActivity extends ComponentActivity {
//
//    @SuppressLint("SetTextI18n")
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//
//        ScrollView scrollView = new ScrollView(this);
//        LinearLayout layout = new LinearLayout(this);
//        layout.setOrientation(LinearLayout.VERTICAL);
//        TextView textView = new TextView(this);
//        layout.addView(textView);
//        scrollView.addView(layout);
//        setContentView(scrollView);
//
//        final Intent intent = getIntent();
//        final String benchmarkFromIntent = intent.getStringExtra("BENCHMARK");
//        final String languageFromIntent = intent.getStringExtra("LANGUAGE");
//
//        final String selectedBenchmark;
//        if (benchmarkFromIntent != null && languageFromIntent != null) {
//            selectedBenchmark = benchmarkFromIntent + languageFromIntent;
//            Log.d("BENCHMARK", "Automated mode: " + selectedBenchmark);
//        } else {
//            selectedBenchmark = "BinaryTreesC";
//            Log.d("BENCHMARK", "Manual mode: " + selectedBenchmark);
//        }
//
//        new Thread(() -> {
//            try {
//                Thread.sleep(1000); // settle
//
//                Log.d("BENCHMARK", "Starting benchmark: " + selectedBenchmark);
//
//                long startTime = System.currentTimeMillis();
//                String result = runSelectedBenchmark(selectedBenchmark);
//                long duration = System.currentTimeMillis() - startTime;
//
//                Log.d("BENCHMARK", selectedBenchmark + " completed: " + duration + "ms");
//                Log.d("BENCHMARK_RESULT", result);
//
//                saveToFile(result);
//
//                runOnUiThread(() -> textView.setText(result + "\n\nTotal time: " + duration + "ms"));
//
//                // 🔔 broadcast completion so the test can wait reliably
//                Intent done = new Intent("com.example.myapplication.BENCHMARK_DONE");
//                done.putExtra("selected", selectedBenchmark);
//                done.putExtra("durationMs", duration);
//                sendBroadcast(done);
//
//                // Auto-exit if launched by automation
//                if (benchmarkFromIntent != null) {
//                    Thread.sleep(2000);
//                    runOnUiThread(this::finish);
//                }
//
//            } catch (Exception e) {
//                Log.e("BENCHMARK", "Benchmark failed", e);
//                runOnUiThread(() -> textView.setText("Benchmark failed: " + e.getMessage()));
//            }
//        }).start();
//    }
//
//    private String runSelectedBenchmark(String benchmark) {
//        switch (benchmark) {
//            // ============ JAVA ============
//            case "BinaryTreesJava": return BinaryTreesBenchmark.runBenchmark();
//            case "FannkuchReduxJava": return FannkuchReduxBenchmark.runBenchmark();
//            case "FastaJava": return FastaBenchmark.runBenchmark();
//            case "MandelbrotJava": return MandelbrotBenchmark.runBenchmark();
//            case "NBodyJava": return NBodyBenchmark.runBenchmark();
//            case "RevCompJava": return RevCompBenchmark.runBenchmark();
//            case "SpectralNormJava": return SpectralNormBenchmark.runBenchmark();
//
//            // ============ KOTLIN ============
//            case "BinaryTreesKotlin": return BinaryTreesBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "FannkuchReduxKotlin": return FannkuchReduxBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "FastaKotlin": return FastaBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "MandelbrotKotlin": return MandelbrotBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "NBodyKotlin": return NBodyBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "RevCompKotlin": return RevCompBenchmarkKotlin.INSTANCE.runBenchmark();
//            case "SpectralNormKotlin": return SpectralNormBenchmarkKotlin.INSTANCE.runBenchmark();
//
//            // ============ C ============
//            case "BinaryTreesC": return NativeBenchmarks.runBinaryTreesBenchmarkC(4, 16);
//            case "FannkuchReduxC": return NativeBenchmarks.runFannkuchReduxBenchmarkC(10);
//            case "FastaC": return NativeBenchmarks.runFastaBenchmarkC(10000);
//            case "MandelbrotC": return NativeBenchmarks.runMandelbrotBenchmarkC(6000);
//            case "NBodyC": return NativeBenchmarks.runNBodyBenchmarkC(500000);
//            case "RevCompC": return NativeBenchmarks.runRevCompBenchmarkC(generateLargeFasta());
//            case "SpectralNormC": return NativeBenchmarks.runSpectralNormBenchmarkC(100);
//
//            // ============ C++ ============
//            case "BinaryTreesCpp": return NativeBenchmarks.runBinaryTreesBenchmarkCpp(4, 16);
//            case "FannkuchReduxCpp": return NativeBenchmarks.runFannkuchReduxBenchmarkCpp(10);
//            case "FastaCpp": return NativeBenchmarks.runFastaBenchmarkCpp(10000);
//            case "MandelbrotCpp": return NativeBenchmarks.runMandelbrotBenchmarkCpp(8000);
//            case "NBodyCpp": return NativeBenchmarks.runNBodyBenchmarkCpp(5000000);
//            case "RevCompCpp": return NativeBenchmarks.runRevCompBenchmarkCpp(generateLargeFasta());
//            case "SpectralNormCpp": return NativeBenchmarks.runSpectralNormBenchmarkCpp(5000);
//
//    private String runJsBenchmark(String jsFile, String jsCall) {
//        String jsSource = loadAssetAsString(jsFile);
//        if (jsSource == null) return "ERROR: Could not load " + jsFile;
//        try (Duktape duktape = Duktape.create()) {
//            duktape.evaluate(jsSource);
//            return (String) duktape.evaluate(jsCall);
//        } catch (Exception e) {
//            Log.e("JS", "Error running JS benchmark", e);
//            return "JS error: " + e.getMessage();
//        }
//    }
//
//    private String loadAssetAsString(String filename) {
//        try (InputStream is = getAssets().open(filename)) {
//            int size = is.available();
//            byte[] buffer = new byte[size];
//            is.read(buffer);
//            return new String(buffer, StandardCharsets.UTF_8);
//        } catch (IOException e) {
//            Log.e("ASSET", "Error loading asset: " + filename, e);
//            return null;
//        }
//    }
//
//    private String generateLargeFasta() {
//        StringBuilder sb = new StringBuilder();
//        for (int seq = 0; seq < 1000; seq++) {
//            sb.append(">SEQ").append(seq).append("\n");
//            for (int line = 0; line < 100; line++) {
//                sb.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n");
//            }
//        }
//        return sb.toString();
//    }
//
//    private void saveToFile(String text) {
//        try (FileOutputStream fos = openFileOutput("benchmark_results.txt", MODE_PRIVATE)) {
//            fos.write(text.getBytes());
//            Log.d("FILE", "Results saved");
//        } catch (Exception e) {
//            Log.e("FILE", "Error writing results", e);
//        }
//    }
//}
