package com.example.benchlib;

import android.os.Trace;
import android.util.Log;

public final class FastaBenchmark {

    private static final int IM = 139968;
    private static final int IA = 3877;
    private static final int IC = 29573;
    private static final float ONE_OVER_IM = 1f / IM;
    private static int last = 42;

    public static String runBenchmark() {
        Trace.beginSection("Fasta Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            int n = 10000;
            String ALU =
                    "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
                            "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
                            "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
                            "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
                            "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
                            "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
                            "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

            // Pre-compute cumulative probabilities as floats (like reference)
            float[] iubProbs = computeCumulativeProbs(IUB_PROBS);
            float[] sapienProbs = computeCumulativeProbs(HOMO_SAPIENS_PROBS);

            for (int iteration = 0; iteration < 1; iteration++) {
                StringBuilder temp = new StringBuilder();
                temp.append(">ONE Homo sapiens alu\n");
                generateRepeat(ALU, n * 2, temp);

                temp.append(">TWO IUB ambiguity codes\n");
                generateRandom(n * 3, temp, IUB_CODES, iubProbs);

                temp.append(">THREE Homo sapiens frequency\n");
                generateRandom(n * 5, temp, HOMO_SAPIENS_CODES, sapienProbs);
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "Fasta Java duration: " + duration + "ms");

            return "Fasta benchmark completed: " + duration + "ms";
        } finally {
            Trace.endSection();
        }
    }

    private static float[] computeCumulativeProbs(double[] probs) {
        float[] cumProbs = new float[probs.length];
        double cp = 0;
        for (int i = 0; i < probs.length; i++) {
            cp += probs[i];
            cumProbs[i] = (float) cp;
        }
        cumProbs[cumProbs.length - 1] = 2f; // Sentinel value like reference
        return cumProbs;
    }

    private static void generateRepeat(String seed, int n, StringBuilder out) {
        int length = seed.length();
        int pos = 0;
        while (n > 0) {
            int lineLen = Math.min(60, n);
            if (pos + lineLen < length) {
                out.append(seed, pos, pos + lineLen);
                pos += lineLen;
            } else {
                out.append(seed.substring(pos));
                out.append(seed, 0, lineLen - (length - pos));
                pos = (pos + lineLen) % length;
            }
            out.append('\n');
            n -= lineLen;
        }
    }

    private static void generateRandom(int n, StringBuilder out, char[] codes, float[] cumProbs) {
        int count = 0;
        for (int i = 0; i < n; i++) {
            // Use deterministic LCG like reference implementation
            last = (last * IA + IC) % IM;
            float r = last * ONE_OVER_IM;

            // Find character using linear search (matches reference)
            int m = 0;
            while (cumProbs[m] < r) {
                m++;
            }
            out.append(codes[m]);

            count++;
            if (count == 60) {
                out.append('\n');
                count = 0;
            }
        }
        if (count > 0) out.append('\n');
    }

    // IUB codes - matches reference byte[] iubChars
    private static final char[] IUB_CODES = {
            'a', 'c', 'g', 't',
            'B', 'D', 'H', 'K',
            'M', 'N', 'R', 'S',
            'V', 'W', 'Y'
    };

    // IUB probabilities - matches reference double[] iubProbs
    private static final double[] IUB_PROBS = {
            0.27, 0.12, 0.12, 0.27,
            0.02, 0.02, 0.02, 0.02,
            0.02, 0.02, 0.02, 0.02,
            0.02, 0.02, 0.02
    };

    // Homo sapiens codes - matches reference byte[] sapienChars
    private static final char[] HOMO_SAPIENS_CODES = {
            'a', 'c', 'g', 't'
    };

    // Homo sapiens probabilities - matches reference double[] sapienProbs
    private static final double[] HOMO_SAPIENS_PROBS = {
            0.3029549426680,
            0.1979883004921,
            0.1975473066391,
            0.3015094502008
    };
}