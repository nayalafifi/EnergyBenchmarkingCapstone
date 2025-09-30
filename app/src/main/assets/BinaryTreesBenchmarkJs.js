function TreeNode(left, right) {
    return {left, right};
}

function itemCheck(node) {
    if (node.left === null) {
        return 1;
    }
    return 1 + itemCheck(node.left) + itemCheck(node.right);
}

function bottomUpTree(depth) {
    return depth > 0
        ? new TreeNode(bottomUpTree(depth - 1), bottomUpTree(depth - 1))
        : new TreeNode(null, null);
}

// Main entry point for Android - with iteration loop and timing
function runBinaryTreesBenchmark(maxDepth, iterations) {
    var startTime = Date.now();

    maxDepth = Math.max(6, maxDepth);

    // Run specified number of iterations to match Java
    for (var iter = 0; iter < iterations; iter++) {
        var stretchDepth = maxDepth + 1;
        var check = itemCheck(bottomUpTree(stretchDepth));

        var longLivedTree = bottomUpTree(maxDepth);

        for (var depth = 4; depth <= maxDepth; depth += 2) {
            var iterCount = 1 << (maxDepth - depth + 4);
            var check = 0;
            for (var i = 0; i < iterCount; i++) {
                check += itemCheck(bottomUpTree(depth));
            }
        }
    }

    var duration = Date.now() - startTime;
    return "BinaryTrees JS completed: " + duration + "ms (" + iterations + " iterations)";
}