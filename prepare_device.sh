#!/bin/bash

echo "📵 Killing all background apps..."
adb shell am kill-all

echo "🧹 Clearing cached processes..."
adb shell cmd activity clear-all

echo "❄️ Disabling app standby + doze..."
adb shell dumpsys deviceidle disable

echo "⚙️ Setting performance mode..."
adb shell cmd power set-mode performance

echo "🔌 Checking charging state..."
adb shell dumpsys battery | grep "level\|temperature\|status"



