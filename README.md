
# GPC — Qt/Kotlin JNI Bridge (Android + Windows)

A cross-platform experimental project demonstrating a robust two-way bridge between C++ (Qt 6.8.x) and Kotlin/Android via JNI, plus Android-native features like interactive notifications and UDP audio streaming.

## Overview
- Goal: Reliable C++ ↔ Kotlin calls (JNI), shared logic across Android/Windows.
- Status: Stable bidirectional callbacks, interactive notifications, UDP audio server with adaptive buffering.
- Note: This is an educational/test project, not a production app.

## Features
- C++ → Kotlin and Kotlin → C++ callbacks (thread-safe) using `extern "C" JNIEXPORT` and Qt `QJniObject`.
- Interactive Android notifications with action buttons (BroadcastReceiver).
- UDP voice/audio streaming using `QUdpSocket` + `QAudioSource/QAudioSink` with adaptive buffer.
- Scoped storage-safe file access using `QStandardPaths::AppDataLocation`.
- Cross-platform build targets: Android (ARM) and Windows (x64).

## Architecture
- Qt/C++ core (UI + bridge) decoupled from Android platform code.
- Kotlin layer exposes a singleton bridge object (pre-loaded library) to avoid `dlopen` in isolated receivers.
- JNI methods registered once; Kotlin broadcasts call into the existing singleton to prevent `NoClassDefFoundError` and `dlopen failed`.

Suggested module layout:
- c++/jni/AndroidBridge.h/.cpp — JNI entry points and wrappers
- c++/core/MainWindow.h/.cpp — UI and C++ business logic
- android/kotlin/NotificationHelper.kt — building notifications
- android/kotlin/NotificationReceiver.kt — action handling
- android/kotlin/TestBridge.kt — singleton bridge

## Build Requirements
- Qt: 6.8.1 (Creator + Android kits)
- JDK: 17 LTS (Temurin/OpenLogic). Avoid JDK 21 with Gradle 7.x.
- Android SDK: API 34 (Android 14)
- NDK: r26d
- Gradle wrapper: 7.6 (or 8.7+ if you insist on JDK 21)

Gradle Java home (to force JDK 17):
```
org.gradle.java.home=C:\\Program Files\\Eclipse Adoptium\\jdk-17.0.11
```

## Build (Windows)
- Open in Qt Creator, select MSVC/MinGW kit.
- Configure, build, run.

## Build (Android)
- Tools → Options → Devices → Android: point JDK to 17 LTS.
- Select Android kit, build, and Deploy to device/emulator.
- Ensure `minSdkVersion` ≥ 23 and `targetSdkVersion` 34.

AndroidManifest permissions:
- INTERNET
- ACCESS_NETWORK_STATE
- ACCESS_WIFI_STATE

Multicast lock (required for broadcast/multicast receive):
- Acquire via `WifiManager.createMulticastLock()` (exposed through Kotlin and called from C++ JNI).

## Usage
- From C++: call Kotlin methods via `QJniObject::callMethod<...>()` on the singleton bridge.
- From Kotlin: call back into C++ using `external` functions registered in `JNI_OnLoad`.
- Trigger a notification action to test BroadcastReceiver → Kotlin → C++ callback chain.

## File Access & SQLite
- Use `QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)` for portable app data.
- Avoid direct external storage paths (Scoped Storage restrictions on API 30+).

## Foreground Service (Recommended)
- For reliable background UDP streaming, move Android-side long-running work to a ForegroundService to keep the process alive when screen turns off.

## Benchmark (TODO)
- Round-trip latency (C++ → Kotlin → C++): log timestamps and compute average.
- UDP audio end-to-end latency under varying packet sizes (e.g., 160, 320, 640 bytes PCM @ 8/16 kHz).

## Roadmap
- Modularize JNI into `c++/jni` and Kotlin platform layer into `android/kotlin`.
- Add unit tests for bridge calls (QTest + instrumented Android tests).
- Optional: integrate Opus codec for lower bandwidth and better quality.

## License
Educational/demo purposes. Choose MIT/Apache-2.0 for broader reuse.

## Credits
- Qt 6.8.x
- Android/Kotlin JNI
