#!/bin/bash

BENCHMARKS=("BinaryTreesJava" "BinaryTreesKotlin" "BinaryTreesC" "BinaryTreesCpp" "BinaryTreesJs") #begin with binary trees benchmarks
RUNS=10 #10 runs
PACKAGE="com.example.myapplication"

for benchmark in "${BENCHMARKS[@]}"; do
    for run in $(seq 1 $RUNS); do
        echo "Running $benchmark - iteration $run"

        # Create Perfetto config file
        cat > /tmp/perfetto_config.txt << 'EOF'
buffers: {
    size_kb: 522240
    fill_policy: DISCARD
}
data_sources: {
    config {
        name: "android.power"
        android_power_config {
            battery_poll_ms: 250
            collect_power_rails: true
        }
    }
}
duration_ms: 70000
EOF

        # Start Perfetto trace in background
        adb shell perfetto -c - --txt -o /data/misc/perfetto-traces/trace_${benchmark}_${run}.perfetto-trace < /tmp/perfetto_config.txt &
        PERFETTO_PID=$!

        # Wait for trace to start
        sleep 5

        # Launch app with specific benchmark
        adb shell am start -n $PACKAGE/.MainActivity --es "BENCHMARK" "$benchmark"

        # Wait for benchmark completion, 65 secs is the average for all benchmarks
        sleep 65

        # Kill Perfetto
        kill $PERFETTO_PID 2>/dev/null

        # Pull trace
        adb pull /data/misc/perfetto-traces/trace_${benchmark}_${run}.perfetto-trace ./traces/

        # Force stop app
        adb shell am force-stop $PACKAGE

        # Clean up device trace
        adb shell rm /data/misc/perfetto-traces/trace_${benchmark}_${run}.perfetto-trace

        # Wait between runs
        sleep 10
    done
done

echo "All benchmarks completed. Traces saved to ./traces/"