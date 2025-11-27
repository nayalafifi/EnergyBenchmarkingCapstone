package com.example.myapplication;

import android.content.Context;
import android.util.Log;

import androidx.javascriptengine.JavaScriptSandbox;
import androidx.javascriptengine.JavaScriptIsolate;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class JsEngineRunner {

    private static final String TAG = "JsEngineRunner";

    // Single-thread executor recommended for JS evaluation
    private static final ExecutorService executor =
            Executors.newSingleThreadExecutor();

    public static String runJs(Context context, String jsSource, String jsCall) {
        try {
            // 1. Create Sandbox (async)
            Future<JavaScriptSandbox> sandboxFuture =
                    JavaScriptSandbox.createConnectedInstanceAsync(context);

            JavaScriptSandbox sandbox = sandboxFuture.get();

            // 2. Create isolate
            JavaScriptIsolate isolate = sandbox.createIsolate();

            // 3. Load JS Source
            Future<String> loadFuture =
                    isolate.evaluateJavaScriptAsync(jsSource);

            String loadResult = loadFuture.get();
            if (loadResult != null) {
                Log.d(TAG, "JS Loaded: " + loadResult);
            }

            // 4. Run function
            Future<String> callFuture =
                    isolate.evaluateJavaScriptAsync(jsCall);

            String output = callFuture.get();

            // 5. Clean up
            isolate.close();
            sandbox.close();

            return output != null ? output : "null";

        } catch (Exception e) {
            Log.e(TAG, "JS Engine error", e);
            return "JS ERROR: " + e.getMessage();
        }
    }
}

