package com.example.benchlib

import android.os.Trace
import android.util.Log
import java.io.ByteArrayInputStream
import java.io.IOException
import java.io.InputStream
import java.util.Collections
import java.util.LinkedList
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.TimeUnit

object RevCompBenchmarkKotlin {
    private val map = ByteArray(256)
    private const val CHUNK_SIZE = 1024 * 1024 * 16
    private val NUMBER_OF_CORES = Runtime.getRuntime().availableProcessors()

    init {
        for (i in map.indices) {
            map[i] = i.toByte()
        }
        map['T'.code] = 'A'.code.toByte()
        map['t'.code] = map['T'.code]
        map['A'.code] = 'T'.code.toByte()
        map['a'.code] = map['A'.code]
        map['G'.code] = 'C'.code.toByte()
        map['g'.code] = map['G'.code]
        map['C'.code] = 'G'.code.toByte()
        map['c'.code] = map['C'.code]
        map['V'.code] = 'B'.code.toByte()
        map['v'.code] = map['V'.code]
        map['H'.code] = 'D'.code.toByte()
        map['h'.code] = map['H'.code]
        map['R'.code] = 'Y'.code.toByte()
        map['r'.code] = map['R'.code]
        map['M'.code] = 'K'.code.toByte()
        map['m'.code] = map['M'.code]
        map['Y'.code] = 'R'.code.toByte()
        map['y'.code] = map['Y'.code]
        map['K'.code] = 'M'.code.toByte()
        map['k'.code] = map['K'.code]
        map['B'.code] = 'V'.code.toByte()
        map['b'.code] = map['B'.code]
        map['D'.code] = 'H'.code.toByte()
        map['d'.code] = map['D'.code]
        map['U'.code] = 'A'.code.toByte()
        map['u'.code] = map['U'.code]
    }

    fun runBenchmark(): String {
        Trace.beginSection("RevComp Benchmark")

        val startTime = System.currentTimeMillis()

        try {
            // Generate test input
            val inputBuilder = StringBuilder()
            for (seq in 0..999) {
                inputBuilder.append(">SEQ").append(seq).append("\n")
                for (line in 0..99) {
                    inputBuilder.append("ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT\n")
                }
            }
            val testInput = inputBuilder.toString().toByteArray()

            val iterations = 1
            for (iteration in 0..<iterations) {
                processInput(ByteArrayInputStream(testInput))
            }

            val duration = System.currentTimeMillis() - startTime
            val result = "RevComp completed: " + duration + "ms (" + iterations + " iterations)"
            Log.d("BENCHMARK", result)

            return result
        } catch (e: Exception) {
            Log.e("BENCHMARK", "RevComp failed", e)
            return "RevComp failed: " + e.message
        } finally {
            Trace.endSection()
        }
    }

    @Throws(IOException::class, InterruptedException::class)
    private fun processInput(input: InputStream) {
        val service = Executors.newFixedThreadPool(NUMBER_OF_CORES)
        val list = Collections.synchronizedList(ArrayList<ByteArray>())

        var read: Int
        var buffer: ByteArray
        var lastFinder: Finder? = null

        do {
            buffer = ByteArray(CHUNK_SIZE)
            read = input.read(buffer)
            list.add(buffer)

            val finder = Finder(buffer, read, lastFinder, list, service)
            service.execute(finder)
            lastFinder = finder
        } while (read == CHUNK_SIZE)

        val status = lastFinder!!.finish()
        val mapper = Mapper(
            status!!.lastFinding, status.count - 1, status.lastMapper, list
        )
        service.execute(mapper)

        service.shutdown()
        service.awaitTermination(60, TimeUnit.SECONDS)
    }

    private fun find(list: List<ByteArray>, start: Int, end: Int): IntArray {
        var n = 0
        var lp = 0
        val result = IntArray(4)
        var foundStart = false

        for (bytes in list) {
            if (!foundStart && n + bytes.size > start) {
                result[0] = lp
                result[1] = start - n
                foundStart = true
            }
            if (foundStart && n + bytes.size > end) {
                result[2] = lp
                result[3] = end - n
                break
            }
            n += bytes.size
            lp++
        }
        return result
    }

    private class Status {
        var count: Int = 0
        var lastFinding: Int = 0
        var lastMapper: Mapper? = null
    }

    private class Finder(
        private val a: ByteArray,
        private val size: Int,
        private val previous: Finder?,
        private val list: List<ByteArray>,
        private val service: ExecutorService
    ) :
        Runnable {
        private var status: Status? = null
        private var done = false

        fun finish(): Status? {
            while (!done) try {
                Thread.sleep(1)
            } catch (e: InterruptedException) {
                // ignored
            }
            return status
        }

        override fun run() {
            val findings = LinkedList<Int>()

            for (i in 0..<size) {
                if (a[i] == '>'.code.toByte()) {
                    findings.add(i)
                }
            }

            if (previous == null) {
                status = Status()
            } else {
                status = previous.finish()
                findings.add(0, status!!.lastFinding)
                for (i in 1..<findings.size) {
                    findings[i] = findings[i] + status!!.count
                }
            }

            if (findings.size > 1) for (i in 0..<findings.size - 1) {
                status!!.lastMapper = Mapper(
                    findings[i], findings[i + 1] - 1,
                    status!!.lastMapper, list
                )
                service.execute(status!!.lastMapper)
            }

            status!!.lastFinding = findings[findings.size - 1]
            status!!.count += size
            done = true
        }
    }

    private class Mapper(
        private val start: Int,
        private val end: Int,
        private val previous: Mapper?,
        private val list: List<ByteArray>
    ) :
        Runnable {
        private var done = false

        fun finish() {
            while (!done) try {
                Thread.sleep(1)
            } catch (e: InterruptedException) {
                // ignored
            }
        }

        override fun run() {
            val positions = find(list, start, end)

            var lp1 = positions[0]
            var tob = list[lp1]

            var lp2 = positions[2]
            var bot = list[lp2]

            var p1 = positions[1]
            while (tob[p1] != '\n'.code.toByte()) p1++

            var p2 = positions[3]

            while (lp1 < lp2 || p1 < p2) {
                if (tob[p1] == '\n'.code.toByte()) {
                    p1++
                } else if (bot[p2] == '\n'.code.toByte()) {
                    p2--
                } else {
                    val tmp = tob[p1]
                    tob[p1] = map[bot[p2].toInt()]
                    bot[p2] = map[tmp.toInt()]
                    p1++
                    p2--
                }
                if (p1 == tob.size) {
                    lp1++
                    tob = list[lp1]
                    p1 = 0
                }
                if (p2 == -1) {
                    lp2--
                    bot = list[lp2]
                    p2 = bot.size - 1
                }
            }

            previous?.finish()

            done = true
        }
    }
}