package com.example.myapplication

import android.os.Trace
import android.util.Log
import kotlin.concurrent.thread
import java.util.concurrent.atomic.AtomicInteger

object MandelbrotBenchmarkKotlin {
    fun runBenchmark(): String {
        Trace.beginSection("Mandelbrot Benchmark")

        val startTime = System.currentTimeMillis()

        try {
            val N = 6000

            // Run 100 iterations to match Java
            for (iteration in 0 until 15) {
                val out = Array(N) { ByteArray((N + 7) / 8) }
                val yCt = AtomicInteger(0)

                val Crb = DoubleArray(N + 7)
                val Cib = DoubleArray(N + 7)
                val invN = 2.0 / N
                for (i in 0 until N) {
                    Cib[i] = i * invN - 1.0
                    Crb[i] = i * invN - 1.5
                }

                val poolSize = Runtime.getRuntime().availableProcessors() * 2
                val pool = Array(poolSize) {
                    thread {
                        while (true) {
                            val y = yCt.getAndIncrement()
                            if (y >= out.size) break
                            val line = ByteArray((N + 7) / 8)
                            for (xb in line.indices) {
                                line[xb] = getByte(xb * 8, y, Crb, Cib).toByte()
                            }
                            out[y] = line
                        }
                    }
                }
                pool.forEach { it.join() }
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "Mandelbrot Kotlin duration: ${duration}ms")

            return "Mandelbrot benchmark completed: ${duration}ms"
        } catch (e: InterruptedException) {
            return "Benchmark interrupted."
        } finally {
            Trace.endSection()
        }
    }

    private fun getByte(x: Int, y: Int, Crb: DoubleArray, Cib: DoubleArray): Int {
        var res = 0
        for (i in 0 until 8 step 2) {
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
                    b = b or 0x2
                    if (b == 0x3) break
                }
                if (Zr2 * Zr2 + Zi2 * Zi2 > 4) {
                    b = b or 0x1
                    if (b == 0x3) break
                }
            } while (--j > 0)
            res = (res shl 2) + b
        }
        return res.inv()
    }
}