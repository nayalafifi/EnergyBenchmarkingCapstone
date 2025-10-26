function mandelbrot(w, h) {
    var output = new Array(Math.floor(w * h / 8) + 1);
    var bit_num = 0;
    var byte_acc = 0;
    var iter = 50;
    var limit = 2.0;
    var Zr, Zi, Cr, Ci, Tr, Ti;

    var index = 0;
    for (var y = 0; y < h; y++) {
        for (var x = 0; x < w; x++) {
            Zr = Zi = Tr = Ti = 0.0;
            Cr = (2.0 * x / w - 1.5);
            Ci = (2.0 * y / h - 1.0);

            var i = 0;
            for (i = 0; i < iter && (Tr + Ti <= limit * limit); i++) {
                Zi = 2.0 * Zr * Zi + Ci;
                Zr = Tr - Ti + Cr;
                Tr = Zr * Zr;
                Ti = Zi * Zi;
            }

            byte_acc <<= 1;
            if (Tr + Ti <= limit * limit) byte_acc |= 0x01;

            bit_num++;

            if (bit_num == 8) {
                output[index++] = byte_acc;
                byte_acc = 0;
                bit_num = 0;
            } else if (x == w - 1) {
                byte_acc <<= (8 - w % 8);
                output[index++] = byte_acc;
                byte_acc = 0;
                bit_num = 0;
            }
        }
    }
    return output;
}

function runMandelbrotBenchmark(n) {
    var startTime = Date.now();
    var iterations = 100;

    for (var iter = 0; iter < iterations; iter++) {
        mandelbrot(n, n);
    }

    var duration = Date.now() - startTime;
    return "Mandelbrot JS completed: " + duration + "ms (" + iterations + " iterations)";
}