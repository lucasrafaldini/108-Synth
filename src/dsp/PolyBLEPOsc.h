#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace Synth108 {

/**
 * @brief PolyBLEP (Polynomial Band-Limited Step) Oscillator
 * Produces clean, analog-sounding waveforms (Saw, Pulse/PWM, Triangle, Sub, Noise)
 * without digital aliasing artifacts even at high frequencies.
 */
class PolyBLEPOsc {
public:
    enum class Waveform {
        Sawtooth = 0,
        Pulse,      // Square / PWM
        Triangle,
        Sine,
        SubOsc,     // Square wave 1 or 2 octaves below
        Noise
    };

    PolyBLEPOsc() = default;

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        UpdatePhaseIncrement();
    }

    void SetFrequency(double freqHz) {
        mFrequency = std::max(0.1, freqHz);
        UpdatePhaseIncrement();
    }

    void SetWaveform(Waveform wave) {
        mWaveform = wave;
    }

    void SetPulseWidth(double pw) {
        // Clamp pulse width between 5% and 95%
        mPulseWidth = std::clamp(pw, 0.05, 0.95);
    }

    void ResetPhase(double phase = 0.0) {
        mPhase = phase;
    }

    double Process() {
        double out = 0.0;

        switch (mWaveform) {
            case Waveform::Sawtooth:
                out = ProcessSawtooth();
                break;
            case Waveform::Pulse:
                out = ProcessPulse();
                break;
            case Waveform::Triangle:
                out = ProcessTriangle();
                break;
            case Waveform::Sine:
                out = std::sin(mPhase * 2.0 * M_PI);
                break;
            case Waveform::SubOsc:
                out = ProcessSub();
                break;
            case Waveform::Noise:
                out = ProcessNoise();
                break;
        }

        // Advance phase
        mPhase += mPhaseInc;
        if (mPhase >= 1.0) {
            mPhase -= 1.0;
        }

        mSubPhase += mPhaseInc * 0.5; // 1 octave down
        if (mSubPhase >= 1.0) {
            mSubPhase -= 1.0;
        }

        return out;
    }

private:
    void UpdatePhaseIncrement() {
        mPhaseInc = mFrequency / mSampleRate;
    }

    // PolyBLEP residual algorithm for step discontinuities
    inline double PolyBLEP(double t, double dt) const {
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.0;
        } else if (t > 1.0 - dt) {
            t = (t - 1.0) / dt;
            return t * t + t + t + 1.0;
        }
        return 0.0;
    }

    double ProcessSawtooth() {
        // Raw saw: ramps from -1.0 to +1.0
        double rawSaw = (2.0 * mPhase) - 1.0;
        // Subtract PolyBLEP at the reset point (phase 0)
        return rawSaw - PolyBLEP(mPhase, mPhaseInc);
    }

    double ProcessPulse() {
        // Raw pulse between -1.0 and +1.0 with threshold at mPulseWidth
        double rawPulse = (mPhase < mPulseWidth) ? 1.0 : -1.0;
        // Correct transition at 0.0
        rawPulse += PolyBLEP(mPhase, mPhaseInc);
        // Correct transition at mPulseWidth
        double phaseShift = mPhase - mPulseWidth;
        if (phaseShift < 0.0) phaseShift += 1.0;
        rawPulse -= PolyBLEP(phaseShift, mPhaseInc);
        return rawPulse;
    }

    double ProcessTriangle() {
        // Integrated square wave to produce triangle with PolyBLEP
        double pulse = ProcessPulse();
        // Leaky integration
        mTriIntegrator = (mPhaseInc * pulse * 4.0) + (1.0 - mPhaseInc * 2.0) * mTriIntegrator;
        return mTriIntegrator;
    }

    double ProcessSub() {
        // Clean square wave 1 octave down
        double rawSub = (mSubPhase < 0.5) ? 1.0 : -1.0;
        rawSub += PolyBLEP(mSubPhase, mPhaseInc * 0.5);
        double phaseShift = mSubPhase - 0.5;
        if (phaseShift < 0.0) phaseShift += 1.0;
        rawSub -= PolyBLEP(phaseShift, mPhaseInc * 0.5);
        return rawSub;
    }

    double ProcessNoise() {
        // Pseudo-random white noise on [-1.0, 1.0]
        mNoiseSeed = mNoiseSeed * 196314165 + 907633515;
        return (static_cast<double>(mNoiseSeed) / 2147483648.0) - 1.0;
    }

    double mSampleRate = 44100.0;
    double mFrequency = 440.0;
    double mPhase = 0.0;
    double mSubPhase = 0.0;
    double mPhaseInc = 0.01;
    double mPulseWidth = 0.5;
    double mTriIntegrator = 0.0;
    uint32_t mNoiseSeed = 123456789;
    Waveform mWaveform = Waveform::Sawtooth;
};

} // namespace Synth108
