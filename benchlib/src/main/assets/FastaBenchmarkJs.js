// Random number generator
class RandomGenerator {
    constructor() {
        this.last = 42;
    }

    generate() {
        this.last = ((this.last * 3877 + 29573) % 139968) | 0;
        return this.last / 139968;
    }
}

// DNA Generator
class DNAGenerator {
    constructor(probs) {
        this.cumulativeProbs = DNAGenerator._makeCumulative(probs);
    }

    static _makeCumulative(probs) {
        const result = [];
        var sum = 0;

        for (const o of probs) {
            result.push({
                code: o.s.charCodeAt(0),
                cumulativeProb: sum + o.p
            });
            sum += o.p;
        }

        return result;
    }

    randomCode(randomGenerator) {
        const r = randomGenerator.generate();

        for (var i = 0; i < this.cumulativeProbs.length - 1; i++) {
            const p = this.cumulativeProbs[i];

            if (r < p.cumulativeProb) {
                return p.code;
            }
        }

        return this.cumulativeProbs[this.cumulativeProbs.length - 1].code;
    }
}

// ALU sequence
const ALU =
    "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
    "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
    "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
    "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
    "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
    "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
    "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

// Probability tables
const IUB_PROBS = [
    {s: 'a', p: 0.27}, {s: 'c', p: 0.12}, {s: 'g', p: 0.12}, {s: 't', p: 0.27},
    {s: 'B', p: 0.02}, {s: 'D', p: 0.02}, {s: 'H', p: 0.02}, {s: 'K', p: 0.02},
    {s: 'M', p: 0.02}, {s: 'N', p: 0.02}, {s: 'R', p: 0.02}, {s: 'S', p: 0.02},
    {s: 'V', p: 0.02}, {s: 'W', p: 0.02}, {s: 'Y', p: 0.02}
];

const HS_PROBS = [
    {s: 'a', p: 0.3029549426680}, {s: 'c', p: 0.1979883004921},
    {s: 'g', p: 0.1975473066391}, {s: 't', p: 0.3015094502008}
];

// Repeat sequence (no I/O)
function fastaRepeat(n, seq) {
    const lineLen = 60;
    const len = seq.length;
    let pos = 0;

    while (n > 0) {
        const chunkLen = Math.min(lineLen, n);
        for (let i = 0; i < chunkLen; i++) {
            // Would output seq.charCodeAt(pos) here
            const dummy = seq.charCodeAt(pos);
            pos = (pos + 1) % len;
        }
        n -= chunkLen;
    }
}

// Generate random sequence (no I/O)
function fastaRandom(n, generator, randomGen) {
    const lineLen = 60;

    while (n > 0) {
        const chunkLen = Math.min(lineLen, n);
        for (let i = 0; i < chunkLen; i++) {
            const code = generator.randomCode(randomGen);
            // Would output code here
            const dummy = code;
        }
        n -= chunkLen;
    }
}

// Main benchmark driver
function runFastaBenchmark(n, iterations = 1) {
    const start = Date.now();

    for (let iter = 0; iter < iterations; iter++) {
        const random = new RandomGenerator();
        const iubGen = new DNAGenerator(IUB_PROBS);
        const hsGen = new DNAGenerator(HS_PROBS);

        // Generate sequences as per TCLBG
        fastaRepeat(n * 2, ALU);
        fastaRandom(n * 3, iubGen, random);
        fastaRandom(n * 5, hsGen, random);
    }

    const duration = Date.now() - start;
    return `Fasta JS completed: ${duration}ms (${iterations} iterations)`;
}