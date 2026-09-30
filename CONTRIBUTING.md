# Contributing to 108 Synth 🎛️✨

First off, welcome! Whether you are a seasoned audio DSP developer, a front-end UI craftsperson, a sound designer, or a newcomer participating in your very first **Hacktoberfest**, we are thrilled to have you here!

---

## 🎃 Hacktoberfest Participation

**108 Synth** proudly participates in [Hacktoberfest](https://hacktoberfest.com/)! 

### Hacktoberfest Guidelines:
* **Quality over Quantity**: We value meaningful, working contributions. PRs that only fix minor typos or reformat whitespace without substance will be closed and marked as `invalid`/`spam`.
* **Issue First**: For new features or significant changes, please open or comment on an issue first to discuss the implementation before submitting a PR.
* **Tagging**: Once your PR is reviewed and meets our standards, we will label it with `hacktoberfest-accepted` (or merge it) so it counts towards your Hacktoberfest completion!

---

## 🚀 Where You Can Contribute

We have divided the project into several tracks to match different skill levels and interests:

### 1. 🎛️ DSP & Audio Engine (`src/dsp/` in C++17)
* **New Oscillator Waveforms**: Add PolyBLEP anti-aliased Triangle, Supersaw, FM operator, or Wavetable playback.
* **Filter Types**: Implement resonant 12dB/24dB State Variable Filter (SVF), diode ladder, comb filter, or vocal formant filter.
* **Audio Effects**: Add Stereo Delay with ping-pong, algorithmic Reverb, Phaser, Flanger, Bitcrusher, or Overdrive/Distortion.
* **Modulation & Control**: Pitch Bend range configuration, Portamento/Glide modes, Velocity sensitivity curves, MPE (MIDI Polyphonic Expression) support.
* **Optimization**: SIMD vectorization (SSE/AVX/NEON) for voice processing.

### 2. 🎨 User Interface & Experience
* **Knobs & Sliders**: Responsive, touch-friendly, high-DPI canvas/SVG controls.
* **Themes / Skins**: Vintage 80s Roland Juno-106 theme, Dark Cyberpunk, High-Contrast accessibility theme.
* **Visualizers**: Real-time oscilloscope, spectrum analyzer, or envelope curve visualizer.
* **Accessibility (a11y)**: Keyboard navigation, screen-reader friendly ARIA labels for synth parameters.

### 3. 🌐 Web & PWA (`web/`)
* **AudioWorklet DSP**: Porting or loading the C++ core via WebAssembly (`emscripten`) into AudioWorklet.
* **Web MIDI API**: Plug-and-play USB & Bluetooth MIDI keyboard auto-detection.
* **PWA Features**: Offline caching (Service Worker), install prompt, full-screen touch keyboard for mobile/tablets.
* **Preset Import/Export**: Save and load patches as JSON files or share via URL hash.

### 4. 📱 Mobile & DAW Integrations
* **Android (NDK + Oboe)**: Building an ultra-low latency AAudio/OpenSL ES wrapper with Google Oboe.
* **iOS (AUv3)**: Validating and polishing the AUv3 extension for GarageBand, Logic Pro for iPad, and AUM.
* **DAW Plugin Testing**: Testing VST3, AU, and CLAP compatibility across Reaper, Ableton Live, FL Studio, Bitwig, Cubase, etc.

### 5. 🎹 Sound Design & Presets (`presets/`)
* Create signature factory presets:
  * 🎸 Iconic Synthwave & Retrowave Basses
  * 🎺 Vintage Poly Brass & Strings
  * 🚀 Sci-Fi Sound Effects & Risers
  * 🎹 Dreamy 80s Chorused Keys & Pads
  * ⚡ Punchy Plucks & Leads
* Submit your preset file with author attribution!

### 6. 📝 Documentation & Localization
* Translate documentation and UI into Portuguese, Spanish, Japanese, German, etc.
* Write beginners' guides on subtractive synthesis using 108 Synth.
* Record short video demos or audio samples showcasing presets.

---

## 🛠️ Development Setup

### Quick Web Testing (No heavy toolchains needed!)
You don't need Xcode or Visual Studio to test the synth! The Web playground runs anywhere with Python:

```bash
cd web
python3 -m http.server 8080
# Open http://localhost:8080 in your browser (Chrome, Firefox, Safari, Edge)
```

### Native Desktop & DAWs (macOS / iOS)
Requires Xcode and command line tools:
```bash
# Initialize submodules
git submodule update --init

# Open Xcode projects:
open Synth108/projects/Synth108-macOS.xcodeproj
# or for iOS:
open Synth108/projects/Synth108-iOS.xcodeproj
```

---

## 📦 Submission Process

1. **Fork** the repository on GitHub.
2. **Clone** your fork locally:
   ```bash
   git clone https://github.com/<your-username>/108-Synth.git
   cd 108-Synth
   ```
3. **Create a branch** for your work:
   ```bash
   git checkout -b feature/my-cool-feature
   # or
   git checkout -b fix/issue-description
   ```
4. **Make your changes** cleanly and test thoroughly.
5. **Commit** with clear messages following Conventional Commits (e.g. `feat(dsp): add polyblep triangle oscillator`, `fix(ui): correct knob drag sensitivity on touch devices`).
6. **Push** to your fork:
   ```bash
   git push origin feature/my-cool-feature
   ```
7. Open a **Pull Request** against `master` using our PR template!

---

## 📜 Code Style Guidelines

* **C++**: Modern C++17. Follow clean, readable code formatting (`clang-format`).
* **Real-time Audio Safety**: In DSP code (`ProcessBlock`, `Voice::Process`):
  * ❌ **NO dynamic memory allocations** (`new`, `malloc`, resizing vectors).
  * ❌ **NO blocking calls** (file I/O, network, mutex locks).
  * ❌ **NO exceptions**.
* **JavaScript / Web**: Modern vanilla ES6+, zero heavy external dependencies where possible.

---

## 💬 Questions & Community

Have a question or need guidance?
* Open a **GitHub Discussion** or comment on an existing issue.
* Reach out to the maintainers at `lrafaldini@pm.me`.

Thank you for helping make **108 Synth** an amazing open-source synthesizer for everyone! 🎵
