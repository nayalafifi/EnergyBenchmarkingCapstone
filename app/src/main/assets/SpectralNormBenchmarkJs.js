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

function runSpectralNormBenchmark(n) {
    var u = [];
    var v = [];
    var tmp = [];
    for (var i = 0; i < n; i++) u[i] = 1;
    for (var i = 0; i < n; i++) v[i] = 0;
    for (var i = 0; i < n; i++) tmp[i] = 0;

    for (var iter = 0; iter < 10; iter++) {
        Au(n, u, tmp);
        Atu(n, tmp, v);
        Au(n, v, tmp);
        Atu(n, tmp, u);
    }

    var vBv = 0, vv = 0;
    for (var i = 0; i < n; i++) {
        vBv += u[i] * v[i];
        vv += v[i] * v[i];
    }
    return (Math.sqrt(vBv / vv)).toFixed(9);
}
