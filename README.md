# 108 SYNTH 🎛️✨

> **Modern, Multiplatform Polyphonic Analog Virtual Synthesizer**  
> Runs in **DAWs (VST3, AU, AUv3, CLAP)**, **Web (Web Audio / Web MIDI)**, **macOS**, **iOS (iPad / iPhone)**, **Linux**, **Windows**, and **Android**.

[![Hacktoberfest 2026](https://img.shields.io/badge/Hacktoberfest-2026-orange.svg?style=for-the-badge&logo=hacktoberfest)](https://hacktoberfest.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](./LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=for-the-badge)](./CONTRIBUTING.md)
[![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Windows%20%7C%20Linux%20%7C%20iOS%20%7C%20Android%20%7C%20Web-purple.svg?style=for-the-badge)](#platforms)

---

## 🎹 Welcome to Hacktoberfest 2026! 🎃

**108 Synth** is an open-source synthesizer created by **Lucas Rafaldini** and open to contributors worldwide for **Hacktoberfest**!

Whether you are an audio DSP mathematician, a C++ hacker, a Web developer, a UI/UX designer, or a sound designer crafting patches, **there is a place for you to make an impact**.

👉 **Read our [Contribution Guidelines (CONTRIBUTING.md)](./CONTRIBUTING.md)** to get started!

---

## ⚡ The Vision

Traditional synth plugins are often locked to desktop DAWs or tied to proprietary frameworks. **108 Synth** is built with a **Decoupled Core DSP Architecture**:

```
                       ┌──────────────────────────────────────────────┐
                       │           108 SYNTH DSP ENGINE (C++17)       │
                       │  • PolyBLEP Oscillators (Saw, Pulse, Sub)    │
                       │  • Resonant 24dB Ladder/SVF Filter           │
                       │  • Dual Exponential ADSR (VCA & VCF)         │
                       │  • Stereo BBD Chorus (Juno-106 I, II, I+II)  │
                       │  • 16-Voice Polyphony & MIDI Dispatch        │
                       └──────────────────────┬───────────────────────┘
                                              │
         ┌──────────────────┬─────────────────┼──────────────────┬──────────────────┐
         ▼                  ▼                 ▼                  ▼                  ▼
┌─────────────────┐ ┌───────────────┐ ┌───────────────┐ ┌─────────────────┐ ┌───────────────┐
│   DESKTOP DAWs  │ │    iOS AUv3   │ │   WEB & PWA   │ │   ANDROID NDK   │ │   STANDALONE  │
│ VST3 / AU / CLAP│ │ GarageBand /  │ │ Web Audio /   │ │ Google Oboe     │ │ CoreAudio /   │
│ (Mac, Win, Lnx) │ │ Logic for iPad│ │ Web MIDI API  │ │ (AAudio NDK)    │ │ JACK / ALSA   │
└─────────────────┘ └───────────────┘ └───────────────┘ └─────────────────┘ └───────────────┘
```

The **exact same sound engine, presets, and audio characteristics** run anywhere music is made!

---

## 🔊 Sound Engine Features

* **Dual DCOs (Digitally Controlled Oscillators)**:
  * Band-limited **PolyBLEP** anti-aliased waveforms (Sawtooth, Pulse with PWM, Triangle, Sine).
  * Fine/coarse tuning, octave transpositions, and subtle analog drift.
* **Sub-Oscillator & Noise**:
  * Deep sub-bass square oscillator (-1 / -2 octaves).
  * Analog white noise generator for percussive attacks and air.
* **Resonant VCF (Voltage Controlled Filter)**:
  * 4-pole (24dB/oct) & 2-pole (12dB/oct) low-pass filter with non-linear hyperbolic tangent (`tanh`) saturation drive.
  * Self-oscillating resonance with analog warmth.
  * Key tracking and dedicated bipolar envelope modulation.
* **Dual ADSR Envelopes**:
  * Dedicated VCA (Amplitude) and VCF (Filter cutoff) envelopes.
  * Exponential curves for punchy basslines and musical pad swells.
* **Multi-Waveform LFO**:
  * Routes to pitch (vibrato), filter cutoff, and pulse width modulation (PWM).
* **Stereo BBD Chorus**:
  * Emulation of the iconic Roland Juno-106 bucket-brigade stereo chorus (Mode Off, Mode I, Mode II, and combined Mode I+II).
* **Polyphony & MIDI**:
  * 16-voice polyphonic voice allocator with smooth voice stealing.
  * Sample-accurate MIDI Note On/Off, Pitch Bend, Modulation Wheel, and Sustain Pedal.

---

## 🚀 Quickstart: Try It in 5 Seconds!

You don't need gigabytes of compiler SDKs to test **108 Synth** right now! The interactive web synthesizer is ready:

```bash
# 1. Clone the repository
git clone https://github.com/lucasrafaldini/108-Synth.git
cd 108-Synth/web

# 2. Start a local server
python3 -m http.server 8080

# 3. Open in your browser:
# http://localhost:8080
```

* 🎹 **Play** with your mouse, touch screen, or computer keyboard (`A`, `W`, `S`, `E`, `D`, `F`, `T`, `G`, `Y`, `H`, `U`, `J`, `K`).
* 🎛️ **Plug any USB or Bluetooth MIDI keyboard**: The Web MIDI API connects automatically!
* 📱 **Mobile & Android**: Open on Chrome for Android, tap "Add to Home Screen" to install it as a standalone touch synthesizer!

---

## 📂 Project Organization

```
108-Synth/
├── src/
│   └── dsp/                 # Core DSP Engine (Pure C++17, real-time safe)
│       ├── Synth108Engine.h # Voice allocator, parameter manager & stereo rendering
│       ├── Voice.h          # Polyphonic voice model
│       ├── PolyBLEPOsc.h    # Anti-aliased PolyBLEP oscillators
│       ├── LadderFilter.h   # Resonant 24dB / 12dB filter with drive
│       ├── ADSREnvelope.h   # Exponential ADSR envelope generator
│       ├── LFO.h            # Low Frequency Oscillator
│       ├── StereoChorus.h   # Roland Juno-106 BBD stereo chorus
│       └── Preset.h         # Factory preset library
├── Synth108/                # iPlug2 Plugin Layer (DAWs & Desktop)
│   ├── config.h             # Plugin configuration & channel I/O
│   ├── Synth108.h           # iPlug2 Instrument header
│   ├── Synth108.cpp         # Parameter mapping, MIDI and audio callbacks
│   └── projects/            # Xcode (macOS & iOS), Makefiles, VS projects
├── web/                     # Web & PWA Interactive Synthesizer
│   ├── index.html           # Synth UI interface
│   ├── synth.js             # Web Audio & Web MIDI engine
│   ├── style.css            # Retro 80s hardware styling
│   └── manifest.json        # PWA configuration
├── android/                 # Android Native Integration
│   ├── README.md            # Android NDK instructions
│   └── OboeSynthEngine.cpp  # Google Oboe C++ callback bridge
├── presets/                 # Factory Sound Bank
│   └── factory_presets.json # Community preset collection (JSON)
├── .github/                 # GitHub Templates & Workflows
│   ├── ISSUE_TEMPLATE/      # Bug reports, feature requests, preset submissions
│   └── PULL_REQUEST_TEMPLATE.md
├── CONTRIBUTING.md          # Hacktoberfest contributor guide
├── CODE_OF_CONDUCT.md       # Contributor Covenant v2.1
└── LICENSE                  # MIT License
```

---

## 🎃 Hacktoberfest Contribution Tracks

We welcome contributions across all skill levels:

| Track | Description | Languages / Tools |
|---|---|---|
| **🎛️ DSP & Audio** | New waveforms (Supersaw, FM), filter models (diode ladder, SVF), effects (Delay, Reverb, Distortion). | C++17 |
| **🎨 UI & Design** | Vintage faceplates, custom SVG/Canvas rotary knobs, oscilloscope improvements. | HTML / CSS / JS / iGraphics |
| **🌐 Web & PWA** | AudioWorklet WebAssembly porting, preset sharing via URL hash, offline cache. | Web Audio / WASM / JS |
| **📱 Mobile & DAWs** | Android Oboe low-latency tuning, AUv3 testing in GarageBand & AUM, CLAP plugin support. | C++ / Swift / Android NDK |
| **🎹 Sound Design** | Craft new factory patches (Basses, Leads, Pads, Arps) and submit via JSON. | Any (ears & music taste!) |
| **📝 Documentation** | Beginners' guides to synthesis, video tutorials, localization/translations. | Markdown |

Check out the **[Good First Issues](https://github.com/lucasrafaldini/108-Synth/issues)** or open your own proposal!

---

## 🛠️ Building Native Plugins (DAWs)

### Requirements:
* **macOS / iOS**: Xcode 14+ with command line tools.
* **Windows**: Visual Studio 2022.
* **Linux**: GCC 10+ / Clang, JACK/ALSA dev libraries.

```bash
# Clone with submodules
git clone --recursive https://github.com/lucasrafaldini/108-Synth.git
cd 108-Synth

# Initialize iPlug2 dependencies
./setup_container.sh

# Open Xcode workspace for macOS:
open Synth108/projects/Synth108-macOS.xcodeproj
# or for iOS AUv3:
open Synth108/projects/Synth108-iOS.xcodeproj
```

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](./LICENSE) file for details.  
Built upon the rock-solid foundation of the [iPlug2](https://github.com/iPlug2/iPlug2) framework (zlib license).

---

## 🌟 Stargazers & Community

If you love analog synthesizers and open-source audio, please give **108 Synth** a ⭐️ star on GitHub!  
Happy hacking and happy music making! 🎶
