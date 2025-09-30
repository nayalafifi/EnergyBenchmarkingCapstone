package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

public final class FannkuchReduxBenchmark {

    public static String runBenchmark() {
        Trace.beginSection("FannkuchRedux Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            int n = 10;
            StringBuilder output = new StringBuilder();

            // Run 80 times to extend duration to 30-60 seconds
            for (int iteration = 0; iteration < 80; iteration++) {
                int[] perm = new int[n];
                int[] perm1 = new int[n];
                int[] count = new int[n];
                int maxFlips = 0, r = n;
                int checkSum = 0;
                int m = n - 1;

                for (int i = 0; i < n; i++) perm1[i] = i;

                while (true) {
                    System.arraycopy(perm1, 0, perm, 0, n);
                    int flips = 0;
                    int k;

                    while ((k = perm[0]) != 0) {
                        for (int i = 0, j = k; i < j; i++, j--) {
                            int t = perm[i];
                            perm[i] = perm[j];
                            perm[j] = t;
                        }
                        flips++;
                    }

                    checkSum += (perm1[0] & 1) == 0 ? flips : -flips;
                    if (flips > maxFlips) maxFlips = flips;

                    while (r != 1) {
                        count[r - 1] = r;
                        r--;
                    }

                    while (true) {
                        if (r == n) {
                            output.append("Checksum: ").append(checkSum).append("\n");
                            output.append("Maximum flips: ").append(maxFlips).append("\n");
                            break; // Break out of inner while loop
                        }
                        int perm0 = perm1[0];
                        for (int i = 0; i < r; i++) perm1[i] = perm1[i + 1];
                        perm1[r] = perm0;
                        count[r]--;
                        if (count[r] > 0) break;
                        r++;
                    }

                    if (r == n) break; // Break out of outer while loop
                }
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "FannkuchRedux duration: " + duration + "ms");

            return output.toString();
        } finally {
            Trace.endSection();
        }
    }
}