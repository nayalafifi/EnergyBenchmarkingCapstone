package com.example.benchlib

import android.os.Trace
import android.util.Log

object BinaryTreesBenchmarkKotlin {

    class TreeNode(val left: TreeNode?, val right: TreeNode?) {

        companion object {
            fun bottomUpTree(depth: Int): TreeNode {
                return if (depth > 0)
                    TreeNode(bottomUpTree(depth - 1), bottomUpTree(depth - 1))
                else
                    TreeNode(null, null)
            }
        }

        fun itemCheck(): Int {
            return if (left == null) 1 else 1 + (left.itemCheck()) + (right?.itemCheck() ?: 0)
        }
    }

    fun runBenchmark(): String {
        Trace.beginSection("BinaryTrees Benchmark")

        val startTime = System.currentTimeMillis()
        val iterations = 90

        try {
            val minDepth = 4
            val maxDepth = 16
            val stretchDepth = maxDepth + 1

            // Run 150 iterations to get ~30-60 seconds
            for (iteration in 0 until iterations) {
                run {
                    val stretchTree = TreeNode.bottomUpTree(stretchDepth)
                    stretchTree.itemCheck() // Just compute, don't store
                }

                val longLivedTree = TreeNode.bottomUpTree(maxDepth)

                for (depth in minDepth..maxDepth step 2) {
                    val iterationsInner = 1 shl (maxDepth - depth + minDepth)
                    var check = 0
                    for (i in 0 until iterationsInner) {
                        val tree = TreeNode.bottomUpTree(depth)
                        check += tree.itemCheck()
                    }
                }

                longLivedTree.itemCheck() // Just compute, don't store
            }

            val duration = System.currentTimeMillis() - startTime

            val result = "BinaryTrees Kotlin completed: ${duration}ms ($iterations iterations)"
            Log.d("BENCHMARK", result)

            return result
        } finally {
            Trace.endSection()
        }
    }
}