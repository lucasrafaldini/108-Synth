---
name: dsp-validator
description: Validates real-time audio safety and compiles C++17 DSP modules for 108 Synth. Use whenever modifying or reviewing files in src/dsp/.
---

# DSP Validator Skill 🎛️

Use this skill whenever editing or reviewing audio code inside `src/dsp/` to ensure real-time thread safety and strict C++17 compliance.

## When to Use
- After modifying oscillators, filters, envelopes, effects, or voice allocation code.
- During PR review of any Hacktoberfest contribution labeled `dsp`.

## Execution Steps
1. Run the validator script:
   ```bash
   python3 scripts/validate_dsp.py
   ```
2. If violations are reported:
   - Replace any dynamic allocations (`new`, `malloc`, `vector::resize`) with static or pre-allocated fixed buffers.
   - Remove blocking locks (`std::mutex`) and replace with lock-free atomic values.
   - Remove any logging or file I/O from inner processing loops.
3. Verify that the exit code is `0`.
