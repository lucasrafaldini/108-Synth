#pragma once

#include "Synth108Engine.h"
#include <string>
#include <vector>

namespace Synth108 {

struct Preset {
    std::string name;
    std::string category;
    std::string author;
    SynthParams params;
};

inline std::vector<Preset> GetFactoryPresets() {
    std::vector<Preset> presets;

    // 1. 108 Init Lead
    {
        Preset p;
        p.name = "108 Init Lead";
        p.category = "Lead";
        p.author = "Lucas Rafaldini";
        p.params.osc1Waveform = 0; // Saw
        p.params.osc1Level = 0.9;
        p.params.osc2Waveform = 0; // Saw
        p.params.osc2DetuneSemi = 0.08;
        p.params.osc2Level = 0.6;
        p.params.subLevel = 0.2;
        p.params.filterCutoffHz = 4000.0;
        p.params.filterResonance = 0.25;
        p.params.filterEnvAmount = 2500.0;
        p.params.ampAttack = 0.005;
        p.params.ampDecay = 0.3;
        p.params.ampSustain = 0.8;
        p.params.ampRelease = 0.3;
        p.params.chorusMode = 1; // Chorus I
        p.params.chorusMix = 0.5;
        presets.push_back(p);
    }

    // 2. Juno 106 Bass
    {
        Preset p;
        p.name = "Juno 106 Bass";
        p.category = "Bass";
        p.author = "Lucas Rafaldini";
        p.params.osc1Waveform = 1; // Pulse/Square
        p.params.osc1PulseWidth = 0.5;
        p.params.osc1Octave = -12.0;
        p.params.osc1Level = 0.8;
        p.params.osc2Waveform = 0; // Saw
        p.params.osc2Octave = -12.0;
        p.params.osc2DetuneSemi = 0.04;
        p.params.osc2Level = 0.7;
        p.params.subLevel = 0.7; // Thick sub
        p.params.filterCutoffHz = 650.0;
        p.params.filterResonance = 0.4;
        p.params.filterEnvAmount = 1800.0;
        p.params.fltAttack = 0.002;
        p.params.fltDecay = 0.22;
        p.params.fltSustain = 0.0;
        p.params.ampAttack = 0.002;
        p.params.ampDecay = 0.35;
        p.params.ampSustain = 0.4;
        p.params.ampRelease = 0.15;
        p.params.chorusMode = 1; // Chorus I
        p.params.chorusMix = 0.65;
        presets.push_back(p);
    }

    // 3. Poly Brass 80s
    {
        Preset p;
        p.name = "Poly Brass 80s";
        p.category = "Brass";
        p.author = "Lucas Rafaldini";
        p.params.osc1Waveform = 0; // Saw
        p.params.osc1Level = 0.8;
        p.params.osc2Waveform = 0; // Saw
        p.params.osc2DetuneSemi = 0.12;
        p.params.osc2Level = 0.8;
        p.params.subLevel = 0.1;
        p.params.filterCutoffHz = 1200.0;
        p.params.filterResonance = 0.3;
        p.params.filterEnvAmount = 3500.0;
        p.params.fltAttack = 0.08;
        p.params.fltDecay = 0.5;
        p.params.fltSustain = 0.4;
        p.params.ampAttack = 0.04;
        p.params.ampDecay = 0.4;
        p.params.ampSustain = 0.7;
        p.params.ampRelease = 0.4;
        p.params.chorusMode = 2; // Chorus II
        p.params.chorusMix = 0.75;
        presets.push_back(p);
    }

    // 4. Dreamy Juno Pad
    {
        Preset p;
        p.name = "Dreamy Juno Pad";
        p.category = "Pad";
        p.author = "Lucas Rafaldini";
        p.params.osc1Waveform = 0; // Saw
        p.params.osc1Level = 0.7;
        p.params.osc2Waveform = 1; // Pulse/PWM
        p.params.osc2PulseWidth = 0.4;
        p.params.osc2DetuneSemi = 0.07;
        p.params.osc2Level = 0.7;
        p.params.subLevel = 0.3;
        p.params.filterCutoffHz = 1600.0;
        p.params.filterResonance = 0.35;
        p.params.filterEnvAmount = 1400.0;
        p.params.fltAttack = 0.6;
        p.params.fltDecay = 1.0;
        p.params.fltSustain = 0.6;
        p.params.fltRelease = 1.2;
        p.params.ampAttack = 0.5;
        p.params.ampDecay = 0.8;
        p.params.ampSustain = 0.8;
        p.params.ampRelease = 1.5;
        p.params.lfoRateHz = 0.8;
        p.params.lfoToPWM = 0.3;
        p.params.chorusMode = 3; // Chorus I+II
        p.params.chorusMix = 0.85;
        presets.push_back(p);
    }

    // 5. Acid 303 Reso
    {
        Preset p;
        p.name = "Acid 303 Reso";
        p.category = "Bass";
        p.author = "Lucas Rafaldini";
        p.params.osc1Waveform = 0; // Saw
        p.params.osc1Level = 1.0;
        p.params.osc2Level = 0.0;
        p.params.subLevel = 0.0;
        p.params.filterCutoffHz = 450.0;
        p.params.filterResonance = 0.85; // High resonance
        p.params.filterEnvAmount = 4500.0;
        p.params.fltAttack = 0.001;
        p.params.fltDecay = 0.22;
        p.params.fltSustain = 0.0;
        p.params.ampAttack = 0.001;
        p.params.ampDecay = 0.28;
        p.params.ampSustain = 0.2;
        p.params.ampRelease = 0.08;
        p.params.chorusMode = 0; // Dry
        presets.push_back(p);
    }

    return presets;
}

} // namespace Synth108
