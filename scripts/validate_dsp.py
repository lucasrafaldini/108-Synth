#!/usr/bin/env python3
"""
DSP Real-Time Safety & Syntax Validator for 108 Synth
Checks for illegal dynamic allocations (malloc, new, etc.) in DSP code
and verifies C++17 compilation.
"""

import os
import sys
import subprocess
import re

DSP_DIR = os.path.join(os.path.dirname(__file__), "..", "src", "dsp")
FORBIDDEN_CALLS = [
    r"\bmalloc\b",
    r"\bfree\b",
    r"\bnew\b",
    r"\bdelete\b",
    r"\bstd::vector::push_back\b",
    r"\bstd::cout\b",
    r"\bprintf\b",
    r"\bstd::mutex\b",
]

def check_realtime_safety():
    print("🔍 Checking real-time audio safety in src/dsp/...")
    violations = []

    for root, _, files in os.walk(DSP_DIR):
        for file in files:
            if file.endswith((".h", ".cpp")):
                filepath = os.path.join(root, file)
                with open(filepath, "r", encoding="utf-8") as f:
                    for line_num, line in enumerate(f, 1):
                        # Skip comments
                        clean_line = re.sub(r"//.*$", "", line)
                        for pattern in FORBIDDEN_CALLS:
                            if re.search(pattern, clean_line):
                                violations.append((file, line_num, pattern, line.strip()))

    if violations:
        print("⚠️ Potential real-time safety violations found:")
        for v in violations:
            print(f"  {v[0]}:{v[1]} - Forbidden token '{v[2]}': {v[3]}")
        return False
    else:
        print("✅ No dynamic allocations or blocking calls found in DSP loops.")
        return True

def compile_check():
    print("⚙️ Running C++17 syntax compilation check...")
    cmd = [
        "clang++", "-std=c++17", "-Wall", "-Wextra", "-fsyntax-only",
        "-x", "c++", os.path.join(DSP_DIR, "Synth108Engine.h"),
        "-x", "c++", os.path.join(DSP_DIR, "Preset.h"),
    ]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode == 0:
        print("✅ All C++17 DSP modules compiled with 0 errors!")
        return True
    else:
        print("❌ C++ Compilation errors:\n", res.stderr)
        return False

def main():
    rt_ok = check_realtime_safety()
    cp_ok = compile_check()

    if rt_ok and cp_ok:
        print("\n🎉 DSP validation PASSED! Ready for audio processing.")
        sys.exit(0)
    else:
        print("\n❌ DSP validation FAILED.")
        sys.exit(1)

if __name__ == "__main__":
    main()
