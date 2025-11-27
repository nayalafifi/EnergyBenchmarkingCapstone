package com.example.benchlib

import android.os.Trace
import android.util.Log
import java.util.concurrent.atomic.AtomicInteger
import kotlin.math.max
import kotlin.math.min

class FannkuchReduxBenchmarkKotlin : Runnable {
    private lateinit var p: IntArray
    private lateinit var pp: IntArray
    private lateinit var count: IntArray

    private fun firstPermutation(idxParam: Int) {
        var idx = idxParam
        for (i in p.indices) {
            p[i] = i
        }

        for (i in count.size - 1 downTo 1) {
            val d = idx / Fact[i]
            count[i] = d
            idx %= Fact[i]

            System.arraycopy(p, 0, pp, 0, i + 1)
            for (j in 0..i) {
                p[j] = if (j + d <= i) pp[j + d] else pp[j + d - i - 1]
            }
        }
    }

    private fun nextPermutation() {
        var first = p[1]
        p[1] = p[0]
        p[0] = first

        var i = 1
        while (++count[i] > i) {
            count[i++] = 0
            val next = p[1]
            p[0] = next
            for (j in 1 until i) {
                p[j] = p[j + 1]
            }
            p[i] = first
            first = next
        }
    }

    private fun countFlips(): Int {
        var flips = 1
        var first = p[0]
        if (p[first] != 0) {
            System.arraycopy(p, 0, pp, 0, pp.size)
            do {
                ++flips
                var lo = 1
                var hi = first - 1
                while (lo < hi) {
                    val t = pp[lo]
                    pp[lo] = pp[hi]
                    pp[hi] = t
                    ++lo
                    --hi
                }
                val t = pp[first]
                pp[first] = first
                first = t
            } while (pp[first] != 0)
        }
        return flips
    }

    private fun runTask(task: Int) {
        val idxMin = task * CHUNKSZ
        val idxMax = min(Fact[n], idxMin + CHUNKSZ)

        firstPermutation(idxMin)

        var maxflips = 1
        var chksum = 0
        var i = idxMin
        while (true) {
            if (p[0] != 0) {
                val flips = countFlips()
                maxflips = max(maxflips, flips)
                chksum += if (i % 2 == 0) flips else -flips
            }

            if (++i == idxMax) {
                break
            }

            nextPermutation()
        }
        maxFlips[task] = maxflips
        chkSums[task] = chksum
    }

    override fun run() {
        p = IntArray(n)
        pp = IntArray(n)
        count = IntArray(n)

        var task: Int
        while (taskId.getAndIncrement().also { task = it } < NTASKS) {
            runTask(task)
        }
    }

    companion object {
        private const val NCHUNKS = 150
        private var CHUNKSZ = 0
        private var NTASKS = 0
        private var n = 0
        private lateinit var Fact: IntArray
        private lateinit var maxFlips: IntArray
        private lateinit var chkSums: IntArray
        private lateinit var taskId: AtomicInteger

        @JvmStatic
        fun runBenchmark(): String {
            Trace.beginSection("FannkuchRedux Benchmark")

            val startTime = System.currentTimeMillis()
            val iterations = 60

            try {
                for (iteration in 0 until iterations) {
                    n = 10 // Input parameter - matches reference (can be 1-12)

                    // Compute factorials
                    Fact = IntArray(n + 1)
                    Fact[0] = 1
                    for (i in 1 until Fact.size) {
                        Fact[i] = Fact[i - 1] * i
                    }

                    // Calculate chunking parameters
                    CHUNKSZ = (Fact[n] + NCHUNKS - 1) / NCHUNKS
                    NTASKS = (Fact[n] + CHUNKSZ - 1) / CHUNKSZ
                    maxFlips = IntArray(NTASKS)
                    chkSums = IntArray(NTASKS)
                    taskId = AtomicInteger(0)

                    // Create and start worker threads
                    val nthreads = Runtime.getRuntime().availableProcessors()
                    val threads = Array(nthreads) {
                        Thread(FannkuchReduxBenchmarkKotlin()).apply { start() }
                    }

                    // Wait for all threads to complete
                    for (t in threads) {
                        try {
                            t.join()
                        } catch (e: InterruptedException) {
                            Log.e("BENCHMARK", "Thread interrupted", e)
                        }
                    }

                    // Aggregate results
                    var res = 0
                    for (v in maxFlips) {
                        res = max(res, v)
                    }
                    var chk = 0
                    for (v in chkSums) {
                        chk += v
                    }

                    // Optionally log intermediate results (commented out for performance)
                    // Log.d("BENCHMARK", "Iteration $iteration: chk=$chk, max=$res")
                }

                val duration = System.currentTimeMillis() - startTime
                val result = "FannkuchRedux Kotlin completed: ${duration}ms ($iterations iterations)"
                Log.d("BENCHMARK", result)

                return result
            } finally {
                Trace.endSection()
            }
        }
    }
}