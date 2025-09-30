package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

public final class NBodyBenchmark {

    public static String runBenchmark() {
        Trace.beginSection("NBody Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            int n = 500_000;

            // Start with 100 iterations as baseline
            for (int iteration = 0; iteration < 143; iteration++) {
                NBodySystem bodies = new NBodySystem();
                for (int i = 0; i < n; ++i)
                    bodies.advance(0.01);
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "NBody Java duration: " + duration + "ms");

            return "NBody benchmark completed: " + duration + "ms";
        } catch (Exception e) {
            return "NBody Benchmark failed: " + e.getMessage();
        } finally {
            Trace.endSection();
        }
    }

    static class NBodySystem {
        private static final double PI = 3.141592653589793;
        private static final double SOLAR_MASS = 4 * PI * PI;
        private static final double DAYS_PER_YEAR = 365.24;
        private static final int BODY_SIZE = 8;
        private static final int BODY_COUNT = 5;

        private static final int x = 0, y = 1, z = 2, vx = 3, vy = 4, vz = 5, mass = 6;

        private final double[] bodies = {
                0, 0, 0, 0, 0, 0, SOLAR_MASS, 0,
                4.84143144246472090e+00, -1.16032004402742839e+00, -1.03622044471123109e-01,
                1.66007664274403694e-03 * DAYS_PER_YEAR, 7.69901118419740425e-03 * DAYS_PER_YEAR,
                -6.90460016972063023e-05 * DAYS_PER_YEAR, 9.54791938424326609e-04 * SOLAR_MASS, 0,
                8.34336671824457987e+00, 4.12479856412430479e+00, -4.03523417114321381e-01,
                -2.76742510726862411e-03 * DAYS_PER_YEAR, 4.99852801234917238e-03 * DAYS_PER_YEAR,
                2.30417297573763929e-05 * DAYS_PER_YEAR, 2.85885980666130812e-04 * SOLAR_MASS, 0,
                1.28943695621391310e+01, -1.51111514016986312e+01, -2.23307578892655734e-01,
                2.96460137564761618e-03 * DAYS_PER_YEAR, 2.37847173959480950e-03 * DAYS_PER_YEAR,
                -2.96589568540237556e-05 * DAYS_PER_YEAR, 4.36624404335156298e-05 * SOLAR_MASS, 0,
                1.53796971148509165e+01, -2.59193146099879641e+01, 1.79258772950371181e-01,
                2.68067772490389322e-03 * DAYS_PER_YEAR, 1.62824170038242295e-03 * DAYS_PER_YEAR,
                -9.51592254519715870e-05 * DAYS_PER_YEAR, 5.15138902046611451e-05 * SOLAR_MASS, 0
        };

        public NBodySystem() {
            double px = 0, py = 0, pz = 0;
            for (int i = 0; i < BODY_COUNT; ++i) {
                int offset = i * BODY_SIZE;
                double m = bodies[offset + mass];
                px += bodies[offset + vx] * m;
                py += bodies[offset + vy] * m;
                pz += bodies[offset + vz] * m;
            }
            bodies[vx] = -px / SOLAR_MASS;
            bodies[vy] = -py / SOLAR_MASS;
            bodies[vz] = -pz / SOLAR_MASS;
        }

        public void advance(double dt) {
            for (int i = 0; i < BODY_COUNT; ++i) {
                int io = i * BODY_SIZE;
                for (int j = i + 1; j < BODY_COUNT; ++j) {
                    int jo = j * BODY_SIZE;
                    double dx = bodies[io + x] - bodies[jo + x];
                    double dy = bodies[io + y] - bodies[jo + y];
                    double dz = bodies[io + z] - bodies[jo + z];

                    double d2 = dx * dx + dy * dy + dz * dz;
                    double dist = Math.sqrt(d2);
                    double mag = dt / (d2 * dist);

                    double m1 = bodies[io + mass];
                    double m2 = bodies[jo + mass];

                    bodies[io + vx] -= dx * m2 * mag;
                    bodies[io + vy] -= dy * m2 * mag;
                    bodies[io + vz] -= dz * m2 * mag;

                    bodies[jo + vx] += dx * m1 * mag;
                    bodies[jo + vy] += dy * m1 * mag;
                    bodies[jo + vz] += dz * m1 * mag;
                }
            }

            for (int i = 0; i < BODY_COUNT; ++i) {
                int o = i * BODY_SIZE;
                bodies[o + x] += dt * bodies[o + vx];
                bodies[o + y] += dt * bodies[o + vy];
                bodies[o + z] += dt * bodies[o + vz];
            }
        }

        public double energy() {
            double e = 0.0;
            for (int i = 0; i < BODY_COUNT; ++i) {
                int io = i * BODY_SIZE;
                double velX = bodies[io + vx];
                double velY = bodies[io + vy];
                double velZ = bodies[io + vz];
                double m = bodies[io + mass];
                e += 0.5 * m * (velX * velX + velY * velY + velZ * velZ);

                for (int j = i + 1; j < BODY_COUNT; ++j) {
                    int jo = j * BODY_SIZE;
                    double dx = bodies[io + x] - bodies[jo + x];
                    double dy = bodies[io + y] - bodies[jo + y];
                    double dz = bodies[io + z] - bodies[jo + z];
                    double dist = Math.sqrt(dx * dx + dy * dy + dz * dz);
                    e -= (m * bodies[jo + mass]) / dist;
                }
            }
            return e;
        }
    }
}
