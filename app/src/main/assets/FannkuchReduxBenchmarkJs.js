function fillFact(n) {
    var fact = [];
    fact[0] = 1;
    for (var i = 1; i <= n; i++) {
        fact[i] = fact[i - 1] * i;
    }
    return fact;
}

function div(val, by) {
    return Math.floor(val / by);
}

function fannkuchredux(n) {
    var fact = fillFact(n);
    var maxFlips = 0;
    var checkSum = 0;
    var p = [];
    var pp = [];
    var count = [];
    var permCount = fact[n];

    for (var idx = 0; idx < permCount; idx++) {
        // Generate permutation for idx
        for (var i = 0; i < n; i++) {
            p[i] = i;
        }
        var idxCopy = idx;
        for (var i = n - 1; i > 0; i--) {
            var d = div(idxCopy, fact[i]);
            count[i] = d;
            idxCopy = idxCopy % fact[i];
            for (var j = 0; j < n; j++) {
                pp[j] = p[j];
            }
            for (var j = 0; j <= i; j++) {
                if (j + d <= i) {
                    p[j] = pp[j + d];
                } else {
                    p[j] = pp[j + d - i - 1];
                }
            }
        }
        // Count flips
        var flips = 0;
        for (var j = 0; j < n; j++) {
            pp[j] = p[j];
        }
        var first = pp[0];
        while (first !== 0) {
            var temp = pp[first];
            pp[first] = first;
            if (first > 1) {
                for (var k = 1, l = first - 1; k < l; k++, l--) {
                    var t = pp[k];
                    pp[k] = pp[l];
                    pp[l] = t;
                }
            }
            first = temp;
            flips++;
        }
        if (maxFlips < flips) {
            maxFlips = flips;
        }
        checkSum += (idx % 2 === 0) ? flips : -flips;
    }
    return maxFlips;
}

// Entry point for Android with iteration loop and timing
function runFannkuchReduxBenchmark(n, iterations) {
    var startTime = Date.now();

    // Run specified number of iterations to match Java
    for (var iter = 0; iter < iterations; iter++) {
        fannkuchredux(n);
    }

    var duration = Date.now() - startTime;
    return "FannkuchRedux JS completed: " + duration + "ms (" + iterations + " iterations)";
}