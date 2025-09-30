package com.example.myapplication

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

        try {
            val minDepth = 4
            val maxDepth = 16
            val stretchDepth = maxDepth + 1

            val output = StringBuilder()

            // Run 150 iterations to get ~30-60 seconds
            for (iteration in 0 until 150) {
                run {
                    val stretchTree = TreeNode.bottomUpTree(stretchDepth)
                    output.append("stretch tree of depth $stretchDepth\t check: ${stretchTree.itemCheck()}\n")
                }

                val longLivedTree = TreeNode.bottomUpTree(maxDepth)

                for (depth in minDepth..maxDepth step 2) {
                    val iterations = 1 shl (maxDepth - depth + minDepth)
                    var check = 0
                    for (i in 0 until iterations) {
                        val tree = TreeNode.bottomUpTree(depth)
                        check += tree.itemCheck()
                    }
                    output.append("$iterations\t trees of depth $depth\t check: $check\n")
                }

                output.append("long lived tree of depth $maxDepth\t check: ${longLivedTree.itemCheck()}\n")
            }

            val duration = System.currentTimeMillis() - startTime
            Log.d("BENCHMARK", "BinaryTrees Kotlin duration: ${duration}ms")

            return output.toString()
        } finally {
            Trace.endSection()
        }
    }
}