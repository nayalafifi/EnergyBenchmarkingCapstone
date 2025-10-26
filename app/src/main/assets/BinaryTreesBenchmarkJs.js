function TreeNode(left, right) {
    this.left = left;
    this.right = right;
}

TreeNode.prototype.itemCheck = function() {
    if (this.left === null) return 1;
    return 1 + this.left.itemCheck() + this.right.itemCheck();
};

function bottomUpTree(depth) {
    if (depth > 0) {
        return new TreeNode(
            bottomUpTree(depth - 1),
            bottomUpTree(depth - 1)
        );
    }
    return new TreeNode(null, null);
}

function runBinaryTreesBenchmark(n) {
    var startTime = Date.now();
    var iterations = 150;

    for (var iter = 0; iter < iterations; iter++) {
        var minDepth = 4;
        var maxDepth = Math.max(minDepth + 2, n);
        var stretchDepth = maxDepth + 1;

        var check = bottomUpTree(stretchDepth).itemCheck();

        var longLivedTree = bottomUpTree(maxDepth);

        for (var depth = minDepth; depth <= maxDepth; depth += 2) {
            var iterations_inner = 1 << (maxDepth - depth + minDepth);
            var check_inner = 0;

            for (var i = 1; i <= iterations_inner; i++) {
                check_inner += bottomUpTree(depth).itemCheck();
            }
        }

        longLivedTree.itemCheck();
    }

    var duration = Date.now() - startTime;
    return "BinaryTrees JS completed: " + duration + "ms (" + iterations + " iterations)";
}