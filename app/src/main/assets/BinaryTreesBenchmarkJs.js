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

// Main entry point for Android
function runBinaryTreesBenchmark(maxDepth)
{
    maxDepth = Math.max(6, maxDepth);
    var stretchDepth = maxDepth + 1;
    var output = "";

    var check = itemCheck(bottomUpTree(stretchDepth));
    output += "stretch tree of depth " + stretchDepth + "\t check: " + check + "\n";

    var longLivedTree = bottomUpTree(maxDepth);

    for (var depth = 4; depth <= maxDepth; depth += 2) {
        var iterations = 1 << (maxDepth - depth + 4);
        var check = 0;
        for (var i = 0; i < iterations; i++) {
            check += itemCheck(bottomUpTree(depth));
        }
        output += iterations + "\t trees of depth " + depth + "\t check: " + check + "\n";
    }

    output += "long lived tree of depth " + maxDepth + "\t check: " + itemCheck(longLivedTree) + "\n";
    return output;
}
