#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
TRACES_DIR="$PROJECT_DIR/traces"
mkdir -p "$TRACES_DIR"

BENCHMARKS=("BinaryTreesJava")
RUNS=1
PACKAGE="com.example.myapplication"

for benchmark in "${BENCHMARKS[@]}"; do
  for run in $(seq 1 $RUNS); do
    echo "Running $benchmark - iteration $run"

    TRACE_NAME="trace_${benchmark}_${run}"
    TRACE_DEVICE_PATH="/data/misc/perfetto-traces/${TRACE_NAME}.pftrace"
    TRACE_LOCAL_PATH="$TRACES_DIR/${TRACE_NAME}.pftrace"

    # Stop app to ensure clean start
    adb shell am force-stop $PACKAGE || true

    # Start Perfetto in background with 70s duration
    adb shell perfetto --txt -o /data/local/tmp/test_trace.pftrace -c - <<EOF
buffers: { size_kb: 32768 fill_policy: RING_BUFFER }

data_sources: { config { name: "linux.ftrace"
    ftrace_config {
        ftrace_events: "sched/sched_switch"
        ftrace_events: "sched/sched_wakeup"
        ftrace_events: "sched/sched_waking"
        ftrace_events: "cpu_frequency"
        ftrace_events: "cpu_idle"
    }
}}

duration_ms: 10000
EOF


    sleep 5

    # Launch app benchmark
    adb shell am start -n $PACKAGE/.MainActivity --es "BENCHMARK" "$benchmark"

    # Wait until trace finishes
    sleep 70

    # Pull trace file
    adb pull $TRACE_DEVICE_PATH "$TRACE_LOCAL_PATH" || echo "Failed to pull trace!"

    # Show file size
    SIZE=$(stat -c%s "$TRACE_LOCAL_PATH" 2>/dev/null || stat -f%z "$TRACE_LOCAL_PATH" 2>/dev/null || echo 0)
    echo "Trace file size: $SIZE bytes"

    # Cleanup
    adb shell rm -f $TRACE_DEVICE_PATH || true

    sleep 10
  done
done
