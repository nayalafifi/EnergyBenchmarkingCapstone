#include <jni.h>
#include <cmath>
#include <time.h>
#include <android/log.h>

constexpr double PI = 3.141592653589793;
constexpr double SOLAR_MASS = 4 * PI * PI;
constexpr double DAYS_PER_YEAR = 365.24;

struct Body {
    double x, y, z;
    double vx, vy, vz;
    double mass;
};

static void advance(Body* bodies, int nbodies, double dt) {
    // Phase 1: Calculate all position differences and distances
    constexpr int N = 10; // (5 * 4) / 2 = 10 pairs
    double r[N][3];
    double mag[N];

    int idx = 0;
    for (int i = 0; i < nbodies; i++) {
        for (int j = i + 1; j < nbodies; j++) {
            r[idx][0] = bodies[i].x - bodies[j].x;
            r[idx][1] = bodies[i].y - bodies[j].y;
            r[idx][2] = bodies[i].z - bodies[j].z;
            idx++;
        }
    }

    // Phase 2: Calculate magnitudes
    // Note: Without SIMD, we use regular sqrt, but match the structure
    for (int i = 0; i < N; i++) {
        double dsquared = r[i][0] * r[i][0] + r[i][1] * r[i][1] + r[i][2] * r[i][2];
        double distance = std::sqrt(dsquared);
        mag[i] = dt / (dsquared * distance);
    }

    // Phase 3: Update velocities
    idx = 0;
    for (int i = 0; i < nbodies; i++) {
        for (int j = i + 1; j < nbodies; j++) {
            double xmag = r[idx][0] * mag[idx];
            double ymag = r[idx][1] * mag[idx];
            double zmag = r[idx][2] * mag[idx];

            bodies[i].vx -= xmag * bodies[j].mass;
            bodies[i].vy -= ymag * bodies[j].mass;
            bodies[i].vz -= zmag * bodies[j].mass;

            bodies[j].vx += xmag * bodies[i].mass;
            bodies[j].vy += ymag * bodies[i].mass;
            bodies[j].vz += zmag * bodies[i].mass;

            idx++;
        }
    }

    // Phase 4: Update positions
    for (int i = 0; i < nbodies; i++) {
        bodies[i].x += dt * bodies[i].vx;
        bodies[i].y += dt * bodies[i].vy;
        bodies[i].z += dt * bodies[i].vz;
    }
}

static double energy(Body* bodies, int nbodies) {
    double e = 0.0;

    for (int i = 0; i < nbodies; i++) {
        Body& b = bodies[i];
        e += 0.5 * b.mass * (b.vx * b.vx + b.vy * b.vy + b.vz * b.vz);

        for (int j = i + 1; j < nbodies; j++) {
            Body& b2 = bodies[j];
            double dx = b.x - b2.x;
            double dy = b.y - b2.y;
            double dz = b.z - b2.z;
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            e -= (b.mass * b2.mass) / distance;
        }
    }
    return e;
}

static void offset_momentum(Body* bodies, int nbodies) {
    double px = 0.0, py = 0.0, pz = 0.0;

    for (int i = 0; i < nbodies; i++) {
        px += bodies[i].vx * bodies[i].mass;
        py += bodies[i].vy * bodies[i].mass;
        pz += bodies[i].vz * bodies[i].mass;
    }

    bodies[0].vx = -px / SOLAR_MASS;
    bodies[0].vy = -py / SOLAR_MASS;
    bodies[0].vz = -pz / SOLAR_MASS;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_benchlib_NativeBenchmarks_runNBodyBenchmarkCpp(
        JNIEnv *env,
        jclass clazz,
        jint n) {

    clock_t start = clock();
    int iterations = 1;  // FIXED: Only run once like TCLBG

    Body bodies[5] = {
            {0, 0, 0, 0, 0, 0, SOLAR_MASS},
            {4.84143144246472090e+00, -1.16032004402742839e+00, -1.03622044471123109e-01,
                      1.66007664274403694e-03 * DAYS_PER_YEAR, 7.69901118419740425e-03 * DAYS_PER_YEAR,
                            -6.90460016972063023e-05 * DAYS_PER_YEAR, 9.54791938424326609e-04 * SOLAR_MASS},
            {8.34336671824457987e+00, 4.12479856412430479e+00, -4.03523417114321381e-01,
                      -2.76742510726862411e-03 * DAYS_PER_YEAR, 4.99852801234917238e-03 * DAYS_PER_YEAR,
                            2.30417297573763929e-05 * DAYS_PER_YEAR, 2.85885980666130812e-04 * SOLAR_MASS},
            {1.28943695621391310e+01, -1.51111514016986312e+01, -2.23307578892655734e-01,
                      2.96460137564761618e-03 * DAYS_PER_YEAR, 2.37847173959480950e-03 * DAYS_PER_YEAR,
                            -2.96589568540237556e-05 * DAYS_PER_YEAR, 4.36624404335156298e-05 * SOLAR_MASS},
            {1.53796971148509165e+01, -2.59193146099879641e+01, 1.79258772950371181e-01,
                      2.68067772490389322e-03 * DAYS_PER_YEAR, 1.62824170038242295e-03 * DAYS_PER_YEAR,
                            -9.51592254519715870e-05 * DAYS_PER_YEAR, 5.15138902046611451e-05 * SOLAR_MASS}
    };

    for (int iter = 0; iter < iterations; iter++) {
        // FIXED: Call offset_momentum only once at start
        offset_momentum(bodies, 5);
        energy(bodies, 5);

        for (int i = 0; i < n; i++) {
            advance(bodies, 5, 0.01);
        }

        energy(bodies, 5);
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK",
                        "NBody C++ completed: %ldms (%d iterations)",
                        duration, iterations);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "NBody C++ completed: %ldms (%d iterations)",
             duration, iterations);
    return env->NewStringUTF(buffer);
}