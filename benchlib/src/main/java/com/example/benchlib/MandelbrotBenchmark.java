package com.example.benchlib;

import android.os.Trace;
import android.util.Log;

import java.util.concurrent.atomic.AtomicInteger;

public final class MandelbrotBenchmark {
    static byte[][] out;
    static AtomicInteger yCt;
    static double[] Crb;
    static double[] Cib;

    static int getByte(int x, int y) {
        int res = 0;
        for (int i = 0; i < 8; i += 2) {
            double Zr1 = Crb[x + i];
            double Zi1 = Cib[y];

            double Zr2 = Crb[x + i + 1];
            double Zi2 = Cib[y];

            int b = 0;
            int j = 49;
            do {
                double nZr1 = Zr1 * Zr1 - Zi1 * Zi1 + Crb[x + i];
                double nZi1 = Zr1 * Zi1 + Zr1 * Zi1 + Cib[y];
                Zr1 = nZr1;
                Zi1 = nZi1;

                double nZr2 = Zr2 * Zr2 - Zi2 * Zi2 + Crb[x + i + 1];
                double nZi2 = Zr2 * Zi2 + Zr2 * Zi2 + Cib[y];
                Zr2 = nZr2;
                Zi2 = nZi2;

                if (Zr1 * Zr1 + Zi1 * Zi1 > 4) {
                    b |= 2;
                    if (b == 3) break;
                }
                if (Zr2 * Zr2 + Zi2 * Zi2 > 4) {
                    b |= 1;
                    if (b == 3) break;
                }
            } while (--j > 0);
            res = (res << 2) + b;
        }
        return ~res;
    }

    static void putLine(int y, byte[] line) {
        for (int xb = 0; xb < line.length; xb++)
            line[xb] = (byte) getByte(xb * 8, y);
    }

    public static String runBenchmark() {
        Trace.beginSection("Mandelbrot Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            int N = 6000;
            int iterations = 1;

            for (int it = 0; it < iterations; it++) {
                // Initialize static arrays - matches reference
                Crb = new double[N + 7];
                Cib = new double[N + 7];
                double invN = 2.0 / N;
                for (int i = 0; i < N; i++) {
                    Cib[i] = i * invN - 1.0;
                    Crb[i] = i * invN - 1.5;
                }
                yCt = new AtomicInteger();
                out = new byte[N][(N + 7) / 8];

                // Create thread pool - matches reference
                Thread[] pool = new Thread[2 * Runtime.getRuntime().availableProcessors()];
                for (int i = 0; i < pool.length; i++) {
                    pool[i] = new Thread() {
                        public void run() {
                            int y;
                            while ((y = yCt.getAndIncrement()) < out.length)
                                putLine(y, out[y]);
                        }
                    };
                }
                for (Thread t : pool) t.start();
                for (Thread t : pool) t.join();
            }

            long duration = System.currentTimeMillis() - startTime;
            String result = "Mandelbrot completed: " + duration + "ms (" + iterations + " iterations)";
            Log.d("BENCHMARK", result);

            return result;

        } catch (InterruptedException e) {
            Log.e("BENCHMARK", "Benchmark interrupted", e);
            return "Benchmark interrupted.";
        } finally {
            Trace.endSection();
        }
    }
}