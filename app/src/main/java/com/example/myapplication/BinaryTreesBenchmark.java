package com.example.myapplication;

import android.os.Trace;
import android.util.Log;

public final class BinaryTreesBenchmark {

    static class TreeNode {
        TreeNode left, right;

        TreeNode(TreeNode left, TreeNode right) {
            this.left = left;
            this.right = right;
        }

        static TreeNode bottomUpTree(int depth) {
            if (depth > 0)
                return new TreeNode(bottomUpTree(depth - 1), bottomUpTree(depth - 1));
            else
                return new TreeNode(null, null);
        }

        int itemCheck() {
            if (left == null) return 1;
            return 1 + left.itemCheck() + right.itemCheck();
        }
    }

    public static String runBenchmark() {
        Trace.beginSection("BinaryTrees Benchmark");

        long startTime = System.currentTimeMillis();

        try {
            int minDepth = 4;
            int maxDepth = 16;
            int stretchDepth = maxDepth + 1;

            StringBuilder output = new StringBuilder();

            // Run 150 iterations to get ~30 seconds (15 iterations = 3s, so 150 ≈ 30s)
            for (int iteration = 0; iteration < 150; iteration++) {
                {
                    TreeNode stretchTree = TreeNode.bottomUpTree(stretchDepth);
                    output.append("stretch tree of depth ").append(stretchDepth)
                            .append("\t check: ").append(stretchTree.itemCheck()).append("\n");
                }

                TreeNode longLivedTree = TreeNode.bottomUpTree(maxDepth);

                for (int depth = minDepth; depth <= maxDepth; depth += 2) {
                    int iterations = 1 << (maxDepth - depth + minDepth);
                    int check = 0;
                    for (int i = 0; i < iterations; i++) {
                        TreeNode tree = TreeNode.bottomUpTree(depth);
                        check += tree.itemCheck();
                    }
                    output.append(iterations).append("\t trees of depth ").append(depth)
                            .append("\t check: ").append(check).append("\n");
                }

                output.append("long lived tree of depth ").append(maxDepth)
                        .append("\t check: ").append(longLivedTree.itemCheck()).append("\n");
            }

            long duration = System.currentTimeMillis() - startTime;
            Log.d("BENCHMARK", "BinaryTrees Java duration: " + duration + "ms");

            return output.toString();
        } finally {
            Trace.endSection();
        }
    }
}