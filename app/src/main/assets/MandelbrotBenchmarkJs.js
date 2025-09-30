function getByte(x, y, Crb, Cib) {
    var res = 0;
    for (var i = 0; i < 8; i += 2) {
        var Zr1 = Crb[x + i];
        var Zi1 = Cib[y];
        var Zr2 = Crb[x + i + 1];
        var Zi2 = Cib[y];

        var b = 0;
        var j = 49;
        do {
            var nZr1 = Zr1 * Zr1 - Zi1 * Zi1 + Crb[x + i];
            var nZi1 = Zr1 * Zi1 + Zr1 * Zi1 + Cib[y];
            Zr1 = nZr1;
            Zi1 = nZi1;

            var nZr2 = Zr2 * Zr2 - Zi2 * Zi2 + Crb[x + i + 1];
            var nZi2 = Zr2 * Zi2 + Zr2 * Zi2 + Cib[y];
            Zr2 = nZr2;
            Zi2 = nZi2;

            if (Zr1 * Zr1 + Zi1 * Zi1 > 4) {
                b |= 2;
                if (b === 3) break;
            }
            if (Zr2 * Zr2 + Zi2 * Zi2 > 4) {
                b |= 1;
                if (b === 3) break;
            }
        } while (--j > 0);
        res = (res << 2) + b;
    }
    return res ^ -1;
}

function runMandelbrotBenchmark(n, iterations) {
    var startTime = Date.now();

    // Run specified number of iterations
    for (var iter = 0; iter < iterations; iter++) {
        var Crb = new Array(n + 7);
        var Cib = new Array(n + 7);
        var invN = 2.0 / n;

        for (var i = 0; i < n; i++) {
            Cib[i] = i * invN - 1.0;
            Crb[i] = i * invN - 1.5;
        }

        var out = new Array(n);
        for (var i = 0; i < n; i++) {
            out[i] = new Array(Math.floor((n + 7) / 8));
        }

        // Single-threaded computation (JS doesn't have easy threading in Duktape)
        for (var y = 0; y < n; y++) {
            for (var xb = 0; xb < Math.floor((n + 7) / 8); xb++) {
                out[y][xb] = getByte(xb * 8, y, Crb, Cib);
            }
        }
    }

    var duration = Date.now() - startTime;
    return "Mandelbrot JS completed: " + duration + "ms (" + iterations + " iterations)";
}