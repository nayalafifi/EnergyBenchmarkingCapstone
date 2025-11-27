package com.example.benchlib

import android.os.Trace
import android.util.Log
import java.util.concurrent.atomic.AtomicInteger

object MandelbrotBenchmarkKotlin {
    private lateinit var out: Array<ByteArray>
    private lateinit var yCt: AtomicInteger
    private lateinit var Crb: DoubleArray
    private lateinit var Cib: DoubleArray

    private fun getByte(x: Int, y: Int): Int {
        var res = 0
        var i = 0
        while (i < 8) {
            var Zr1 = Crb[x + i]
            var Zi1 = Cib[y]

            var Zr2 = Crb[x + i + 1]
            var Zi2 = Cib[y]

            var b = 0
            var j = 49
            do {
                val nZr1 = Zr1 * Zr1 - Zi1 * Zi1 + Crb[x + i]
                val nZi1 = Zr1 * Zi1 + Zr1 * Zi1 + Cib[y]
                Zr1 = nZr1
                Zi1 = nZi1

                val nZr2 = Zr2 * Zr2 - Zi2 * Zi2 + Crb[x + i + 1]
                val nZi2 = Zr2 * Zi2 + Zr2 * Zi2 + Cib[y]
                Zr2 = nZr2
                Zi2 = nZi2

                if (Zr1 * Zr1 + Zi1 * Zi1 > 4) {
                    b = b or 2
                    if (b == 3) break
                }
                if (Zr2 * Zr2 + Zi2 * Zi2 > 4) {
                    b = b or 1
                    if (b == 3) break
                }
            } while (--j > 0)
            res = (res shl 2) + b
            i += 2
        }
        return res.inv()
    }

    private fun putLine(y: Int, line: ByteArray) {
        for (xb in line.indices)
            line[xb] = getByte(xb * 8, y).toByte()
    }

    @JvmStatic
    fun runBenchmark(): String {
        Trace.beginSection("Mandelbrot Benchmark")

        val startTime = System.currentTimeMillis()

        try {
            val N = 6000
            val iterations = 1

            for (it in 0 until iterations) {
                // Initialize arrays - matches reference
                Crb = DoubleArray(N + 7)
                Cib = DoubleArray(N + 7)
                val invN = 2.0 / N
                for (i in 0 until N) {
                    Cib[i] = i * invN - 1.0
                    Crb[i] = i * invN - 1.5
                }
                yCt = AtomicInteger()
                out = Array(N) { ByteArray((N + 7) / 8) }

                // Create thread pool - matches reference
                val nThreads = 2 * Runtime.getRuntime().availableProcessors()
                val pool = Array(nThreads) {
                    object : Thread() {
                        override fun run() {
                            var y: Int
                            while (yCt.getAndIncrement().also { y = it } < out.size)
                                putLine(y, out[y])
                        }
                    }
                }
                for (t in pool) t.start()
                for (t in pool) t.join()
            }

            val duration = System.currentTimeMillis() - startTime
            val result = "Mandelbrot Kotlin completed: ${duration}ms ($iterations iterations)"
            Log.d("BENCHMARK", result)

            return result
        } catch (e: InterruptedException) {
            Log.e("BENCHMARK", "Benchmark interrupted", e)
            return "Benchmark interrupted."
        } finally {
            Trace.endSection()
        }
    }
}