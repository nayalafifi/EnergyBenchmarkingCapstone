import subprocess
import re
import sys


def run_adb_command(command):
    """Executes an ADB command and returns its output."""
    try:
        # We use a list of arguments for better security and handling
        result = subprocess.run(
            ["adb"] + command,
            capture_output=True,
            text=True,
            check=True,
            encoding='utf-8'
        )
        return result.stdout
    except FileNotFoundError:
        print("Error: 'adb' command not found.")
        print("Please ensure ADB is installed and in your system's PATH.")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error executing ADB command: {' '.join(command)}")
        print(f"Stderr: {e.stderr}")
        # Check for a common issue: device not found
        if "device not found" in e.stderr:
            print("\nError: No Android device found.")
            print("Please ensure your device is connected and USB debugging is enabled and authorized.")
        sys.exit(1)


def get_battery_properties():
    """
    Fetches and parses battery properties from the device using ADB.

    Returns:
        A dictionary containing battery properties or None if parsing fails.
    """
    print("Querying device for battery properties...")
    output = run_adb_command(["shell", "dumpsys", "battery"])

    properties = {}

    # Regex patterns to find the required values
    # Note: Some devices use different keys, these are the most common ones.
    patterns = {
        'capacity_mah': r'harge counter: (\d+)',  # This is often the full design capacity in µAh
        'voltage_mv': r'voltage: (\d+)',
        'temperature_c': r'temperature: (\d+)',
        'health_code': r'health: (\d+)',  # e.g., 2 for 'Good'
        'level': r'level: (\d+)',  # Current battery percentage
    }


    # Some devices report capacity differently, let's try another common key.
    # The 'charge counter' is often the design capacity in microampere-hours (µAh).
    # 'battery capacity' might be a fallback in mAh.
    alt_capacity_pattern = r'battery capacity: (\d+)'

    for key, pattern in patterns.items():
        match = re.findall(pattern, output)
        if match:
            properties[key] = int(match[-1])

    # Handle capacity specifically
    if 'capacity_mah' in properties:
        # The 'charge counter' is typically in microampere-hours (µAh), convert to mAh
        properties['capacity_mah'] = properties['capacity_mah'] / 1000
    else:
        # Try the alternative pattern if the primary one fails
        match = re.search(alt_capacity_pattern, output)
        if match:
            properties['capacity_mah'] = int(match.group(1))

    # Check if all necessary properties were found
    required_keys = ['voltage_mv', 'capacity_mah', 'health_code']
    if not all(key in properties for key in required_keys):
        print("\nError: Could not parse all required battery properties from the device.")
        print("The output of 'adb shell dumpsys battery' on your device may be different.")
        print("Please run the command manually to inspect its output.")
        return None

    # Convert health code to a percentage multiplier
    # Based on Android source constants: 2 corresponds to 'GOOD_HEALTH'
    # We'll assume 'Good' is 100% and anything else is degraded.
    # This is an estimation, as Android doesn't expose a direct health percentage.
    # A more sophisticated approach might involve tracking capacity over time.
    if properties['health_code'] == 2:  # HEALTH_GOOD
        properties['health_multiplier'] = 1.0  # 100%
    elif properties['health_code'] == 3:  # HEALTH_OVERHEAT
        properties['health_multiplier'] = 0.8  # Assume degraded
    elif properties['health_code'] == 4:  # HEALTH_DEAD
        properties['health_multiplier'] = 0.5  # Assume very degraded
    else:  # Others like UNKNOWN, COLD, etc.
        properties['health_multiplier'] = 0.9  # Default assumption for less than perfect health

    # Convert temperature to proper Celsius
    if 'temperature_c' in properties:
        properties['temperature_c'] /= 10.0

    return properties


def estimate_battery_drain(energy_joules):
    """
    Estimates battery drain percentage for a given energy consumption.
    """
    properties = get_battery_properties()
    if properties is None:
        return

    # --- 1. Get Device Battery Capacity (adjusted for health) ---
    design_capacity_mah = properties['capacity_mah']
    health_multiplier = properties['health_multiplier']
    current_voltage_v = properties['voltage_mv'] / 1000.0

    # Effective capacity is the design capacity reduced by the health factor
    effective_capacity_mah = design_capacity_mah * health_multiplier

    # --- 2. Calculate Total Battery Energy in Watt-hours (Wh) ---
    # Formula: Wh = (mAh * V) / 1000
    total_battery_energy_wh = (effective_capacity_mah * current_voltage_v) / 1000.0

    # --- 3. Convert Function's Energy Consumption to Watt-hours (Wh) ---
    # 1 Wh = 3600 Joules
    function_energy_wh = energy_joules / 3600.0

    # --- 4. Calculate the Percentage of Battery Consumed ---
    if total_battery_energy_wh == 0:
        print("Error: Calculated total battery energy is zero. Cannot divide.")
        return

    percentage_drained = (function_energy_wh / total_battery_energy_wh) * 100

    # --- 5. Display the Results ---
    print("\n--- Device Battery Analysis ---")
    print(f"  Design Capacity: {design_capacity_mah:.2f} mAh")
    print(f"  Current Voltage: {current_voltage_v:.3f} V")
    if 'temperature_c' in properties:
        print(f"  Temperature:     {properties['temperature_c']:.1f} °C")
    print(f"  Est. Health Multiplier: {health_multiplier:.2f} ({health_multiplier * 100:.0f}%)")
    print(f"  Effective Capacity: {effective_capacity_mah:.2f} mAh")
    print(f"  Total Battery Energy: {total_battery_energy_wh:.3f} Wh (Watt-hours)")
    print("---------------------------------")
    print(f"\n--- Consumption Calculation ---")
    print(f"  Function Energy Input: {energy_joules} J")
    print(f"  Function Energy (Wh):  {function_energy_wh:.6f} Wh")
    print("\n---------------------------------")
    print(f"Estimated Battery Drained: {percentage_drained:.6f}%")
    print("---------------------------------")


if __name__ == "__main__":
    # Check for command-line argument for energy in Joules
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <energy_in_joules>")
        sys.exit(1)

    try:
        joules = float(sys.argv[1])
        if joules < 0:
            raise ValueError("Energy must be a non-negative number.")
        estimate_battery_drain(joules)
    except ValueError as e:
        print(f"Invalid input: {e}. Please provide a valid number for joules.")
        sys.exit(1)