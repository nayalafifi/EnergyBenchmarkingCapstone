#include <jni.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <android/log.h>

#define PI 3.141592653589793
#define SOLAR_MASS (4 * PI * PI)
#define DAYS_PER_YEAR 365.24

struct planet {
    double x, y, z;
    double vx, vy, vz;
    double mass;
};

void advance(int nbodies, struct planet *bodies, double dt) {
    int i, j;
    for (i = 0; i < nbodies; i++) {
        struct planet *b = &(bodies[i]);
        for (j = i + 1; j < nbodies; j++) {
            struct planet *b2 = &(bodies[j]);
            double dx = b->x - b2->x;
            double dy = b->y - b2->y;
            double dz = b->z - b2->z;
            double distanced = dx * dx + dy * dy + dz * dz;
            double distance = sqrt(distanced);
            double mag = dt / (distanced * distance);
            b->vx -= dx * b2->mass * mag;
            b->vy -= dy * b2->mass * mag;
            b->vz -= dz * b2->mass * mag;
            b2->vx += dx * b->mass * mag;
            b2->vy += dy * b->mass * mag;
            b2->vz += dz * b->mass * mag;
        }
    }
    for (i = 0; i < nbodies; i++) {
        struct planet *b = &(bodies[i]);
        b->x += dt * b->vx;
        b->y += dt * b->vy;
        b->z += dt * b->vz;
    }
}

double energy(int nbodies, struct planet *bodies) {
    double e = 0.0;
    int i, j;
    for (i = 0; i < nbodies; i++) {
        struct planet *b = &(bodies[i]);
        e += 0.5 * b->mass * (b->vx * b->vx + b->vy * b->vy + b->vz * b->vz);
        for (j = i + 1; j < nbodies; j++) {
            struct planet *b2 = &(bodies[j]);
            double dx = b->x - b2->x;
            double dy = b->y - b2->y;
            double dz = b->z - b2->z;
            double distance = sqrt(dx * dx + dy * dy + dz * dz);
            e -= (b->mass * b2->mass) / distance;
        }
    }
    return e;
}

void offset_momentum(int nbodies, struct planet *bodies) {
    double px = 0.0, py = 0.0, pz = 0.0;
    int i;
    for (i = 0; i < nbodies; i++) {
        px += bodies[i].vx * bodies[i].mass;
        py += bodies[i].vy * bodies[i].mass;
        pz += bodies[i].vz * bodies[i].mass;
    }
    bodies[0].vx = -px / SOLAR_MASS;
    bodies[0].vy = -py / SOLAR_MASS;
    bodies[0].vz = -pz / SOLAR_MASS;
}

#define NBODIES 5

JNIEXPORT jstring JNICALL Java_com_example_myapplication_NativeBenchmarks_runNBodyBenchmarkC(
        JNIEnv *env, jclass clazz, jint steps) {

    clock_t start = clock();

    // Run 143 iterations to match Java
    for (int iteration = 0; iteration < 143; iteration++) {
        struct planet bodies[NBODIES] = {
                {0, 0, 0, 0, 0, 0, SOLAR_MASS},
                {4.84143144246472090, -1.16032004402742839, -0.103622044471123109,
                          1.66007664274403694e-03 * DAYS_PER_YEAR,
                             7.69901118419740425e-03 * DAYS_PER_YEAR,
                                -6.90460016972063023e-05 * DAYS_PER_YEAR,
                        9.54791938424326609e-04 * SOLAR_MASS},
                {8.34336671824457987, 4.12479856412430479, -0.403523417114321381,
                          -2.76742510726862411e-03 * DAYS_PER_YEAR,
                             4.99852801234917238e-03 * DAYS_PER_YEAR,
                                2.30417297573763929e-05 * DAYS_PER_YEAR,
                        2.85885980666130812e-04 * SOLAR_MASS},
                {12.894369562139131, -15.1111514016986312, -0.223307578892655734,
                          2.96460137564761618e-03 * DAYS_PER_YEAR,
                             2.3784717395948095e-03 * DAYS_PER_YEAR,
                                -2.96589568540237556e-05 * DAYS_PER_YEAR,
                        4.36624404335156298e-05 * SOLAR_MASS},
                {15.3796971148509165, -25.9193146099879641, 0.179258772950371181,
                          2.68067772490389322e-03 * DAYS_PER_YEAR,
                             1.62824170038242295e-03 * DAYS_PER_YEAR,
                                -9.5159225451971587e-05 * DAYS_PER_YEAR,
                        5.15138902046611451e-05 * SOLAR_MASS}
        };

        offset_momentum(NBODIES, bodies);

        for (int i = 0; i < steps; i++) {
            advance(NBODIES, bodies, 0.01);
        }
    }

    clock_t end = clock();
    long duration = ((end - start) * 1000) / CLOCKS_PER_SEC;

    __android_log_print(ANDROID_LOG_DEBUG, "BENCHMARK", "NBody C duration: %ldms", duration);

    char output[256];
    snprintf(output, sizeof(output), "NBody C completed: %ldms", duration);
    return (*env)->NewStringUTF(env, output);
}