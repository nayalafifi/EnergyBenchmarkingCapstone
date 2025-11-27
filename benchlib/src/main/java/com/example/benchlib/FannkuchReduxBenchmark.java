package com.example.benchlib;

import android.os.Trace;
import android.util.Log;
import java.util.concurrent.atomic.AtomicInteger;

public final class FannkuchReduxBenchmark implements Runnable {

    private static final int NCHUNKS = 150;
    private static int CHUNKSZ;
    private static int NTASKS;
    private static int n;
    private static int[] Fact;
    private static int[] maxFlips;
    private static int[] chkSums;
    private static AtomicInteger taskId;

    int[] p, pp, count;

    void firstPermutation(int idx) {
        for (int i = 0; i < p.length; ++i) {
            p[i] = i;
        }

        for (int i = count.length - 1; i > 0; --i) {
            int d = idx / Fact[i];
            count[i] = d;
            idx = idx % Fact[i];

            System.arraycopy(p, 0, pp, 0, i + 1);
            for (int j = 0; j <= i; ++j) {
                p[j] = j + d <= i ? pp[j + d] : pp[j + d - i - 1];
            }
        }
    }

    void nextPermutation() {
        int first = p[1];
        p[1] = p[0];
        p[0] = first;

        int i = 1;
        while (++count[i] > i) {
            count[i++] = 0;
            int next = p[0] = p[1];
            for (int j = 1; j < i; ++j) {
                p[j] = p[j + 1];
            }
            p[i] = first;
            first = next;
        }
    }

    int countFlips() {
        int flips = 1;
        int first = p[0];
        if (p[first] != 0) {
            System.arraycopy(p, 0, pp, 0, pp.length);
            do {
                ++flips;
                for (int lo = 1, hi = first - 1; lo < hi; ++lo, --hi) {
                    int t = pp[lo];
                    pp[lo] = pp[hi];
                    pp[hi] = t;
                }
                int t = pp[first];
                pp[first] = first;
                first = t;
            } while (pp[first] != 0);
        }
        return flips;
    }

    void runTask(int task) {
        int idxMin = task * CHUNKSZ;
        int idxMax = Math.min(Fact[n], idxMin + CHUNKSZ);

        firstPermutation(idxMin);

        int maxflips = 1;
        int chksum = 0;
        for (int i = idxMin;;) {

            if (p[0] != 0) {
                int flips = countFlips();
                maxflips = Math.max(maxflips, flips);
                chksum += i % 2 == 0 ? flips : -flips;
            }

            if (++i == idxMax) {
                break;
            }

            nextPermutation();
        }
        maxFlips[task] = maxflips;
        chkSums[task] = chksum;
    }

    @Override
    public void run() {
        p = new int[n];
        pp = new int[n];
        count = new int[n];

        int task;
        while ((task = taskId.getAndIncrement()) < NTASKS) {
            runTask(task);
        }
    }

    public static String runBenchmark() {
        Trace.beginSection("FannkuchRedux Benchmark");

        long startTime = System.currentTimeMillis();
        int iterations = 60;

        try {
            for (int iteration = 0; iteration < iterations; iteration++) {
                n = 10;  // Input parameter - matches reference (can be 1-12)

                // Skip invalid inputs - matches reference validation
                if (n <= 1) {
                    continue;
                }

                // Compute factorials
                Fact = new int[n + 1];
                Fact[0] = 1;
                for (int i = 1; i < Fact.length; ++i) {
                    Fact[i] = Fact[i - 1] * i;
                }

                // Calculate chunking parameters
                CHUNKSZ = (Fact[n] + NCHUNKS - 1) / NCHUNKS;
                NTASKS = (Fact[n] + CHUNKSZ - 1) / CHUNKSZ;
                maxFlips = new int[NTASKS];
                chkSums = new int[NTASKS];
                taskId = new AtomicInteger(0);

                // Create and start worker threads
                int nthreads = Runtime.getRuntime().availableProcessors();
                Thread[] threads = new Thread[nthreads];
                for (int i = 0; i < nthreads; ++i) {
                    threads[i] = new Thread(new FannkuchReduxBenchmark());
                    threads[i].start();
                }

                // Wait for all threads to complete
                for (Thread t : threads) {
                    try {
                        t.join();
                    } catch (InterruptedException e) {
                        Log.e("BENCHMARK", "Thread interrupted", e);
                    }
                }

                // Aggregate results
                int res = 0;
                for (int v : maxFlips) {
                    res = Math.max(res, v);
                }
                int chk = 0;
                for (int v : chkSums) {
                    chk += v;
                }

                // Optionally log intermediate results (commented out for performance)
                // Log.d("BENCHMARK", "Iteration " + iteration + ": chk=" + chk + ", max=" + res);
            }

            long duration = System.currentTimeMillis() - startTime;
            String result = "FannkuchRedux completed: " + duration + "ms (" + iterations + " iterations)";
            Log.d("BENCHMARK", result);

            return result;
        } finally {
            Trace.endSection();
        }
    }
}