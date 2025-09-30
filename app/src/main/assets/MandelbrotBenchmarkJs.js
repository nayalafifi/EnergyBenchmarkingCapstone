function runMandelbrotBenchmark(size) {
    // Parameters
    var maxIter = 50;
    var limit = 4;
    var output = "";

    // Precompute initial real and imaginary values
    var initialR = [];
    var initialI = [];
    var inv = 2 / size;
    for (var xy = 0; xy < size; xy++) {
        var i = inv * xy;
        initialR[xy] = i - 1.5;
        initialI[xy] = i - 1.0;
    }

    // Compute Mandelbrot set
    for (var y = 0; y < size; y++) {
        var row = [];
        var ci = initialI[y];
        for (var x = 0; x < size; x++) {
            var cr = initialR[x];
            var zr = cr, zi = ci;
            var iter = 0;
            while (zr * zr + zi * zi <= limit && iter < maxIter) {
                var temp = zr * zr - zi * zi + cr;
                zi = 2 * zr * zi + ci;
                zr = temp;
                iter++;
            }
            row.push(iter);
        }
        output += row.join(" ") + "\n";
    }
    return output;
}
