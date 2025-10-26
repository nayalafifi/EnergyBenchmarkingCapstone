// === FASTA (finalized, CLBG-equivalent) ===

// Random number generator constants
let last = 42;
const A = 3877, C = 29573, M = 139968;

function rand(max) {
  last = (last * A + C) % M;
  return (max * last) / M;
}

// DNA sequences
const ALU =
  "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGG" +
  "GAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGA" +
  "CCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAAT" +
  "ACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCA" +
  "GCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGG" +
  "AGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCC" +
  "AGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

// Probability tables (pairs → cumulative arrays)
const IUB_PAIRS = [
  ['a',0.27],['c',0.12],['g',0.12],['t',0.27],
  ['B',0.02],['D',0.02],['H',0.02],['K',0.02],['M',0.02],
  ['N',0.02],['R',0.02],['S',0.02],['V',0.02],['W',0.02],['Y',0.02]
];
const HS_PAIRS = [
  ['a',0.3029549426680],['c',0.1979883004921],
  ['g',0.1975473066391],['t',0.3015094502008]
];

// Build cumulative distributions
function cumulativeFromPairs(pairs) {
  const cumu = [];
  let acc = 0;
  for (let i = 0; i < pairs.length; i++) {
    acc += pairs[i][1];
    cumu.push([pairs[i][0], acc]);
  }
  cumu[cumu.length - 1][1] = 1.0;
  return cumu;
}

const IUB_CUMU = cumulativeFromPairs(IUB_PAIRS);
const HS_CUMU  = cumulativeFromPairs(HS_PAIRS);

// Binary search over cumulative array
function pickFromCumu(r, cumu) {
  let lo = 0, hi = cumu.length - 1;
  while (lo < hi) {
    const mid = (lo + hi) >>> 1;
    if (r < cumu[mid][1]) hi = mid;
    else lo = mid + 1;
  }
  return cumu[lo][0];
}

// Sequence repeat (no I/O)
function fastaRepeat(n, seq) {
  const len = seq.length;
  let i = 0;
  while (n > 0) {
    const L = Math.min(60, n);
    i = (i + L) % len; // emulate wrap-around
    n -= L;
  }
}

// Random sequence generation (no I/O)
function fastaRandom(n, cumu) {
  const lineLen = 60;
  const line = new Array(lineLen);
  while (n > 0) {
    const L = Math.min(lineLen, n);
    for (let i = 0; i < L; i++) {
      const r = rand(1);
      line[i] = pickFromCumu(r, cumu);
    }
    n -= L;
  }
}

// Main benchmark driver
function runFastaBenchmark(n, iterations = 3300) {
  const start = Date.now();

  for (let iter = 0; iter < iterations; iter++) {
    last = 42; // reset RNG seed for repeatability
    fastaRepeat(2 * n, ALU);
    fastaRandom(3 * n, IUB_CUMU);
    fastaRandom(5 * n, HS_CUMU);
  }

  const duration = Date.now() - start;
  return `Fasta JS completed: ${duration}ms (${iterations} iterations)`;
}

// Example usage:
// console.log(runFastaBenchmark(1000));
