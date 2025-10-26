function eval_A(i, j) {
    return 1.0 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

function eval_A_times_u(N, u, Au) {
    for (var i = 0; i < N; i++) {
        Au[i] = 0;
        for (var j = 0; j < N; j++) {
            Au[i] += eval_A(i, j) * u[j];
        }
    }
}

function eval_At_times_u(N, u, Au) {
    for (var i = 0; i < N; i++) {
        Au[i] = 0;
        for (var j = 0; j < N; j++) {
            Au[i] += eval_A(j, i) * u[j];
        }
    }
}

function eval_AtA_times_u(N, u, AtAu) {
    var v = new Array(N);
    eval_A_times_u(N, u, v);
    eval_At_times_u(N, v, AtAu);
}

function spectralNorm(N) {
    var u = new Array(N);
    var v = new Array(N);

    for (var i = 0; i < N; i++) u[i] = 1.0;

    for (var i = 0; i < 10; i++) {
        eval_AtA_times_u(N, u, v);
        eval_AtA_times_u(N, v, u);
    }

    var vBv = 0.0, vv = 0.0;
    for (var i = 0; i < N; i++) {
        vBv += u[i] * v[i];
        vv += v[i] * v[i];
    }

    return Math.sqrt(vBv / vv);
}

function runSpectralNormBenchmark(n) {
    var startTime = Date.now();
    var iterations = 400;

    for (var iter = 0; iter < iterations; iter++) {
        spectralNorm(n);
    }

    var duration = Date.now() - startTime;
    return "SpectralNorm JS completed: " + duration + "ms (" + iterations + " iterations)";
}