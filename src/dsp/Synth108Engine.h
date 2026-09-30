#pragma once

#include "Voice.h"
#include "LFO.h"
#include "StereoChorus.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>

namespace Synth108 {

/**
 * @brief Parameters for Synth108
 */
struct SynthParams {
    // Oscillators
    int osc1Waveform = 0;     // 0: Saw, 1: Pulse, 2: Tri, 3: Sine
    double osc1PulseWidth = 0.5;
    double osc1Octave = 0.0;  // in semitones: -24, -12, 0, +12, +24
    double osc1Level = 0.8;

    int osc2Waveform = 0;
    double osc2PulseWidth = 0.5;
    double osc2Octave = 0.0;
    double osc2DetuneSemi = 0.05; // ~5 cents detune for thickness
    double osc2Level = 0.7;

    double subLevel = 0.3;
    double noiseLevel = 0.0;

    // Filter
    double filterCutoffHz = 2500.0;
    double filterResonance = 0.2;
    double filterEnvAmount = 3000.0;
    double filterKeyFollow = 0.5;

    // Amp ADSR
    double ampAttack = 0.01;
    double ampDecay = 0.2;
    double ampSustain = 0.8;
    double ampRelease = 0.4;

    // Filter ADSR
    double fltAttack = 0.05;
    double fltDecay = 0.4;
    double fltSustain = 0.3;
    double fltRelease = 0.5;

    // LFO
    double lfoRateHz = 2.5;
    int lfoWaveform = 1;      // Triangle
    double lfoToPitch = 0.0;  // Vibrato depth (semitones)
    double lfoToFilter = 300.0; // Cutoff mod (Hz)
    double lfoToPWM = 0.2;    // PWM modulation depth

    // Chorus
    int chorusMode = 1;       // 0: Off, 1: Mode I, 2: Mode II, 3: Mode I+II
    double chorusMix = 0.7;

    // Master
    double masterVolume = 0.8;
};

/**
 * @brief Polyphonic 108 Synthesizer Engine
 * Real-time audio engine that handles voice management, parameter dispatch,
 * MIDI note events, modulation, and stereo processing.
 */
class Synth108Engine {
public:
    static constexpr int kMaxVoices = 16;

    Synth108Engine() {
        SetSampleRate(44100.0);
        ApplyParams();
    }

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        for (auto& voice : mVoices) {
            voice.SetSampleRate(mSampleRate);
        }
        mLFO.SetSampleRate(mSampleRate);
        mChorus.SetSampleRate(mSampleRate);
    }

    SynthParams& GetParams() { return mParams; }
    const SynthParams& GetParams() const { return mParams; }

    void UpdateParams() {
        ApplyParams();
    }

    // MIDI Event Handling
    void NoteOn(int noteNumber, double velocity) {
        if (velocity <= 0.0) {
            NoteOff(noteNumber);
            return;
        }

        // Allocate voice: first search for idle voice, or steal oldest
        int voiceIdx = FindFreeVoice();
        if (voiceIdx < 0) {
            voiceIdx = StealVoice();
        }

        mVoiceAge[voiceIdx] = ++mGlobalAgeCounter;
        mVoices[voiceIdx].NoteOn(noteNumber, velocity, mParams.osc1Octave, mParams.osc2DetuneSemi + mParams.osc2Octave);
    }

    void NoteOff(int noteNumber) {
        for (size_t i = 0; i < kMaxVoices; ++i) {
            if (mVoices[i].GetNoteNumber() == noteNumber && mVoices[i].IsActive()) {
                if (mSustainPedal) {
                    mSustainedNotes[i] = true;
                } else {
                    mVoices[i].NoteOff();
                }
            }
        }
    }

    void PitchBend(double normalizedBend) { // -1.0 to +1.0
        mPitchBendSemi = normalizedBend * 2.0; // 2 semitones bend range
    }

    void ModWheel(double normalizedValue) { // 0.0 to 1.0
        mModWheel = normalizedValue;
    }

    void SustainPedal(bool pressed) {
        mSustainPedal = pressed;
        if (!pressed) {
            // Release all sustained notes
            for (size_t i = 0; i < kMaxVoices; ++i) {
                if (mSustainedNotes[i]) {
                    mSustainedNotes[i] = false;
                    mVoices[i].NoteOff();
                }
            }
        }
    }

    void AllNotesOff() {
        for (size_t i = 0; i < kMaxVoices; ++i) {
            mVoices[i].Kill();
            mSustainedNotes[i] = false;
        }
    }

