#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace Synth108 {

/**
 * @brief Low Frequency Oscillator (LFO)
 * Generates modulation signals for pitch (vibrato), filter cutoff (wah/wobble),
 * and pulse width (PWM).
 */
class LFO {
public:
    enum class Waveform {
        Sine = 0,
        Triangle,
        SawUp,
        SawDown,
        Square,
        SampleAndHold
    };

    LFO() = default;

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        UpdatePhaseInc();
    }

    void SetRate(double rateHz) {
        mRateHz = std::clamp(rateHz, 0.05, 30.0);
        UpdatePhaseInc();
    }

    void SetWaveform(Waveform wave) {
        mWaveform = wave;
    }

    void ResetPhase(double phase = 0.0) {
        mPhase = phase;
    }

    double Process() {
        double out = 0.0;

        switch (mWaveform) {
            case Waveform::Sine:
                out = std::sin(mPhase * 2.0 * M_PI);
                break;

            case Waveform::Triangle:
                out = (mPhase < 0.5) ? (4.0 * mPhase - 1.0) : (3.0 - 4.0 * mPhase);
                break;

            case Waveform::SawUp:
                out = 2.0 * mPhase - 1.0;
                break;

            case Waveform::SawDown:
                out = 1.0 - 2.0 * mPhase;
                break;

            case Waveform::Square:
                out = (mPhase < 0.5) ? 1.0 : -1.0;
                break;

            case Waveform::SampleAndHold:
                out = mSHValue;
                break;
        }

        mPhase += mPhaseInc;
        if (mPhase >= 1.0) {
            mPhase -= 1.0;
            // Generate new random value for S&H on cycle completion
            mRandomSeed = mRandomSeed * 196314165 + 907633515;
            mSHValue = (static_cast<double>(mRandomSeed) / 2147483648.0) - 1.0;
        }

        return out;
    }

private:
    void UpdatePhaseInc() {
        mPhaseInc = mRateHz / mSampleRate;
    }

    double mSampleRate = 44100.0;
    double mRateHz = 2.0;
    double mPhase = 0.0;
    double mPhaseInc = 0.0001;
    double mSHValue = 0.0;
    uint32_t mRandomSeed = 54321;
    Waveform mWaveform = Waveform::Triangle;
};

} // namespace Synth108
