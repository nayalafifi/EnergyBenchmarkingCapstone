package com.example.myapplication

import android.os.Trace
import android.util.Log

object RevCompBenchmarkKotlin {

    private val map = ByteArray(128)

    init {
        val from = "ACBDGHK\nMNSRUTWVYacbdghkmnsrutwvy"
        val to = "TGVHCDM\nKNSYAAWBRTGVHCDMKNSYAAWBR"

        for (i in from.indices) {
            map[from[i].code] = to[i].code.toByte()
        }
    }

    fun runBenchmark(): String {
        Trace.beginSection("RevComp Benchmark")

        val startTime = System.currentTimeMillis()

        return try {
            // Generate larger input - 1000 sequences
            val inputBuilder = StringBuilder()
            for (seq in 0 until 1000) {
                inputBuilder.append(">SEQ$seq\n")
                for (line in 0 until 100) {
                    inputBuilder.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n")
                }
            }
            val input = inputBuilder.toString()

            // Run 1100 iterations to match Java
            for (iteration in 0 until 1100) {
                val buf = input.toByteArray()

                // Map complement
                for (i in buf.indices) {
                    val b = buf[i].toInt()
                    if (b != '\n'.code && b != '>'.code && b in map.indices) {
                        buf[i] = map[b]
                    }
                }

                // Reverse each sequence
                var seqStart = 0
                for (i in buf.indices) {
                    if (buf[i] == '>'.code.toByte()) {
                        if (i > seqStart) {
                            reverseSection(buf, seqStart, i - 1)
                        }
                        var j = i
                        while (j < buf.size && buf[j] != '\n'.code.toByte()) j++
                        seqStart = j + 1
                    }
                }
                if (seqStart < buf.size) {
                    reverseSection(buf, seqStart, buf.size - 1)
                }
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "RevComp Kotlin duration: ${duration}ms")

            "RevComp benchmark completed: ${duration}ms"
        } finally {
            Trace.endSection()
        }
    }

    private fun reverseSection(buf: ByteArray, start: Int, end: Int) {
        var s = start
        var e = end
        while (s < e) {
            if (buf[s] == '\n'.code.toByte()) {
                s++
                continue
            }
            if (buf[e] == '\n'.code.toByte()) {
                e--
                continue
            }
            val temp = buf[s]
            buf[s] = buf[e]
            buf[e] = temp
            s++
            e--
        }
    }
}