    // Real-Time Audio Block Processing
    template<typename SampleType>
    void ProcessBlock(SampleType** outputs, int numFrames) {
        for (int s = 0; s < numFrames; ++s) {
            // 1. Process global LFO
            double lfoVal = mLFO.Process();

            // ModWheel increases LFO pitch depth
            double pitchMod = (mParams.lfoToPitch * (mModWheel * 0.8 + 0.2)) * lfoVal + mPitchBendSemi;
            double lfoFilterMod = mParams.lfoToFilter * lfoVal;
            double lfoPWMMod = mParams.lfoToPWM * lfoVal * 0.4;

            // Update PWM on active voices
            double pw1 = std::clamp(mParams.osc1PulseWidth + lfoPWMMod, 0.05, 0.95);
            double pw2 = std::clamp(mParams.osc2PulseWidth - lfoPWMMod, 0.05, 0.95);

            // 2. Sum active voices
            double voiceSum = 0.0;
            for (auto& voice : mVoices) {
                if (voice.IsActive()) {
                    voice.mOsc1.SetPulseWidth(pw1);
                    voice.mOsc2.SetPulseWidth(pw2);
                    voice.UpdateFrequencies(mParams.osc1Octave, mParams.osc2DetuneSemi + mParams.osc2Octave, pitchMod);

                    voiceSum += voice.Process(
                        mParams.osc1Level,
                        mParams.osc2Level,
                        mParams.subLevel,
                        mParams.noiseLevel,
                        mParams.filterCutoffHz,
                        mParams.filterResonance,
                        mParams.filterEnvAmount,
                        mParams.filterKeyFollow,
                        lfoFilterMod
                    );
                }
            }

            // Scale voice mix
            voiceSum *= 0.25;

            // 3. Process Stereo Chorus
            double outL = 0.0;
            double outR = 0.0;
            mChorus.Process(voiceSum, voiceSum, outL, outR);

            // 4. Master volume and soft limiter
            outL *= mParams.masterVolume;
            outR *= mParams.masterVolume;

            // Soft saturation to avoid digital clipping
            outL = std::tanh(outL);
            outR = std::tanh(outR);

            outputs[0][s] = static_cast<SampleType>(outL);
            outputs[1][s] = static_cast<SampleType>(outR);
        }
    }

private:
    int FindFreeVoice() {
        for (int i = 0; i < kMaxVoices; ++i) {
            if (!mVoices[i].IsActive()) {
                return i;
            }
        }
        return -1;
    }

    int StealVoice() {
        int oldestIdx = 0;
        uint64_t oldestAge = mVoiceAge[0];

        for (int i = 1; i < kMaxVoices; ++i) {
            if (mVoiceAge[i] < oldestAge) {
                oldestAge = mVoiceAge[i];
                oldestIdx = i;
            }
        }
        return oldestIdx;
    }

    void ApplyParams() {
        for (auto& voice : mVoices) {
            voice.mOsc1.SetWaveform(static_cast<PolyBLEPOsc::Waveform>(mParams.osc1Waveform));
            voice.mOsc2.SetWaveform(static_cast<PolyBLEPOsc::Waveform>(mParams.osc2Waveform));
            voice.mOsc1.SetPulseWidth(mParams.osc1PulseWidth);
            voice.mOsc2.SetPulseWidth(mParams.osc2PulseWidth);

            voice.mAmpEnv.SetAttackTime(mParams.ampAttack);
            voice.mAmpEnv.SetDecayTime(mParams.ampDecay);
            voice.mAmpEnv.SetSustainLevel(mParams.ampSustain);
            voice.mAmpEnv.SetReleaseTime(mParams.ampRelease);

            voice.mFilterEnv.SetAttackTime(mParams.fltAttack);
            voice.mFilterEnv.SetDecayTime(mParams.fltDecay);
            voice.mFilterEnv.SetSustainLevel(mParams.fltSustain);
            voice.mFilterEnv.SetReleaseTime(mParams.fltRelease);
        }

        mLFO.SetRate(mParams.lfoRateHz);
        mLFO.SetWaveform(static_cast<LFO::Waveform>(mParams.lfoWaveform));

        mChorus.SetMode(static_cast<StereoChorus::Mode>(mParams.chorusMode));
        mChorus.SetMix(mParams.chorusMix);
    }

    double mSampleRate = 44100.0;
    SynthParams mParams;

    std::array<Voice, kMaxVoices> mVoices;
    std::array<uint64_t, kMaxVoices> mVoiceAge{0};
    std::array<bool, kMaxVoices> mSustainedNotes{false};
    uint64_t mGlobalAgeCounter = 0;

    LFO mLFO;
    StereoChorus mChorus;

    double mPitchBendSemi = 0.0;
    double mModWheel = 0.0;
    bool mSustainPedal = false;
};

} // namespace Synth108
