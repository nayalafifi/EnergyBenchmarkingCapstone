package com.example.benchlib

import android.annotation.SuppressLint
import android.os.Trace
import android.util.Log

object BinaryTreesBenchmarkKotlin {
    private const val MIN_DEPTH = 4

    private fun bottomUpTree(depth: Int): TreeNode {
        if (0 < depth) {
            return TreeNode(bottomUpTree(depth - 1), bottomUpTree(depth - 1))
        }
        return TreeNode()
    }

    @SuppressLint("UnclosedTrace")
    fun runBenchmark(): String {
        Trace.beginSection("BinaryTrees Benchmark")

        val startTime = System.currentTimeMillis()
        val iterations = 10

        try {
            val n = 18
            val maxDepth = if (n < (MIN_DEPTH + 2)) MIN_DEPTH + 2 else n
            val stretchDepth = maxDepth + 1

            for (iteration in 0..<iterations) {
                // Stretch tree
                val stretchCheck = bottomUpTree(stretchDepth).itemCheck()

                // Long-lived tree
                val longLivedTree = bottomUpTree(maxDepth)

                // Create and check trees at different depths
                // Note: Reference uses parallel execution here, but for Android
                // we'll keep it sequential to avoid threading complexity
                var d = MIN_DEPTH
                while (d <= maxDepth) {
                    val depth = d
                    var check = 0

                    val iterationsInner = 1 shl (maxDepth - depth + MIN_DEPTH)
                    for (i in 1..iterationsInner) {
                        val treeNode1 = bottomUpTree(depth)
                        check += treeNode1.itemCheck()
                    }
                    d += 2
                }

                // Check long-lived tree
                val longLivedCheck = longLivedTree.itemCheck()
            }

            val duration = System.currentTimeMillis() - startTime

            val result =
                "BinaryTrees Java completed: " + duration + "ms (" + iterations + " iterations)"
            Log.d("BENCHMARK", result)

            return result
        } finally {
            Trace.endSection()
        }
    }

    private class TreeNode(
        private val left: TreeNode? = null,
        private val right: TreeNode? = null
    ) {
        fun itemCheck(): Int {
            // if necessary deallocate here
            if (null == left) {
                return 1
            }
            return 1 + left.itemCheck() + right!!.itemCheck()
        }
    }
}