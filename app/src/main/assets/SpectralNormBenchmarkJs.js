function A(i, j) {
    return 1 / (((i + j) * (i + j + 1) / 2) + i + 1);
}

function Au(n, u, v) {
    for (var i = 0; i < n; i++) {
        var sum = 0;
        for (var j = 0; j < n; j++) {
            sum += A(i, j) * u[j];
        }
        v[i] = sum;
    }
}

function Atu(n, u, v) {
    for (var i = 0; i < n; i++) {
        var sum = 0;
        for (var j = 0; j < n; j++) {
            sum += A(j, i) * u[j];
        }
        v[i] = sum;
    }
}

function runSpectralNormBenchmark(n, iterations) {
    var startTime = Date.now();

    // Run specified number of iterations
    for (var iter = 0; iter < iterations; iter++) {
        var u = [];
        var v = [];
        var tmp = [];
        for (var i = 0; i < n; i++) u[i] = 1;
        for (var i = 0; i < n; i++) v[i] = 0;
        for (var i = 0; i < n; i++) tmp[i] = 0;

        for (var i = 0; i < 10; i++) {
            Au(n, u, tmp);
            Atu(n, tmp, v);
            Au(n, v, tmp);
            Atu(n, tmp, u);
        }
    }

    var duration = Date.now() - startTime;
    return "SpectralNorm JS completed: " + duration + "ms (" + iterations + " iterations)";
}