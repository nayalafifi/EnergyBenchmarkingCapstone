#!/bin/bash
set -euo pipefail

BENCHMARK="${BENCHMARK:-FannkuchRedux}"   # Change this to run a different benchmark
RUNS="${RUNS:-10}"
COOLDOWN="${COOLDOWN:-30}"
MAX_WAIT="${MAX_WAIT:-300}"

for LANGUAGE in Java Kotlin C Cpp; do

export CSV_FILENAME="$(echo $BENCHMARK | tr '[:upper:]' '[:lower:]')_$(echo $LANGUAGE | tr '[:upper:]' '[:lower:]')_auto.csv"
export LANGUAGE BENCHMARK RUNS COOLDOWN MAX_WAIT
export APP_MODULE="app"
export TEST_CLASS="com.example.myapplication.BenchmarkJUnitTest"
export TEST_METHOD="run${BENCHMARK}${LANGUAGE}"
export GRADLE_TASK=":$APP_MODULE:connectedAndroidTest"

# Ensure only one connected device
if adb devices | awk 'NR>1 && NF{print $2}' | grep -qv '^device$'; then
  echo "⚠️  Multiple or offline devices detected. Specify which to use:"
  adb devices
  echo "   Example: ANDROID_SERIAL=<serial> $0"
  exit 1
fi

echo "========================================================================"
echo "JUnit + E-MANAFA automated benchmark"
echo "Benchmark: $BENCHMARK | Language: $LANGUAGE | Runs: $RUNS"
echo "CSV: $CSV_FILENAME"
echo "JUnit target: ${TEST_CLASS}#${TEST_METHOD}"
echo "========================================================================"
# read -p "Press ENTER to start..."

# Create CSV header if needed
if [ ! -f "$CSV_FILENAME" ]; then
  echo "timestamp,language,benchmark,run,total_energy_j,duration_ms,charge_before,charge_after,temp_before_c,temp_after_c,status" > "$CSV_FILENAME"
  echo "✅ Created $CSV_FILENAME"
fi

# Build AND INSTALL once
echo "🔧 Building and installing app and tests..."
./gradlew :$APP_MODULE:assembleDebug :$APP_MODULE:assembleAndroidTest -q
./gradlew :$APP_MODULE:installDebug :$APP_MODULE:installDebugAndroidTest -q || {
    echo "❌ Installation failed. Trying to uninstall and reinstall..."
    adb uninstall com.example.myapplication || true
    adb uninstall com.example.myapplication.test || true
    ./gradlew :$APP_MODULE:installDebug :$APP_MODULE:installDebugAndroidTest -q
}
echo "✅ Installation complete"

for run in $(seq 1 $RUNS); do
  echo ""
  echo "========================================================================"
  echo "Run $run of $RUNS - $(date '+%Y-%m-%d %H:%M:%S')"
  echo "========================================================================"

  python3 << 'PYTHON_SCRIPT'
import os, sys, time, subprocess, shlex
from datetime import datetime

LANGUAGE      = os.environ.get("LANGUAGE", "Java")
BENCHMARK     = os.environ.get("BENCHMARK", "BinaryTrees")
RUN_NUM       = int(os.environ.get("run", "1"))
CSV_FILENAME  = os.environ["CSV_FILENAME"]
MAX_WAIT      = int(os.environ.get("MAX_WAIT", "300"))
TEST_CLASS    = os.environ["TEST_CLASS"]
TEST_METHOD   = os.environ["TEST_METHOD"]
PACKAGE       = "com.example.myapplication"

sys.path.insert(0, '../')
try:
    from manafa.emanafa import EManafa
except Exception as e:
    print(f"❌ Could not import E-MANAFA: {e}")
    sys.exit(1)

def run_cmd(cmd):
    return subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

def get_battery_level():
    out = run_cmd(["adb", "shell", "dumpsys", "battery"]).stdout
    for line in out.splitlines():
        if "level" in line:
            return int(line.split(":")[1].strip())
    return None

