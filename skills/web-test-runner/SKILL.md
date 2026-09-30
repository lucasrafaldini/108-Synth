---
name: web-test-runner
description: Runs and tests the Web Audio interactive synthesizer playground on local servers and checks console errors.
---

# Web Test Runner Skill 🌐

Use this skill to test sound generation, Web MIDI connectivity, and responsive UI controls in the web browser.

## How to Test
1. Start the local server:
   ```bash
   cd web
   python3 -m http.server 8080
   ```
2. Navigate to `http://localhost:8080`.
3. Check browser console for Web Audio context errors or missing asset 404s.
4. Verify audio output on click/touch/keyboard note trigger.
