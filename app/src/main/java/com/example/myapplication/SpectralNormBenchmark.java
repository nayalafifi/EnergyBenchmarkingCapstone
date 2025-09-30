package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

import java.text.DecimalFormat;

public final class SpectralNormBenchmark {

    private static final DecimalFormat formatter = new DecimalFormat("#.000000000");
    private static final int NCPU = Runtime.getRuntime().availableProcessors();

    public static String runBenchmark() {
        Trace.beginSection("SpectralNorm Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            final int n = 100;

            // Run 5000 iterations as baseline
            for (int iteration = 0; iteration < 400; iteration++) {
                final double[] u = new double[n];
                final double[] v = new double[n];
                for (int i = 0; i < n; i++) u[i] = 1.0;

                for (int i = 0; i < 10; i++) {
                    aTimesTransp(v, u, n);
                    aTimesTransp(u, v, n);
                }
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "SpectralNorm Java duration: " + duration + "ms");

            return "SpectralNorm benchmark completed: " + duration + "ms";
        } catch (Exception e) {
            return "SpectralNorm Benchmark failed: " + e.getMessage();
        } finally {
            Trace.endSection();
        }
    }

    private static void aTimesTransp(double[] v, double[] u, int n) throws InterruptedException {
        final double[] x = new double[n];
        final Thread[] t = new Thread[NCPU];

        for (int i = 0; i < NCPU; i++) {
            int start = i * n / NCPU;
            int end = (i + 1) * n / NCPU;
            t[i] = new Times(x, start, end, u, false, n);
            t[i].start();
        }
        for (Thread thread : t) thread.join();

        for (int i = 0; i < NCPU; i++) {
            int start = i * n / NCPU;
            int end = (i + 1) * n / NCPU;
            t[i] = new Times(v, start, end, x, true, n);
            t[i].start();
        }
        for (Thread thread : t) thread.join();
    }

    private static final class Times extends Thread {
        private final double[] v, u;
        private final int start, end, size;
        private final boolean transpose;

        Times(double[] v, int start, int end, double[] u, boolean transpose, int size) {
            this.v = v;
            this.start = start;
            this.end = end;
            this.u = u;
            this.transpose = transpose;
            this.size = size;
        }

        @Override
        public void run() {
            for (int i = start; i < end; i++) {
                double sum = 0.0;
                for (int j = 0; j < size; j++) {
                    sum += u[j] / a(transpose ? j : i, transpose ? i : j);
                }
                v[i] = sum;
            }
        }

        private int a(int i, int j) {
            return (i + j) * (i + j + 1) / 2 + i + 1;
        }
    }
}