def get_battery_temp():
    out = run_cmd(["adb", "shell", "dumpsys", "battery"]).stdout
    for line in out.splitlines():
        if "temperature" in line:
            return int(line.split(":")[1].strip()) / 10.0
    return None

charge_before = get_battery_level()
temp_before = get_battery_temp()

print(f"\n🔋 Charge before: {charge_before}%")
print(f"🌡️ Temp before: {temp_before}°C")

run_cmd(['adb', 'shell', 'am', 'force-stop', PACKAGE])
run_cmd(['adb', 'logcat', '-c'])
time.sleep(2)

print("\n1️⃣  Initializing E-MANAFA...")
try:
    em = EManafa()
    em.init()
    print("   ✅ Ready")
except Exception as e:
    print(f"   ❌ Init failed: {e}")
    sys.exit(1)

print("\n🧹 Resetting batterystats…")
run_cmd(["adb", "shell", "dumpsys", "batterystats", "--reset"])
time.sleep(3)

print("\n2️⃣  Starting profiling…")
try:
    em.start()
    print("   ✅ Profiling started")
    print("   ⏳ Warm-up 5s…")
    time.sleep(5)
except Exception as e:
    print(f"   ❌ Profiling start failed: {e}")
    sys.exit(1)

adb_cmd = [
    "adb", "shell", "am", "instrument", "-w",
    "-e", "class", f"{TEST_CLASS}#{TEST_METHOD}",
    "com.example.myapplication.test/androidx.test.runner.AndroidJUnitRunner"
]

print(f"\n3️⃣  Launching JUnit benchmark: {TEST_CLASS}#{TEST_METHOD}")
start_wall = time.time()
proc = subprocess.Popen(adb_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

duration_ms = None
completed = False

print(f"\n⏳ Waiting for BENCHMARK_RESULT (max {MAX_WAIT}s)…")

deadline = time.time() + MAX_WAIT
while time.time() < deadline:
    line = proc.stdout.readline()
    if not line:
        break
    print(line.strip())

    if "Time:" in line:
        try:
            seconds = float(line.split("Time:")[1].strip())
            duration_ms = int(seconds * 1000)
        except:
            pass

    if "OK (" in line and "test" in line:
        completed = True

if not completed:
    print("⚠️ Timeout: benchmark did not finish in time.")
else:
    print("✅ Benchmark finished")

print("\n4️⃣  Stopping profiler…")
try:
    em.stop()
except Exception as e:
    print(f"⚠️ Stop warning: {e}")

print("\n5️⃣ Parsing energy…")
try:
    em.parse_results()
except Exception as e:
    print(f"⚠️ Parse warning: {e}")

try:
    begin = em.perf_events.events[0].time
    end   = em.perf_events.events[-1].time
    total_joules, _, _ = em.get_consumption_in_between(begin, end)
except:
    total_joules = 0.0

charge_after = get_battery_level()
temp_after = get_battery_temp()

print(f"\n🔋 Charge after: {charge_after}%")
print(f"🌡️ Temp after: {temp_after}°C")

ts = datetime.now().isoformat()
status = "success" if completed else "failed"

with open(CSV_FILENAME, "a") as f:
    f.write(
        f"{ts},{LANGUAGE},{BENCHMARK},{RUN_NUM},"
        f"{total_joules:.4f},{duration_ms},"
        f"{charge_before},{charge_after},"
        f"{temp_before},{temp_after},"
        f"{status}\n"
    )

print("\n✅ Run complete.")

run_cmd(['adb', 'shell', 'am', 'force-stop', PACKAGE])

PYTHON_SCRIPT

  if [ $run -lt $RUNS ]; then
    echo ""
    echo "❄️ Cooling down ${COOLDOWN}s…"
    sleep "$COOLDOWN"
  fi
done

echo ""
echo "🎉 ALL RUNS COMPLETED!"
echo "📁 CSV: $CSV_FILENAME"

done


