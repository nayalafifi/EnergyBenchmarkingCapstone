#include "NBodyBenchmarkC.h"
#include <cmath>
#include <string>
#include <sstream>
#include <android/log.h>
#include <ctime>

static constexpr double PI = 3.141592653589793;
static constexpr double SOLAR_MASS = 4 * PI * PI;
static constexpr double DAYS_PER_YEAR = 365.24;

struct Body {
    double x, y, z;
    double vx, vy, vz;
    double mass;
};

static constexpr size_t N_BODIES = 5;

void offset_momentum(Body* bodies) {
    double px = 0.0, py = 0.0, pz = 0.0;
    for (size_t i = 0; i < N_BODIES; i++) {
        px += bodies[i].vx * bodies[i].mass;
        py += bodies[i].vy * bodies[i].mass;
        pz += bodies[i].vz * bodies[i].mass;
    }
    bodies[0].vx = -px / SOLAR_MASS;
    bodies[0].vy = -py / SOLAR_MASS;
    bodies[0].vz = -pz / SOLAR_MASS;
}

void advance(Body* bodies, double dt) {
    for (size_t i = 0; i < N_BODIES; i++) {
        for (size_t j = i + 1; j < N_BODIES; j++) {
            double dx = bodies[i].x - bodies[j].x;
            double dy = bodies[i].y - bodies[j].y;
            double dz = bodies[i].z - bodies[j].z;
            double dSquared = dx * dx + dy * dy + dz * dz;
            double distance = std::sqrt(dSquared);
            double mag = dt / (dSquared * distance);

            bodies[i].vx -= dx * bodies[j].mass * mag;
            bodies[i].vy -= dy * bodies[j].mass * mag;
            bodies[i].vz -= dz * bodies[j].mass * mag;

            bodies[j].vx += dx * bodies[i].mass * mag;
            bodies[j].vy += dy * bodies[i].mass * mag;
            bodies[j].vz += dz * bodies[i].mass * mag;
        }
    }
    for (size_t i = 0; i < N_BODIES; i++) {
        bodies[i].x += dt * bodies[i].vx;
        bodies[i].y += dt * bodies[i].vy;
        bodies[i].z += dt * bodies[i].vz;
    }
}

double energy(const Body* bodies) {
    double e = 0.0;
    for (size_t i = 0; i < N_BODIES; i++) {
        e += 0.5 * bodies[i].mass * (bodies[i].vx * bodies[i].vx +
                                     bodies[i].vy * bodies[i].vy +
                                     bodies[i].vz * bodies[i].vz);
        for (size_t j = i + 1; j < N_BODIES; j++) {
            double dx = bodies[i].x - bodies[j].x;
            double dy = bodies[i].y - bodies[j].y;
            double dz = bodies[i].z - bodies[j].z;
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            e -= (bodies[i].mass * bodies[j].mass) / distance;
        }
    }
    return e;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_NativeBenchmarks_runNBodyBenchmarkCpp(
        JNIEnv* env, jclass, jint n) {

    clock_t start = clock();

    // Run 143 iterations to match Java
    for (int iteration = 0; iteration < 143; iteration++) {
        Body bodies[N_BODIES] = {
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

        offset_momentum(bodies);

        for (int i = 0; i < n; i++) {
            advance(bodies, 0.01);
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "NBody C++ duration: %ldms", duration);

    std::ostringstream oss;
    oss << "NBody C++ completed: " << duration << "ms";
    return env->NewStringUTF(oss.str().c_str());
}
