package com.example.myapplication

import android.os.Trace
import android.util.Log
import java.text.DecimalFormat

object SpectralNormBenchmarkKotlin {

    private val formatter = DecimalFormat("#.000000000")
    private val NCPU = Runtime.getRuntime().availableProcessors()

    fun runBenchmark(): String {
        Trace.beginSection("SpectralNorm Benchmark")

        val startTime = System.currentTimeMillis()

        return try {
            val n = 100

            // Run 5000 iterations to match Java
            for (iteration in 0 until 400) {
                val u = DoubleArray(n) { 1.0 }
                val v = DoubleArray(n)

                repeat(10) {
                    aTimesTransp(v, u, n)
                    aTimesTransp(u, v, n)
                }
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "SpectralNorm Kotlin duration: ${duration}ms")

            "SpectralNorm benchmark completed: ${duration}ms"
        } catch (e: Exception) {
            "SpectralNorm Benchmark failed: ${e.message}"
        } finally {
            Trace.endSection()
        }
    }

    private fun aTimesTransp(v: DoubleArray, u: DoubleArray, n: Int) {
        val x = DoubleArray(n)
        val threads = Array(NCPU) { i ->
            val start = i * n / NCPU
            val end = (i + 1) * n / NCPU
            Times(x, start, end, u, false, n)
        }
        threads.forEach { it.start() }
        threads.forEach { it.join() }

        for (i in 0 until NCPU) {
            val start = i * n / NCPU
            val end = (i + 1) * n / NCPU
            threads[i] = Times(v, start, end, x, true, n)
        }
        threads.forEach { it.start() }
        threads.forEach { it.join() }
    }

    private class Times(
        private val v: DoubleArray,
        private val start: Int,
        private val end: Int,
        private val u: DoubleArray,
        private val transpose: Boolean,
        private val size: Int
    ) : Thread() {

        override fun run() {
            for (i in start until end) {
                var sum = 0.0
                for (j in 0 until size) {
                    sum += u[j] / a(if (transpose) j else i, if (transpose) i else j)
                }
                v[i] = sum
            }
        }

        private fun a(i: Int, j: Int): Int {
            return (i + j) * (i + j + 1) / 2 + i + 1
        }
    }
}