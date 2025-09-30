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
  let px = 0, py = 0, pz = 0;
  const size = bodies.length;
  for (let i = 0; i < size; i++) {
    const body = bodies[i];
    const mass = body.mass;
    px += body.vx * mass;
    py += body.vy * mass;
    pz += body.vz * mass;
  }
  const body = bodies[0];
  body.vx = -px / SOLAR_MASS;
  body.vy = -py / SOLAR_MASS;
  body.vz = -pz / SOLAR_MASS;
}
function advance(bodies, dt) {
  const size = bodies.length;
  for (let i = 0; i < size; i++) {
    const bodyi = bodies[i];
    let vxi = bodyi.vx;
    let vyi = bodyi.vy;
    let vzi = bodyi.vz;
    for (let j = i + 1; j < size; j++) {
      const bodyj = bodies[j];
      const dx = bodyi.x - bodyj.x;
      const dy = bodyi.y - bodyj.y;
      const dz = bodyi.z - bodyj.z;
      const d2 = dx * dx + dy * dy + dz * dz;
      const mag = dt / (d2 * Math.sqrt(d2));
      const massj = bodyj.mass;
      vxi -= dx * massj * mag;
      vyi -= dy * massj * mag;
      vzi -= dz * massj * mag;
      const massi = bodyi.mass;
      bodyj.vx += dx * massi * mag;
      bodyj.vy += dy * massi * mag;
      bodyj.vz += dz * massi * mag;
    }
    bodyi.vx = vxi;
    bodyi.vy = vyi;
    bodyi.vz = vzi;
  }
  for (let i = 0; i < size; i++) {
    const body = bodies[i];
    body.x += dt * body.vx;
    body.y += dt * body.vy;
    body.z += dt * body.vz;
  }
}
function energy(bodies) {
  let e = 0;
  const size = bodies.length;
  for (let i = 0; i < size; i++) {
    const bodyi = bodies[i];
    e +=
      0.5 *
      bodyi.mass *
      (bodyi.vx * bodyi.vx + bodyi.vy * bodyi.vy + bodyi.vz * bodyi.vz);
    for (let j = i + 1; j < size; j++) {
      const bodyj = bodies[j];
      const dx = bodyi.x - bodyj.x;
      const dy = bodyi.y - bodyj.y;
      const dz = bodyi.z - bodyj.z;
      const distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
      e -= (bodyi.mass * bodyj.mass) / distance;
    }
  }
  return e;
}

// Entry point for Android
function runNBodyBenchmark(n) {
  const bodies = [Sun(), Jupiter(), Saturn(), Uranus(), Neptune()];
  offsetMomentum(bodies);
  var output = "";
  output += energy(bodies).toFixed(9) + "\n";
  for (let i = 0; i < n; i++) {
    advance(bodies, 0.01);
  }
  output += energy(bodies).toFixed(9) + "\n";
  return output;
}
