plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.example.myapplication"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.example.myapplication"
        minSdk = 24
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

    // ----------------------------------------------------
    // MERGE BENCHLIB ASSETS INTO *MAIN* APK ONLY
    // ----------------------------------------------------
    sourceSets {
        getByName("main") {
            assets.srcDirs(
                "src/main/assets",
                "../benchlib/assets"    // <-- this is the critical part
            )
        }
    }
    // ----------------------------------------------------

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

    // Duktape JS engine
    implementation(libs.duktape.android)

    // Your native and JS benchmark library
    implementation(project(":benchlib"))

    // Unit tests
    testImplementation(libs.junit)

    // Instrumentation tests
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)

    // Benchmarking libs
    androidTestImplementation("androidx.benchmark:benchmark-junit4:1.4.1")
    androidTestImplementation("androidx.benchmark:benchmark-macro-junit4:1.4.1")
}
