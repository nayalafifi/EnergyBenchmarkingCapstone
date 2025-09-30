const PI = Math.PI;
const SOLAR_MASS = 4 * PI * PI;
const DAYS_PER_YEAR = 365.24;

function Body(x, y, z, vx, vy, vz, mass) {
  this.x = x;
  this.y = y;
  this.z = z;
  this.vx = vx;
  this.vy = vy;
  this.vz = vz;
  this.mass = mass;
}

function Jupiter() {
  return new Body(
    4.8414314424647209,
    -1.16032004402742839,
    -1.03622044471123109e-1,
    1.66007664274403694e-3 * DAYS_PER_YEAR,
    7.69901118419740425e-3 * DAYS_PER_YEAR,
    -6.90460016972063023e-5 * DAYS_PER_YEAR,
    9.54791938424326609e-4 * SOLAR_MASS
  );
}

function Saturn() {
  return new Body(
    8.34336671824457987,
    4.12479856412430479,
    -4.03523417114321381e-1,
    -2.76742510726862411e-3 * DAYS_PER_YEAR,
    4.99852801234917238e-3 * DAYS_PER_YEAR,
    2.30417297573763929e-5 * DAYS_PER_YEAR,
    2.85885980666130812e-4 * SOLAR_MASS
  );
}

function Uranus() {
  return new Body(
    1.2894369562139131e1,
    -1.51111514016986312e1,
    -2.23307578892655734e-1,
    2.96460137564761618e-3 * DAYS_PER_YEAR,
    2.3784717395948095e-3 * DAYS_PER_YEAR,
    -2.96589568540237556e-5 * DAYS_PER_YEAR,
    4.36624404335156298e-5 * SOLAR_MASS
  );
}

function Neptune() {
  return new Body(
    1.53796971148509165e1,
    -2.59193146099879641e1,
    1.79258772950371181e-1,
    2.68067772490389322e-3 * DAYS_PER_YEAR,
    1.62824170038242295e-3 * DAYS_PER_YEAR,
    -9.5159225451971587e-5 * DAYS_PER_YEAR,
    5.15138902046611451e-5 * SOLAR_MASS
  );
}

function Sun() {
  return new Body(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SOLAR_MASS);
}

function offsetMomentum(bodies) {
  var px = 0, py = 0, pz = 0;
  var size = bodies.length;
  for (var i = 0; i < size; i++) {
    var body = bodies[i];
    var mass = body.mass;
    px += body.vx * mass;
    py += body.vy * mass;
    pz += body.vz * mass;
  }
  var body = bodies[0];
  body.vx = -px / SOLAR_MASS;
  body.vy = -py / SOLAR_MASS;
  body.vz = -pz / SOLAR_MASS;
}

function advance(bodies, dt) {
  var size = bodies.length;
  for (var i = 0; i < size; i++) {
    var bodyi = bodies[i];
    var vxi = bodyi.vx;
    var vyi = bodyi.vy;
    var vzi = bodyi.vz;
    for (var j = i + 1; j < size; j++) {
      var bodyj = bodies[j];
      var dx = bodyi.x - bodyj.x;
      var dy = bodyi.y - bodyj.y;
      var dz = bodyi.z - bodyj.z;
      var d2 = dx * dx + dy * dy + dz * dz;
      var mag = dt / (d2 * Math.sqrt(d2));
      var massj = bodyj.mass;
      vxi -= dx * massj * mag;
      vyi -= dy * massj * mag;
      vzi -= dz * massj * mag;
      var massi = bodyi.mass;
      bodyj.vx += dx * massi * mag;
      bodyj.vy += dy * massi * mag;
      bodyj.vz += dz * massi * mag;
    }
    bodyi.vx = vxi;
    bodyi.vy = vyi;
    bodyi.vz = vzi;
  }
  for (var i = 0; i < size; i++) {
    var body = bodies[i];
    body.x += dt * body.vx;
    body.y += dt * body.vy;
    body.z += dt * body.vz;
  }
}

// Entry point for Android with iteration loop and timing
function runNBodyBenchmark(n, iterations) {
  var startTime = Date.now();

  // Run specified number of iterations
  for (var iter = 0; iter < iterations; iter++) {
    var bodies = [Sun(), Jupiter(), Saturn(), Uranus(), Neptune()];
    offsetMomentum(bodies);

    for (var i = 0; i < n; i++) {
      advance(bodies, 0.01);
    }
  }

  var duration = Date.now() - startTime;
  return "NBody JS completed: " + duration + "ms (" + iterations + " iterations)";
}