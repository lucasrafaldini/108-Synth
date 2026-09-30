# 108 Synth - AI Agent & Developer Guide 🎛️🤖

Welcome to the **108 Synth** codebase. This repository is built to be fully **agentic** — designed for AI coding assistants (Claude Code, Antigravity, Cursor, Windsurf, GitHub Copilot) and human contributors to collaborate seamlessly.

---

## 🏛️ Architecture Overview

The codebase is organized in decoupled layers to maximize portability across DAWs, Web, iOS, Android, and desktop:

```
108-Synth/
├── src/
│   └── dsp/                 # Core DSP Engine (Pure C++17, real-time audio safe)
│       ├── Synth108Engine.h # Voice allocator, parameter manager & stereo rendering
│       ├── Voice.h          # Polyphonic voice model (Dual Osc + Sub + Noise + VCF + ADSRs)
│       ├── PolyBLEPOsc.h    # Band-limited PolyBLEP oscillators
│       ├── LadderFilter.h   # Resonant 24dB / 12dB filter with tanh drive
│       ├── ADSREnvelope.h   # Exponential ADSR envelope generator
│       ├── LFO.h            # Low Frequency Oscillator
│       ├── StereoChorus.h   # Roland Juno-106 BBD stereo chorus emulation
│       └── Preset.h         # Factory preset library
├── Synth108/                # iPlug2 Layer (DAWs VST3, AU, AUv3, CLAP, Standalone)
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
├── presets/                 # Factory Sound Bank (JSON)
│   └── factory_presets.json # Community preset collection
├── .agents/                 # Specialized Agent Roles
│   ├── dsp-engineer.md      # DSP & Audio Math specialist
│   ├── ui-designer.md       # Front-end & Audio UI specialist
│   ├── sound-designer.md    # Preset & Synthesis specialist
│   └── platform-integrator.md # DAW & Mobile specialist
├── skills/                  # Autonomous Agent Skills (SKILL.md)
│   ├── dsp-validator/       # Real-time safety and C++ syntax checker
│   ├── preset-builder/      # Patch generator and JSON/C++ synchronizer
│   └── web-test-runner/     # Web synth preview runner
└── CONTRIBUTING.md          # Hacktoberfest contributor guidelines
```

---

## ⚡ Real-Time Audio Golden Rules (Strict C++ DSP Constraints)

When working on any file inside `src/dsp/` or in audio callbacks:

1. ❌ **NO Dynamic Memory Allocations**: Never call `malloc`, `free`, `new`, `delete`, or methods that resize memory (e.g. `std::vector::push_back`, `std::string` concatenation) on the audio rendering thread (`ProcessBlock`, `Voice::Process`, etc.).
2. ❌ **NO Blocking Operations / Locks**: Never use `std::mutex`, file I/O (`std::cout`, `printf`, file reading), network calls, or sleep inside the audio thread.
3. ❌ **NO Exceptions**: Never throw or catch exceptions inside the DSP processing chain.
4. ✅ **Sample-Rate Invariance**: Always multiply rates and time by `1.0 / sampleRate`.
5. ✅ **Denormal Protection**: Guard against audio denormals by clamping tiny floating-point numbers or adding a small bias offset when calculating recursive filters.
6. ✅ **Anti-Aliasing**: Oscillators must use band-limiting techniques (such as PolyBLEP) rather than naive hard-edge digital transitions.

---

## 🛠️ Common Commands

### C++ DSP Syntax & Compilation Check
```bash
# Verify C++ DSP syntax (clean C++17)
clang++ -std=c++17 -Wall -Wextra -fsyntax-only -x c++ src/dsp/Synth108Engine.h -x c++ src/dsp/Preset.h -x c++ android/OboeSynthEngine.cpp
```

### Validate Presets & JSON Files
```bash
python3 -c "import json; json.load(open('presets/factory_presets.json')); json.load(open('web/manifest.json')); print('All JSON files valid!')"
```

### Run the Interactive Web Synth Playground
```bash
cd web
python3 -m http.server 8080
# Open http://localhost:8080 in Chrome, Safari, or Firefox
```

### Open Xcode Projects (macOS & iOS AUv3)
```bash
# macOS VST3 / AU / Standalone:
open Synth108/projects/Synth108-macOS.xcodeproj

# iOS AUv3 / iPad GarageBand:
open Synth108/projects/Synth108-iOS.xcodeproj
```

---

## 🤖 Agent Roles (`.agents/`)

When assigning tasks or acting on this repository, adopt one of the specialized roles:

* **DSP Engineer (`.agents/dsp-engineer.md`)**: Responsible for filters, oscillators, envelope curves, delay lines, and SIMD optimizations.
* **UI/UX Designer (`.agents/ui-designer.md`)**: Responsible for HTML/CSS canvas components, vintage hardware aesthetics, responsiveness, and touch UX.
* **Sound Designer (`.agents/sound-designer.md`)**: Responsible for sonic curation, crafting patches, and structuring preset banks.
* **Platform Integrator (`.agents/platform-integrator.md`)**: Responsible for VST3/AU/AUv3 plumbing, CMake, Android Oboe NDK, and WebAssembly builds.

---

## 🧭 Hacktoberfest Protocol for Agents

When helping human contributors during Hacktoberfest:
1. Always guide them toward an open issue from the **[Issues list](https://github.com/lucasrafaldini/108-Synth/issues)**.
2. Ensure their contributions adhere to `CONTRIBUTING.md` and pass the DSP validator skill.
3. Help contributors write clear, friendly PR descriptions using `.github/PULL_REQUEST_TEMPLATE.md`.
