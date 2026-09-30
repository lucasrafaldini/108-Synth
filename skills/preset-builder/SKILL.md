---
name: preset-builder
description: Generates, formats, and synchronizes new synthesizer presets across JSON and C++ headers. Use when creating or modifying factory presets.
---

# Preset Builder Skill 🎹

Use this skill when designing new sound patches, adding factory presets, or reviewing preset PRs.

## Preset Data Structure
Every preset must be synchronized across:
1. `presets/factory_presets.json` (for Web, documentation, and external sound designers).
2. `src/dsp/Preset.h` (compiled directly into the native C++ DSP core).

## Standard Preset Schema:
```json
{
  "name": "Preset Title",
  "category": "Bass | Lead | Pad | Brass | Pluck | FX",
  "author": "Author Name",
  "description": "Short musical description",
  "params": {
    "osc1Waveform": "Sawtooth | Pulse | Triangle | Sine",
    "osc1Level": 0.8,
    "osc2Waveform": "Sawtooth | Pulse | Triangle | Sine",
    "osc2DetuneCents": 6.0,
    "osc2Level": 0.7,
    "subLevel": 0.3,
    "noiseLevel": 0.0,
    "filterCutoffHz": 2000.0,
    "filterResonance": 0.3,
    "filterEnvAmount": 2500.0,
    "ampAttack": 0.01,
    "ampDecay": 0.3,
    "ampSustain": 0.8,
    "ampRelease": 0.3,
    "chorusMode": "Off | I | II | I+II",
    "chorusMix": 0.7
  }
}
```

## Validation
After modifying presets, validate JSON validity:
```bash
python3 -c "import json; json.load(open('presets/factory_presets.json')); print('JSON valid!')"
```
