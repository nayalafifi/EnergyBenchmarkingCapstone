var complementMap = {
    'A': 'T', 'a': 'T',
    'T': 'A', 't': 'A',
    'G': 'C', 'g': 'C',
    'C': 'G', 'c': 'G',
    'M': 'K', 'm': 'K',
    'R': 'Y', 'r': 'Y',
    'W': 'W', 'w': 'W',
    'S': 'S', 's': 'S',
    'Y': 'R', 'y': 'R',
    'K': 'M', 'k': 'M',
    'V': 'B', 'v': 'B',
    'H': 'D', 'h': 'D',
    'D': 'H', 'd': 'H',
    'B': 'V', 'b': 'V',
    'N': 'N', 'n': 'N'
};

function complement(c) {
    return complementMap[c] || c;
}

function reverseComplement(seq) {
    var arr = seq.split('');
    var len = arr.length;

    for (var i = 0, j = len - 1; i < j; i++, j--) {
        var temp = complement(arr[i]);
        arr[i] = complement(arr[j]);
        arr[j] = temp;
    }

    if (len % 2 === 1) {
        arr[Math.floor(len / 2)] = complement(arr[Math.floor(len / 2)]);
    }

    return arr.join('');
}

function runRevCompBenchmark(fastaInput) {
    var startTime = Date.now();
    var iterations = 1000;

    for (var iter = 0; iter < iterations; iter++) {
        reverseComplement(fastaInput);
    }

    var duration = Date.now() - startTime;
    return "RevComp JS completed: " + duration + "ms (" + iterations + " iterations)";
}