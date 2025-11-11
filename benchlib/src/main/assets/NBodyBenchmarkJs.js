var PI = 3.141592653589793;
var SOLAR_MASS = 4 * PI * PI;
var DAYS_PER_YEAR = 365.24;

function Body(x, y, z, vx, vy, vz, mass) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.vx = vx;
    this.vy = vy;
    this.vz = vz;
    this.mass = mass;
}

function advance(bodies, dt) {
    for (var i = 0; i < bodies.length; i++) {
        var b = bodies[i];
        for (var j = i + 1; j < bodies.length; j++) {
            var b2 = bodies[j];
            var dx = b.x - b2.x;
            var dy = b.y - b2.y;
            var dz = b.z - b2.z;
            var distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            var mag = dt / (distance * distance * distance);

            b.vx -= dx * b2.mass * mag;
            b.vy -= dy * b2.mass * mag;
            b.vz -= dz * b2.mass * mag;

            b2.vx += dx * b.mass * mag;
            b2.vy += dy * b.mass * mag;
            b2.vz += dz * b.mass * mag;
        }
    }

    for (var i = 0; i < bodies.length; i++) {
        var b = bodies[i];
        b.x += dt * b.vx;
        b.y += dt * b.vy;
        b.z += dt * b.vz;
    }
}

function energy(bodies) {
    var e = 0.0;

    for (var i = 0; i < bodies.length; i++) {
        var b = bodies[i];
        e += 0.5 * b.mass * (b.vx * b.vx + b.vy * b.vy + b.vz * b.vz);

        for (var j = i + 1; j < bodies.length; j++) {
            var b2 = bodies[j];
            var dx = b.x - b2.x;
            var dy = b.y - b2.y;
            var dz = b.z - b2.z;
            var distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            e -= (b.mass * b2.mass) / distance;
        }
    }
    return e;
}

function offsetMomentum(bodies) {
    var px = 0.0, py = 0.0, pz = 0.0;

    for (var i = 0; i < bodies.length; i++) {
        px += bodies[i].vx * bodies[i].mass;
        py += bodies[i].vy * bodies[i].mass;
        pz += bodies[i].vz * bodies[i].mass;
    }

    bodies[0].vx = -px / SOLAR_MASS;
    bodies[0].vy = -py / SOLAR_MASS;
    bodies[0].vz = -pz / SOLAR_MASS;
}

function runNBodyBenchmark(n) {
    var startTime = Date.now();
    var iterations = 143;

    var bodies = [
        new Body(0, 0, 0, 0, 0, 0, SOLAR_MASS),
        new Body(4.84143144246472090e+00, -1.16032004402742839e+00, -1.03622044471123109e-01,
                 1.66007664274403694e-03 * DAYS_PER_YEAR, 7.69901118419740425e-03 * DAYS_PER_YEAR,
                 -6.90460016972063023e-05 * DAYS_PER_YEAR, 9.54791938424326609e-04 * SOLAR_MASS),
        new Body(8.34336671824457987e+00, 4.12479856412430479e+00, -4.03523417114321381e-01,
                 -2.76742510726862411e-03 * DAYS_PER_YEAR, 4.99852801234917238e-03 * DAYS_PER_YEAR,
                 2.30417297573763929e-05 * DAYS_PER_YEAR, 2.85885980666130812e-04 * SOLAR_MASS),
        new Body(1.28943695621391310e+01, -1.51111514016986312e+01, -2.23307578892655734e-01,
                 2.96460137564761618e-03 * DAYS_PER_YEAR, 2.37847173959480950e-03 * DAYS_PER_YEAR,
                 -2.96589568540237556e-05 * DAYS_PER_YEAR, 4.36624404335156298e-05 * SOLAR_MASS),
        new Body(1.53796971148509165e+01, -2.59193146099879641e+01, 1.79258772950371181e-01,
                 2.68067772490389322e-03 * DAYS_PER_YEAR, 1.62824170038242295e-03 * DAYS_PER_YEAR,
                 -9.51592254519715870e-05 * DAYS_PER_YEAR, 5.15138902046611451e-05 * SOLAR_MASS)
    ];

    for (var iter = 0; iter < iterations; iter++) {
        offsetMomentum(bodies);
        energy(bodies);

        for (var i = 0; i < n; i++) {
            advance(bodies, 0.01);
        }

        energy(bodies);
    }

    var duration = Date.now() - startTime;
    return "NBody JS completed: " + duration + "ms (" + iterations + " iterations)";
}