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
        int iterations = 150;

        try {
            int minDepth = 4;
            int maxDepth = 16;
            int stretchDepth = maxDepth + 1;

            // Run 150 iterations to get ~30 seconds
            for (int iteration = 0; iteration < iterations; iteration++) {
                {
                    TreeNode stretchTree = TreeNode.bottomUpTree(stretchDepth);
                    stretchTree.itemCheck(); // Just compute, don't store
                }

                TreeNode longLivedTree = TreeNode.bottomUpTree(maxDepth);

                for (int depth = minDepth; depth <= maxDepth; depth += 2) {
                    int iterationsInner = 1 << (maxDepth - depth + minDepth);
                    int check = 0;
                    for (int i = 0; i < iterationsInner; i++) {
                        TreeNode tree = TreeNode.bottomUpTree(depth);
                        check += tree.itemCheck();
                    }
                }

                longLivedTree.itemCheck(); // Just compute, don't store
            }

            long duration = System.currentTimeMillis() - startTime;

            String result = "BinaryTrees Java completed: " + duration + "ms (" + iterations + " iterations)";
            Log.d("BENCHMARK", result);

            return result;
        } finally {
            Trace.endSection();
        }
    }
}