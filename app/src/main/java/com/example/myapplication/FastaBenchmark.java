package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

public final class FastaBenchmark {

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

            // Run 3300 iterations to get ~40 seconds
            for (int iteration = 0; iteration < 3300; iteration++) {
                StringBuilder temp = new StringBuilder();
                temp.append(">ONE Homo sapiens alu\n");
                generateRepeat(ALU, n * 2, temp);

                temp.append(">TWO IUB ambiguity codes\n");
                generateRandom(n * 3, temp, IUB_CODES, IUB_PROBS);

                temp.append(">THREE Homo sapiens frequency\n");
                generateRandom(n * 5, temp, HOMO_SAPIENS_CODES, HOMO_SAPIENS_PROBS);
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "Fasta Java duration: " + duration + "ms");

            return "Fasta benchmark completed: " + duration + "ms";
        } finally {
            Trace.endSection();
        }
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

    private static void generateRandom(int n, StringBuilder out, char[] codes, double[] probs) {
        double[] cumProbs = new double[probs.length];
        cumProbs[0] = probs[0];
        for (int i = 1; i < probs.length; i++) cumProbs[i] = cumProbs[i - 1] + probs[i];

        int count = 0;
        for (int i = 0; i < n; i++) {
            out.append(selectRandom(codes, cumProbs));
            count++;
            if (count == 60) {
                out.append('\n');
                count = 0;
            }
        }
        if (count > 0) out.append('\n');
    }

    private static char selectRandom(char[] codes, double[] cumProbs) {
        double r = Math.random();
        for (int i = 0; i < cumProbs.length; i++) {
            if (r < cumProbs[i]) return codes[i];
        }
        return codes[codes.length - 1];
    }

    private static final char[] IUB_CODES = {
            'a', 'c', 'g', 't',
            'B', 'D', 'H', 'K',
            'M', 'N', 'R', 'S',
            'V', 'W', 'Y'
    };

    private static final double[] IUB_PROBS = {
            0.27, 0.12, 0.12, 0.27,
            0.02, 0.02, 0.02, 0.02,
            0.02, 0.02, 0.02, 0.02,
            0.02, 0.02, 0.02
    };

    private static final char[] HOMO_SAPIENS_CODES = {
            'a', 'c', 'g', 't'
    };

    private static final double[] HOMO_SAPIENS_PROBS = {
            0.3029549426680,
            0.1979883004921,
            0.1975473066391,
            0.3015094502008
    };
}