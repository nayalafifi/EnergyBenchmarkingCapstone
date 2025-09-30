package com.example.myapplication

import android.os.Trace
import android.util.Log
import kotlin.math.min
import kotlin.random.Random

object FastaBenchmarkKotlin {

    private val IUB_CODES = charArrayOf(
        'a', 'c', 'g', 't',
        'B', 'D', 'H', 'K',
        'M', 'N', 'R', 'S',
        'V', 'W', 'Y'
    )

    private val IUB_PROBS = doubleArrayOf(
        0.27, 0.12, 0.12, 0.27,
        0.02, 0.02, 0.02, 0.02,
        0.02, 0.02, 0.02, 0.02,
        0.02, 0.02, 0.02
    )

    private val HOMO_SAPIENS_CODES = charArrayOf('a', 'c', 'g', 't')

    private val HOMO_SAPIENS_PROBS = doubleArrayOf(
        0.3029549426680,
        0.1979883004921,
        0.1975473066391,
        0.3015094502008
    )

    fun runBenchmark(): String {
        Trace.beginSection("Fasta Benchmark")

        val startTime = System.currentTimeMillis()

        return try {
            val n = 10000  // Increased from 1000 to 10000
            val ALU =
                "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
                        "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
                        "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
                        "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
                        "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
                        "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
                        "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

            // Run 3300 iterations to match Java
            for (iteration in 0 until 500) {
                val temp = StringBuilder()
                temp.append(">ONE Homo sapiens alu\n")
                generateRepeat(ALU, n * 2, temp)

                temp.append(">TWO IUB ambiguity codes\n")
                generateRandom(n * 3, temp, IUB_CODES, IUB_PROBS)

                temp.append(">THREE Homo sapiens frequency\n")
                generateRandom(n * 5, temp, HOMO_SAPIENS_CODES, HOMO_SAPIENS_PROBS)
                // temp gets garbage collected
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "Fasta Kotlin duration: ${duration}ms")

            "Fasta benchmark completed: ${duration}ms"
        } finally {
            Trace.endSection()
        }
    }

    private fun generateRepeat(seed: String, n: Int, out: StringBuilder) {
        val length = seed.length
        var pos = 0
        var remaining = n
        while (remaining > 0) {
            val lineLen = min(60, remaining)
            if (pos + lineLen < length) {
                out.append(seed.substring(pos, pos + lineLen))
                pos += lineLen
            } else {
                out.append(seed.substring(pos))
                out.append(seed.substring(0, lineLen - (length - pos)))
                pos = (pos + lineLen) % length
            }
            out.append('\n')
            remaining -= lineLen
        }
    }

    private fun generateRandom(n: Int, out: StringBuilder, codes: CharArray, probs: DoubleArray) {
        val cumProbs = DoubleArray(probs.size)
        cumProbs[0] = probs[0]
        for (i in 1 until probs.size) {
            cumProbs[i] = cumProbs[i - 1] + probs[i]
        }

        var count = 0
        repeat(n) {
            out.append(selectRandom(codes, cumProbs))
            count++
            if (count == 60) {
                out.append('\n')
                count = 0
            }
        }
        if (count > 0) out.append('\n')
    }

    private fun selectRandom(codes: CharArray, cumProbs: DoubleArray): Char {
        val r = Random.nextDouble()
        for (i in cumProbs.indices) {
            if (r < cumProbs[i]) return codes[i]
        }
        return codes.last()
    }
}