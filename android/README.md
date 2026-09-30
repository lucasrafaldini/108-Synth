# Android NDK Integration for 108 Synth 📱

This directory contains the native Android NDK integration for **108 Synth** using [Google Oboe](https://github.com/google/oboe) (AAudio / OpenSL ES) for ultra-low audio latency.

## Two Ways to Run on Android:

### Option 1: Instant PWA (Recommended for rapid testing)
1. Open the Web version in **Google Chrome for Android**.
2. Tap the three dots menu (⋮) -> **"Add to Home Screen"** / **"Install App"**.
3. 108 Synth will launch full-screen with offline support, touch multitouch piano, and Web MIDI support for USB-C and Bluetooth MIDI keyboards!

### Option 2: Native Android App (Google Oboe NDK)
For sub-10ms audio latency with native Android AAudio:
* CMake builds `src/dsp/Synth108Engine.h` directly with Oboe's audio callback stream.
* Works seamlessly with Android Studio and Gradle.

## Hacktoberfest Contributions for Android:
- [ ] Connect Android MIDI API (`android.media.midi`) to JNI `NoteOn` / `NoteOff`.
- [ ] Implement Jetpack Compose UI or Flutter frontend calling the C++ DSP via JNI / FFI.
- [ ] Optimize Oboe buffer sizes for low-latency performance modes (`PerformanceMode::LowLatency`).
