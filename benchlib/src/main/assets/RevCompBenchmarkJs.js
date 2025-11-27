// Lookup table matching TCLBG - using character codes for direct array lookup
var smap = new Array(256);

// Initialize with identity
for (var i = 0; i < 256; i++) {
    smap[i] = i;
}

// Set complement mappings - uppercase
smap[65] = 84;   // A -> T
smap[84] = 65;   // T -> A
smap[67] = 71;   // C -> G
smap[71] = 67;   // G -> C
smap[77] = 75;   // M -> K
smap[82] = 89;   // R -> Y
smap[87] = 87;   // W -> W
smap[83] = 83;   // S -> S
smap[89] = 82;   // Y -> R
smap[75] = 77;   // K -> M
smap[86] = 66;   // V -> B
smap[72] = 68;   // H -> D
smap[68] = 72;   // D -> H
smap[66] = 86;   // B -> V
smap[78] = 78;   // N -> N

// Set complement mappings - lowercase
smap[97] = 84;   // a -> T
smap[116] = 65;  // t -> A
smap[99] = 71;   // c -> G
smap[103] = 67;  // g -> C
smap[109] = 75;  // m -> K
smap[114] = 89;  // r -> Y
smap[119] = 87;  // w -> W
smap[115] = 83;  // s -> S
smap[121] = 82;  // y -> R
smap[107] = 77;  // k -> M
smap[118] = 66;  // v -> B
smap[104] = 68;  // h -> D
smap[100] = 72;  // d -> H
smap[98] = 86;   // b -> V
smap[110] = 78;  // n -> N

function reverseComplement(seq) {
    var len = seq.length;
    var result = new Array(len);

    // Process in reverse order and apply complement lookup
    for (var i = 0; i < len; i++) {
        var code = seq.charCodeAt(len - 1 - i);
        result[i] = String.fromCharCode(smap[code]);
    }

    return result.join('');
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