package com.example.benchlib;

import android.os.Trace;
import android.util.Log;

public final class BinaryTreesBenchmark {

    private static final int MIN_DEPTH = 4;

    private static final class TreeNode {
        private final TreeNode left;
        private final TreeNode right;

        private TreeNode(final TreeNode left, final TreeNode right) {
            this.left = left;
            this.right = right;
        }

        private TreeNode() {
            this(null, null);
        }

        private int itemCheck() {
            // if necessary deallocate here
            if (null == left) {
                return 1;
            }
            return 1 + left.itemCheck() + right.itemCheck();
        }
    }

    private static TreeNode bottomUpTree(final int depth) {
        if (0 < depth) {
            return new TreeNode(bottomUpTree(depth - 1), bottomUpTree(depth - 1));
        }
        return new TreeNode();
    }

    public static String runBenchmark() {
        Trace.beginSection("BinaryTrees Benchmark");

        long startTime = System.currentTimeMillis();
        int iterations = 10;

        try {
            int n = 18;
            final int maxDepth = n < (MIN_DEPTH + 2) ? MIN_DEPTH + 2 : n;
            final int stretchDepth = maxDepth + 1;

            for (int iteration = 0; iteration < iterations; iteration++) {
                // Stretch tree
                int stretchCheck = bottomUpTree(stretchDepth).itemCheck();

                // Long-lived tree
                final TreeNode longLivedTree = bottomUpTree(maxDepth);

                // Create and check trees at different depths
                // Note: Reference uses parallel execution here, but for Android
                // we'll keep it sequential to avoid threading complexity
                for (int d = MIN_DEPTH; d <= maxDepth; d += 2) {
                    final int depth = d;
                    int check = 0;

                    final int iterationsInner = 1 << (maxDepth - depth + MIN_DEPTH);
                    for (int i = 1; i <= iterationsInner; ++i) {
                        final TreeNode treeNode1 = bottomUpTree(depth);
                        check += treeNode1.itemCheck();
                    }
                }

                // Check long-lived tree
                int longLivedCheck = longLivedTree.itemCheck();
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