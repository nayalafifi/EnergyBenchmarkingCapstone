var map = {};

function initializeMap() {
    var from = "ACBDGHKMNSRUTWVYacbdghkmnsrutwvy";
    var to = "TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR";
    
    for (var i = 0; i < from.length; i++) {
        map[from[i]] = to[i];
    }
    map['\n'] = '\n';
    map['>'] = '>';
}

function reverseSection(buf, start, end) {
    while (start < end) {
        if (buf[start] === '\n') {
            start++;
            continue;
        }
        if (buf[end] === '\n') {
            end--;
            continue;
        }
        var temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
}

function runRevCompBenchmark(input, iterations) {
    var startTime = Date.now();
    
    initializeMap();
    
    // Run specified number of iterations
    for (var iter = 0; iter < iterations; iter++) {
        var buf = input.split('');
        
        // Map complement
        for (var i = 0; i < buf.length; i++) {
            var b = buf[i];
            if (b !== '\n' && b !== '>') {
                buf[i] = map[b] || b;
            }
        }
        
        // Reverse each sequence
        var seqStart = 0;
        for (var i = 0; i < buf.length; i++) {
            if (buf[i] === '>') {
                if (i > seqStart) {
                    reverseSection(buf, seqStart, i - 1);
                }
                seqStart = i;
                while (i < buf.length && buf[i] !== '\n') i++;
                seqStart = i + 1;
            }
        }
        if (seqStart < buf.length) {
            reverseSection(buf, seqStart, buf.length - 1);
        }
    }
    
    var duration = Date.now() - startTime;
    return "RevComp JS completed: " + duration + "ms (" + iterations + " iterations)";
}