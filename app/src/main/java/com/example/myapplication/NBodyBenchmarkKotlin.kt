package com.example.myapplication

import android.os.Trace
import android.util.Log

object NBodyBenchmarkKotlin {

    fun runBenchmark(): String {
        Trace.beginSection("NBody Benchmark")

        val startTime = System.currentTimeMillis()

        return try {
            val n = 500_000

            // Run 143 iterations to match Java
            for (iteration in 0 until 143) {
                val bodies = NBodySystem()
                for (i in 0 until n) bodies.advance(0.01)
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "NBody Kotlin duration: ${duration}ms")

            "NBody benchmark completed: ${duration}ms"
        } catch (e: Exception) {
            "NBody Benchmark failed: ${e.message}"
        } finally {
            Trace.endSection()
        }
    }

    class NBodySystem {
        companion object {
            private const val PI = 3.141592653589793
            private const val SOLAR_MASS = 4 * PI * PI
            private const val DAYS_PER_YEAR = 365.24
            private const val BODY_SIZE = 8
            private const val BODY_COUNT = 5

            private const val x = 0
            private const val y = 1
            private const val z = 2
            private const val vx = 3
            private const val vy = 4
            private const val vz = 5
            private const val mass = 6
        }

        private val bodies: DoubleArray = doubleArrayOf(
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SOLAR_MASS, 0.0,
            4.84143144246472090e+00, -1.16032004402742839e+00, -1.03622044471123109e-01,
            1.66007664274403694e-03 * DAYS_PER_YEAR, 7.69901118419740425e-03 * DAYS_PER_YEAR,
            -6.90460016972063023e-05 * DAYS_PER_YEAR, 9.54791938424326609e-04 * SOLAR_MASS, 0.0,
            8.34336671824457987e+00, 4.12479856412430479e+00, -4.03523417114321381e-01,
            -2.76742510726862411e-03 * DAYS_PER_YEAR, 4.99852801234917238e-03 * DAYS_PER_YEAR,
            2.30417297573763929e-05 * DAYS_PER_YEAR, 2.85885980666130812e-04 * SOLAR_MASS, 0.0,
            1.28943695621391310e+01, -1.51111514016986312e+01, -2.23307578892655734e-01,
            2.96460137564761618e-03 * DAYS_PER_YEAR, 2.37847173959480950e-03 * DAYS_PER_YEAR,
            -2.96589568540237556e-05 * DAYS_PER_YEAR, 4.36624404335156298e-05 * SOLAR_MASS, 0.0,
            1.53796971148509165e+01, -2.59193146099879641e+01, 1.79258772950371181e-01,
            2.68067772490389322e-03 * DAYS_PER_YEAR, 1.62824170038242295e-03 * DAYS_PER_YEAR,
            -9.51592254519715870e-05 * DAYS_PER_YEAR, 5.15138902046611451e-05 * SOLAR_MASS, 0.0
        )

        init {
            var px = 0.0
            var py = 0.0
            var pz = 0.0
            for (i in 0 until BODY_COUNT) {
                val offset = i * BODY_SIZE
                val m = bodies[offset + mass]
                px += bodies[offset + vx] * m
                py += bodies[offset + vy] * m
                pz += bodies[offset + vz] * m
            }
            bodies[vx] = -px / SOLAR_MASS
            bodies[vy] = -py / SOLAR_MASS
            bodies[vz] = -pz / SOLAR_MASS
        }

        fun advance(dt: Double) {
            for (i in 0 until BODY_COUNT) {
                val io = i * BODY_SIZE
                for (j in i + 1 until BODY_COUNT) {
                    val jo = j * BODY_SIZE
                    val dx = bodies[io + x] - bodies[jo + x]
                    val dy = bodies[io + y] - bodies[jo + y]
                    val dz = bodies[io + z] - bodies[jo + z]

                    val d2 = dx * dx + dy * dy + dz * dz
                    val dist = Math.sqrt(d2)
                    val mag = dt / (d2 * dist)

                    val m1 = bodies[io + mass]
                    val m2 = bodies[jo + mass]

                    bodies[io + vx] -= dx * m2 * mag
                    bodies[io + vy] -= dy * m2 * mag
                    bodies[io + vz] -= dz * m2 * mag

                    bodies[jo + vx] += dx * m1 * mag
                    bodies[jo + vy] += dy * m1 * mag
                    bodies[jo + vz] += dz * m1 * mag
                }
            }

            for (i in 0 until BODY_COUNT) {
                val o = i * BODY_SIZE
                bodies[o + x] += dt * bodies[o + vx]
                bodies[o + y] += dt * bodies[o + vy]
                bodies[o + z] += dt * bodies[o + vz]
            }
        }

        fun energy(): Double {
            var e = 0.0
            for (i in 0 until BODY_COUNT) {
                val io = i * BODY_SIZE
                val velX = bodies[io + vx]
                val velY = bodies[io + vy]
                val velZ = bodies[io + vz]
                val m = bodies[io + mass]
                e += 0.5 * m * (velX * velX + velY * velY + velZ * velZ)

                for (j in i + 1 until BODY_COUNT) {
                    val jo = j * BODY_SIZE
                    val dx = bodies[io + x] - bodies[jo + x]
                    val dy = bodies[io + y] - bodies[jo + y]
                    val dz = bodies[io + z] - bodies[jo + z]
                    val dist = Math.sqrt(dx * dx + dy * dy + dz * dz)
                    e -= (m * bodies[jo + mass]) / dist
                }
            }
            return e
        }
    }
}