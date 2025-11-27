plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.example.myapplication"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.example.myapplication"
        minSdk = 26
        targetSdk = 36

        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        testApplicationId = "com.example.myapplication.test"
    }

    buildTypes {
        getByName("debug") {
            isDebuggable = true
        }
        getByName("release") {
            isMinifyEnabled = false
        }
        create("benchmark") {
            isDebuggable = true
            isProfileable = true
            matchingFallbacks += listOf("release")
        }
    }

    // ========= ASSET MERGING =========
    sourceSets {
        getByName("main") {
            assets.srcDirs(
                "src/main/assets",
                "../benchlib/assets"     // Your JS files from benchlib
            )
        }
        getByName("androidTest") {
            assets.srcDirs(
                "src/androidTest/assets"
            )
        }
    }
    // ==================================

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions {
        jvmTarget = "11"
    }

    buildFeatures {
        prefab = true
        buildConfig = true
    }
}

dependencies {
    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.core.ktx)

    // Your native benchmarks jni + JS files
    implementation(project(":benchlib"))

    // Unit tests
    testImplementation(libs.junit)

    // Instrumented Tests
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)

    // Android Benchmark Libraries (for profiling)
    androidTestImplementation("androidx.benchmark:benchmark-junit4:1.4.1")
    androidTestImplementation("androidx.benchmark:benchmark-macro-junit4:1.4.1")

    implementation("androidx.javascriptengine:javascriptengine:1.0.0")
    androidTestImplementation("androidx.javascriptengine:javascriptengine:1.0.0")

}
