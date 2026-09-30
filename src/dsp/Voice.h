#pragma once

#include "PolyBLEPOsc.h"
#include "ADSREnvelope.h"
#include "LadderFilter.h"
#include <cmath>

namespace Synth108 {

/**
 * @brief Polyphonic Synthesizer Voice
 * Contains dual oscillators, sub-oscillator, noise generator,
 * resonant filter, and dedicated VCA / VCF ADSR envelopes.
 */
class Voice {
public:
    Voice() = default;

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate;
        mOsc1.SetSampleRate(sampleRate);
        mOsc2.SetSampleRate(sampleRate);
        mSubOsc.SetSampleRate(sampleRate);
        mSubOsc.SetWaveform(PolyBLEPOsc::Waveform::SubOsc);
        mNoiseOsc.SetSampleRate(sampleRate);
        mNoiseOsc.SetWaveform(PolyBLEPOsc::Waveform::Noise);
        mFilter.SetSampleRate(sampleRate);
        mAmpEnv.SetSampleRate(sampleRate);
        mFilterEnv.SetSampleRate(sampleRate);
    }

    void NoteOn(int noteNumber, double velocity, double osc1Tune, double osc2Detune) {
        mNoteNumber = noteNumber;
        mVelocity = velocity;
        mBaseFreq = MidiNoteToHz(noteNumber);

        UpdateFrequencies(osc1Tune, osc2Detune, 0.0);

        mAmpEnv.NoteOn();
        mFilterEnv.NoteOn();
    }

    void NoteOff() {
        mAmpEnv.NoteOff();
        mFilterEnv.NoteOff();
    }

    void Kill() {
        mAmpEnv.Reset();
        mFilterEnv.Reset();
        mFilter.Reset();
        mNoteNumber = -1;
    }

    bool IsActive() const {
        return mAmpEnv.IsActive();
    }

    int GetNoteNumber() const {
        return mNoteNumber;
    }

    void UpdateFrequencies(double osc1TuneSemi, double osc2DetuneSemi, double pitchModSemi) {
        double f1 = mBaseFreq * std::pow(2.0, (osc1TuneSemi + pitchModSemi) / 12.0);
        double f2 = mBaseFreq * std::pow(2.0, (osc2DetuneSemi + pitchModSemi) / 12.0);

        mOsc1.SetFrequency(f1);
        mOsc2.SetFrequency(f2);
        mSubOsc.SetFrequency(f1 * 0.5); // Sub-osc follows Osc1
    }

    double Process(
        double osc1Level,
        double osc2Level,
        double subLevel,
        double noiseLevel,
        double baseCutoffHz,
        double resonance,
        double filterEnvAmount,
        double filterKeyFollow,
        double lfoFilterMod
    ) {
        if (!IsActive()) return 0.0;

        // 1. Generate oscillator waveforms
        double s1 = mOsc1.Process() * osc1Level;
        double s2 = mOsc2.Process() * osc2Level;
        double sub = mSubOsc.Process() * subLevel;
        double noise = mNoiseOsc.Process() * noiseLevel;

        double mixedOsc = s1 + s2 + sub + noise;

        // 2. Process envelopes
        double ampEnvVal = mAmpEnv.Process();
        double fltEnvVal = mFilterEnv.Process();

        // 3. Compute dynamic filter cutoff
        // Key tracking: centers at MIDI note 60 (Middle C)
        double keyTrackFactor = (mNoteNumber - 60) * filterKeyFollow * 100.0;
        double modCutoff = baseCutoffHz + (fltEnvVal * filterEnvAmount) + keyTrackFactor + lfoFilterMod;
        mFilter.SetCutoff(modCutoff);
        mFilter.SetResonance(resonance);

        // 4. Run through resonant ladder filter
        double filtered = mFilter.Process(mixedOsc);

        // 5. Apply VCA amplitude envelope and note velocity
        double velocityScale = 0.3 + (0.7 * mVelocity);
        return filtered * ampEnvVal * velocityScale;
    }

    // Direct accessors for setup
    PolyBLEPOsc mOsc1;
    PolyBLEPOsc mOsc2;
    LadderFilter mFilter;
    ADSREnvelope mAmpEnv;
    ADSREnvelope mFilterEnv;

private:
    static double MidiNoteToHz(int note) {
        return 440.0 * std::pow(2.0, (note - 69) / 12.0);
    }

    double mSampleRate = 44100.0;
    int mNoteNumber = -1;
    double mVelocity = 0.0;
    double mBaseFreq = 440.0;
    PolyBLEPOsc mSubOsc;
    PolyBLEPOsc mNoiseOsc;
};

} // namespace Synth108
