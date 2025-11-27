package com.example.benchlib

import android.os.Trace
import android.util.Log
import kotlin.math.min

object FastaBenchmarkKotlin {
    private const val IM = 139968
    private const val IA = 3877
    private const val IC = 29573
    private const val ONE_OVER_IM = 1f / IM
    private var last = 42

    fun runBenchmark(): String {
        Trace.beginSection("Fasta Benchmark")

        val startTime = System.currentTimeMillis()

        try {
            val n = 10000
            val ALU =
                "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
                        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
                        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
                        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
                        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
                        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
                        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

            // Pre-compute cumulative probabilities as floats (like reference)
            val iubProbs = computeCumulativeProbs(IUB_PROBS)
            val sapienProbs = computeCumulativeProbs(HOMO_SAPIENS_PROBS)

            for (iteration in 0..1) {
                val temp = StringBuilder()
                temp.append(">ONE Homo sapiens alu\n")
                generateRepeat(ALU, n * 2, temp)

                temp.append(">TWO IUB ambiguity codes\n")
                generateRandom(n * 3, temp, IUB_CODES, iubProbs)

                temp.append(">THREE Homo sapiens frequency\n")
                generateRandom(n * 5, temp, HOMO_SAPIENS_CODES, sapienProbs)
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "Fasta Java duration: " + duration + "ms")

            return "Fasta benchmark completed: " + duration + "ms"
        } finally {
            Trace.endSection()
        }
    }

    private fun computeCumulativeProbs(probs: DoubleArray): FloatArray {
        val cumProbs = FloatArray(probs.size)
        var cp = 0.0
        for (i in probs.indices) {
            cp += probs[i]
            cumProbs[i] = cp.toFloat()
        }
        cumProbs[cumProbs.size - 1] = 2f // Sentinel value like reference
        return cumProbs
    }

    private fun generateRepeat(seed: String, n: Int, out: StringBuilder) {
        var n = n
        val length = seed.length
        var pos = 0
        while (n > 0) {
            val lineLen = min(60.0, n.toDouble()).toInt()
            if (pos + lineLen < length) {
                out.append(seed, pos, pos + lineLen)
                pos += lineLen
            } else {
                out.append(seed.substring(pos))
                out.append(seed, 0, lineLen - (length - pos))
                pos = (pos + lineLen) % length
            }
            out.append('\n')
            n -= lineLen
        }
    }

    private fun generateRandom(n: Int, out: StringBuilder, codes: CharArray, cumProbs: FloatArray) {
        var count = 0
        for (i in 0..<n) {
            // Use deterministic LCG like reference implementation
            last = (last * IA + IC) % IM
            val r = last * ONE_OVER_IM

            // Find character using linear search (matches reference)
            var m = 0
            while (cumProbs[m] < r) {
                m++
            }
            out.append(codes[m])

            count++
            if (count == 60) {
                out.append('\n')
                count = 0
            }
        }
        if (count > 0) out.append('\n')
    }

    // IUB codes - matches reference byte[] iubChars
    private val IUB_CODES = charArrayOf(
        'a', 'c', 'g', 't',
        'B', 'D', 'H', 'K',
        'M', 'N', 'R', 'S',
        'V', 'W', 'Y'
    )

    // IUB probabilities - matches reference double[] iubProbs
    private val IUB_PROBS = doubleArrayOf(
        0.27, 0.12, 0.12, 0.27,
        0.02, 0.02, 0.02, 0.02,
        0.02, 0.02, 0.02, 0.02,
        0.02, 0.02, 0.02
    )

    // Homo sapiens codes - matches reference byte[] sapienChars
    private val HOMO_SAPIENS_CODES = charArrayOf(
        'a', 'c', 'g', 't'
    )

    // Homo sapiens probabilities - matches reference double[] sapienProbs
    private val HOMO_SAPIENS_PROBS = doubleArrayOf(
        0.3029549426680,
        0.1979883004921,
        0.1975473066391,
        0.3015094502008
    )
}