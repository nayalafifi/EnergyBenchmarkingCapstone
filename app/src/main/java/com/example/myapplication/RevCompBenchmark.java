package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

public final class RevCompBenchmark {

    static final byte[] map = new byte[128];

    static {
        String[] mm = {"ACBDGHK\nMNSRUTWVYacbdghkmnsrutwvy",
                "TGVHCDM\nKNSYAAWBRTGVHCDMKNSYAAWBR"};
        for (int i = 0; i < mm[0].length(); i++)
            map[mm[0].charAt(i)] = (byte) mm[1].charAt(i);
    }

    public static String runBenchmark() {
        Trace.beginSection("RevComp Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            // Generate larger input - 1000 sequences
            StringBuilder inputBuilder = new StringBuilder();
            for (int seq = 0; seq < 1000; seq++) {
                inputBuilder.append(">SEQ").append(seq).append("\n");
                for (int line = 0; line < 100; line++) {
                    inputBuilder.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n");
                }
            }
            String input = inputBuilder.toString();

            // Run 1000 iterations as baseline
            for (int iteration = 0; iteration < 1000; iteration++) {
                byte[] buf = input.getBytes();

                // Simple single-threaded reverse complement
                for (int i = 0; i < buf.length; i++) {
                    byte b = buf[i];
                    if (b != '\n' && b != '>') {
                        buf[i] = map[b];
                    }
                }

                // Reverse each sequence
                int seqStart = 0;
                for (int i = 0; i < buf.length; i++) {
                    if (buf[i] == '>') {
                        if (i > seqStart) {
                            reverseSection(buf, seqStart, i - 1);
                        }
                        seqStart = i;
                        while (i < buf.length && buf[i] != '\n') i++;
                        seqStart = i + 1;
                    }
                }
                if (seqStart < buf.length) {
                    reverseSection(buf, seqStart, buf.length - 1);
                }
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "RevComp Java duration: " + duration + "ms");

            return "RevComp benchmark completed: " + duration + "ms";
        } finally {
            Trace.endSection();
        }
    }

    private static void reverseSection(byte[] buf, int start, int end) {
        while (start < end) {
            if (buf[start] == '\n') {
                start++;
                continue;
            }
            if (buf[end] == '\n') {
                end--;
                continue;
            }
            byte temp = buf[start];
            buf[start] = buf[end];
            buf[end] = temp;
            start++;
            end--;
        }
    }
}