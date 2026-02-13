# Android Energy Efficiency: Programming Language Benchmarking

![Android](https://img.shields.io/badge/Platform-Android-green)
![Languages](https://img.shields.io/badge/Languages-5-blue)
![License](https://img.shields.io/badge/License-MIT-yellow)

**NYUAD Computer Science Capstone Project**

A systematic empirical study comparing energy consumption and runtime performance across five programming languages commonly used in Android development: Java, Kotlin, C, C++, and JavaScript.

## 📄 Research Paper

**[Read the Full Capstone Report](./docs/NYUAD_CS_CapstoneReport_Naya_AlAfifi.pdf)**

*Title:* Ranking Android Development Languages by Energy Efficiency  
*Author:* Naya Al Afifi  
*Advisors:* João Paulo Fernandes, Karim Ali  
*Institution:* New York University Abu Dhabi

## 🎯 Project Overview

Mobile devices operate under strict battery constraints, making energy efficiency a critical factor in application development. This project investigates whether programming language choice significantly impacts energy consumption and runtime performance on Android devices.

### Research Questions

1. **RQ1:** Does programming language choice significantly affect runtime under identical computational workloads?
2. **RQ2:** Do specific languages perform better for CPU-bound tasks versus memory-bound tasks on Android?
3. **RQ3:** Is there a consistent performance and energy ranking across languages?

## Methodology

### Benchmarks
Six standardized benchmarks from The Computer Language Benchmarks Game (TCLBG):

**CPU-Intensive:**
- Mandelbrot (floating-point computation)
- SpectralNorm (numerical linear algebra)
- FannkuchRedux (combinatorial permutations)

**Memory-Intensive:**
- BinaryTrees (allocation-heavy tree construction)
- Fasta (string generation)
- RevComp (string manipulation)

### Implementation
- **Java & Kotlin:** Executed on Android Runtime (ART)
- **C & C++:** Compiled via Android NDK, invoked through JNI
- **JavaScript:** Executed via Jetpack JavaScriptEngine

### Measurement Tools
- **Energy Profiling:** E-MANAFA (Perfetto-based tracing)
- **Execution:** JUnit instrumentation tests
- **Device:** Pixel 9 Pro XL
- **Sample Size:** 10 runs per language per benchmark (300 total executions)

### Statistical Analysis
- Normality testing (Shapiro-Wilk)
- Variance testing (Levene's test)
- Omnibus tests (ANOVA, Welch ANOVA, Kruskal-Wallis)
- Post-hoc comparisons (Tukey HSD, Dunn's test with Bonferroni correction)

## Key Findings

### Energy Efficiency
- **Java and Kotlin** showed statistical energy equivalence across all six benchmarks
- **Native languages (C/C++)** excelled in CPU-bound tasks (2-3× faster)
- **JVM languages** outperformed native code in allocation-heavy workloads (2× faster)
- **JavaScript** consistently consumed 2-7× more energy than optimal languages

### Performance Patterns
- **Workload-dependent optimization:** No single language dominates across all task types
- **Allocation efficiency:** Java/Kotlin's garbage collector significantly outperforms manual memory management for short-lived objects
- **Computational intensity:** C achieves best performance in floating-point and array-intensive operations
- **String manipulation:** Native languages (C/C++) show 2.5-2.8× advantage over JVM languages

### Battery Impact
Real battery drain measurements showed language choice can affect device usage time:
- BinaryTrees: 0.138% (Java/Kotlin) vs 0.282% (C++)
- FannkuchRedux: 0.150% (C/C++) vs 0.650% (JavaScript)
- Mandelbrot: 0.147% (C) vs 0.558% (JavaScript)

## Technologies Used

- **Languages:** Java, Kotlin, C, C++, JavaScript
- **Android Tools:** Android NDK, JUnit, ADB, Perfetto
- **Profiling:** E-MANAFA, Batterystats
- **Statistical Analysis:** Python (SciPy, Pandas, NumPy)
- **Automation:** Bash scripting, Python automation framework

## Running the Benchmarks

### Prerequisites
```bash
- Android Studio
- Android NDK installed
- Python 3.8+
- E-MANAFA profiling tool
- Physical Android device (API 24+)
```

### Setup
```bash
# Clone the repository
git clone https://github.com/nayalafifi/EnergyBenchmarkingCapstone.git
cd EnergyBenchmarkingCapstone

# Install dependencies
pip install -r requirements.txt

# Build the project
./gradlew build
```

### Execution
```bash
# Run automated benchmark suite
python automation/run_benchmarks.py

# Run individual benchmark
./gradlew connectedAndroidTest -Pandroid.testInstrumentationRunnerArguments.class=com.example.benchmarks.MandelbrotTest
```

## Results & Visualizations

The study generated comprehensive visualizations including:
- Execution time and energy consumption comparisons
- Box plots showing measurement distributions
- Normalized performance heatmaps
- Battery drain analysis across languages
- Statistical significance matrices

See the [full paper](./docs/NYUAD_CS_CapstoneReport_Naya_AlAfifi.pdf) for detailed results and visualizations.

## Practical Implications

### For Android Developers:

1. **Default to Java/Kotlin** for general-purpose development
   - Energy equivalent across all workloads
   - Excellent toolchain support
   - Suitable for most applications

2. **Use C/C++ selectively** for computation-heavy hotspots
   - Image processing, signal processing
   - Numerical computations, physics simulations
   - Native performance advantages can exceed 2-3×

3. **Avoid JavaScript** for intensive computation
   - 2-7× worse energy consumption
   - Unsuitable for performance-critical operations
   - Better for lightweight scripting tasks

4. **Profile before optimizing**
   - Task-dependent behavior means assumptions can be wrong
   - Measure actual device-level energy use
   - Consider development velocity vs. performance gains

## References

Based on research including:
- Pereira et al. (2017) - Energy efficiency across programming languages
- Chen & Zong (2016) - Android app energy efficiency
- Green Software Lab - Energy-Benchmark-Languages-Android
- Rua & Saraiva (2023) - E-MANAFA profiling tool

## Contributing

This was a capstone research project, but feedback and suggestions are welcome! Feel free to:
- Open issues for questions or discussions
- Suggest additional benchmarks or languages
- Share replication results on different devices

## Contact

**Naya Al Afifi**  
Computer Science, NYUAD  
na3266@nyu.edu

## License

MIT License - See LICENSE file for details

## Acknowledgments

Special thanks to:
- **Advisors:** João Paulo Fernandes and Karim Ali
- **Green Software Lab** for benchmark specifications
- **NYUAD Computer Science Department**

---

*This project was completed as part of NYUAD's Computer Science major graduation requirements (Fall 2025).*
