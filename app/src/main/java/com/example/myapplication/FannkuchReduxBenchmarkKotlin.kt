package com.example.myapplication

import android.os.Trace
import android.util.Log

object FannkuchReduxBenchmarkKotlin {

    fun runBenchmark(): String {
        Trace.beginSection("FannkuchRedux Benchmark")

        val startTime = System.currentTimeMillis()

        try {
            val n = 10
            val output = StringBuilder()

            // Run 20 times to extend duration to 30-60 seconds
            for (iteration in 0 until 80) {
                val perm = IntArray(n)
                val perm1 = IntArray(n) { it }
                val count = IntArray(n)
                var maxFlips = 0
                var r = n
                var checkSum = 0

                outerLoop@ while (true) {
                    // Copy perm1 to perm
                    for (i in 0 until n) perm[i] = perm1[i]
                    var flips = 0
                    var k: Int

                    while (true) {
                        k = perm[0]
                        if (k == 0) break
                        // Flip 0..k
                        var i = 0
                        var j = k
                        while (i < j) {
                            val t = perm[i]
                            perm[i] = perm[j]
                            perm[j] = t
                            i++; j--
                        }
                        flips++
                    }

                    checkSum += if ((perm1[0] and 1) == 0) flips else -flips
                    if (flips > maxFlips) maxFlips = flips

                    // Prepare for next permutation
                    while (r != 1) {
                        count[r - 1] = r
                        r--
                    }

                    while (true) {
                        if (r == n) {
                            output.append("Checksum: $checkSum\n")
                            output.append("Maximum flips: $maxFlips\n")
                            break@outerLoop
                        }

                        val perm0 = perm1[0]
                        for (i in 0 until r) perm1[i] = perm1[i + 1]
                        perm1[r] = perm0
                        count[r]--
                        if (count[r] > 0) break
                        r++
                    }
                }
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "FannkuchRedux Kotlin duration: ${duration}ms")

            return output.toString()
        } finally {
            Trace.endSection()
        }
    }
}