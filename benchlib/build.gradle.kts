plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.example.benchlib"
    compileSdk = 36

    defaultConfig {
        minSdk = 24
        targetSdk = 36
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        consumerProguardFiles("consumer-rules.pro")

        // ✅ Enable native (C/C++) builds
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17"
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    // ✅ Explicit folder structure (for Java, Kotlin, assets, and C++)
    sourceSets {
        named("main") {
            java.srcDirs("src/main/java", "src/main/kotlin")
            assets.srcDirs("src/main/assets")
            res.srcDirs("src/main/res")
            manifest.srcFile("src/main/AndroidManifest.xml")
            jniLibs.srcDirs("src/main/jniLibs")
            jni.srcDirs("src/main/cpp")
        }
    }

    // ✅ Include your native CMake build
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    // ✅ Merge assets with app (so app can read JS files from benchlib)
    packaging {
        resources {
            excludes += setOf("META-INF/LICENSE*", "META-INF/NOTICE*")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions {
        jvmTarget = "11"
    }

    buildFeatures {
        buildConfig = true
    }
}

dependencies {
    // Core Android libraries
    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.core.ktx)

    // ✅ JavaScript engine for JS benchmarks
    implementation("com.squareup.duktape:duktape-android:1.4.0")

    // ✅ WebView (if you choose to execute JS benchmarks using a WebView context)
    implementation("androidx.webkit:webkit:1.9.0")

    // Testing dependencies
    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)
}
