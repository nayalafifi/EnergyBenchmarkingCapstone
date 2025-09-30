// DNA complement map (IUPAC codes)
var complement = {
    'A': 'T', 'a': 't',
    'C': 'G', 'c': 'g',
    'G': 'C', 'g': 'c',
    'T': 'A', 't': 'a',
    'U': 'A', 'u': 'a',
    'M': 'K', 'm': 'k',
    'R': 'Y', 'r': 'y',
    'W': 'W', 'w': 'w',
    'S': 'S', 's': 's',
    'Y': 'R', 'y': 'r',
    'K': 'M', 'k': 'm',
    'V': 'B', 'v': 'b',
    'H': 'D', 'h': 'd',
    'D': 'H', 'd': 'h',
    'B': 'V', 'b': 'v',
    'N': 'N', 'n': 'n'
};

// Returns the reverse complement of a DNA sequence
function reverseComplement(seq) {
    var out = '';
    for (var i = seq.length - 1; i >= 0; i--) {
        var c = seq[i];
        out += complement[c] || c;
    }
    return out;
}

// Main entry point for Android: takes a FASTA string, returns reverse-complemented FASTA
function runRevCompBenchmark(fastaInput) {
    var lines = fastaInput.split('\n');
    var output = '';
    var seq = '';
    var header = '';
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i];
        if (line.startsWith('>')) {
            // If we have a previous sequence, output its reverse complement
            if (seq.length > 0) {
                var rev = reverseComplement(seq);
                // Output in lines of 60 chars
                for (var j = 0; j < rev.length; j += 60) {
                    output += rev.substr(j, 60) + '\n';
                }
                seq = '';
            }
            header = line;
            output += header + '\n';
        } else {
            seq += line.trim();
        }
    }
    // Output last sequence
    if (seq.length > 0) {
        var rev = reverseComplement(seq);
        for (var j = 0; j < rev.length; j += 60) {
            output += rev.substr(j, 60) + '\n';
        }
    }
    return output;
}
