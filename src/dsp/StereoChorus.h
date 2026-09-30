#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

namespace Synth108 {

/**
 * @brief Vintage BBD Stereo Chorus
 * Faithfully captures the magic of the Roland Juno-106 chorus unit
 * with dual modulated delay lines in anti-phase for lush stereo widening.
 */
class StereoChorus {
public:
    enum class Mode {
        Off = 0,
        ModeI,    // Slow & subtle shimmer (~0.5 Hz)
        ModeII,   // Rich & wide swirl (~0.83 Hz)
        ModeI_II  // Combined fast ensemble / rotary
    };

    StereoChorus() {
        SetSampleRate(44100.0);
    }

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        // Max delay buffer: ~50ms
        size_t bufferSize = static_cast<size_t>(mSampleRate * 0.05) + 16;
        mDelayBufferL.assign(bufferSize, 0.0);
        mDelayBufferR.assign(bufferSize, 0.0);
        mWriteIndex = 0;
        UpdateLFOs();
    }

    void SetMode(Mode mode) {
        mMode = mode;
        UpdateLFOs();
    }

    void SetMix(double mix) {
        mMix = std::clamp(mix, 0.0, 1.0);
    }

    void Reset() {
        std::fill(mDelayBufferL.begin(), mDelayBufferL.end(), 0.0);
        std::fill(mDelayBufferR.begin(), mDelayBufferR.end(), 0.0);
        mWriteIndex = 0;
        mLFOPhase1 = 0.0;
        mLFOPhase2 = 0.0;
    }

    void Process(double inL, double inR, double& outL, double& outR) {
        if (mMode == Mode::Off) {
            outL = inL;
            outR = inR;
            return;
        }

        // Advance LFOs
        mLFOPhase1 += mLFOInc1;
        if (mLFOPhase1 >= 1.0) mLFOPhase1 -= 1.0;

        mLFOPhase2 += mLFOInc2;
        if (mLFOPhase2 >= 1.0) mLFOPhase2 -= 1.0;

        double lfo1 = std::sin(mLFOPhase1 * 2.0 * M_PI);
        double lfo2 = std::sin(mLFOPhase2 * 2.0 * M_PI);

        double modL = 0.0;
        double modR = 0.0;

        if (mMode == Mode::ModeI) {
            modL = lfo1 * mDepth1;
            modR = -lfo1 * mDepth1; // Inverted phase for wide stereo
        } else if (mMode == Mode::ModeII) {
            modL = lfo2 * mDepth2;
            modR = -lfo2 * mDepth2;
        } else if (mMode == Mode::ModeI_II) {
            modL = (lfo1 * mDepth1 * 0.6) + (lfo2 * mDepth2 * 0.6);
            modR = (-lfo1 * mDepth1 * 0.6) + (-lfo2 * mDepth2 * 0.6);
        }

        // Base delay around 5.5 ms, modulated by +/- depth
        double delaySamplesL = (mBaseDelayMs + modL) * 0.001 * mSampleRate;
        double delaySamplesR = (mBaseDelayMs + modR) * 0.001 * mSampleRate;

        // Write current inputs
        mDelayBufferL[mWriteIndex] = inL;
        mDelayBufferR[mWriteIndex] = inR;

        // Read interpolated delay
        double wetL = ReadInterpolated(mDelayBufferL, delaySamplesL);
        double wetR = ReadInterpolated(mDelayBufferR, delaySamplesR);

        mWriteIndex = (mWriteIndex + 1) % mDelayBufferL.size();

        // Equal-power dry/wet mix
        double dryGain = (1.0 - mMix);
        double wetGain = mMix;
        outL = (inL * dryGain) + (wetL * wetGain);
        outR = (inR * dryGain) + (wetR * wetGain);
    }

private:
    void UpdateLFOs() {
        // Mode I: ~0.5 Hz, ~1.5 ms depth
        mLFOInc1 = 0.513 / mSampleRate;
        mDepth1 = 1.6;

        // Mode II: ~0.86 Hz, ~2.5 ms depth
        mLFOInc2 = 0.863 / mSampleRate;
        mDepth2 = 2.4;
    }

    double ReadInterpolated(const std::vector<double>& buffer, double delaySamples) const {
        double readPos = static_cast<double>(mWriteIndex) - delaySamples;
        double bufSize = static_cast<double>(buffer.size());
        while (readPos < 0.0) readPos += bufSize;
        while (readPos >= bufSize) readPos -= bufSize;

        size_t idx0 = static_cast<size_t>(readPos);
        size_t idx1 = (idx0 + 1) % buffer.size();
        double frac = readPos - static_cast<double>(idx0);

        // Linear interpolation
        return buffer[idx0] + frac * (buffer[idx1] - buffer[idx0]);
    }

    double mSampleRate = 44100.0;
    Mode mMode = Mode::ModeI;
    double mMix = 0.8;
    double mBaseDelayMs = 5.5;

    double mLFOPhase1 = 0.0;
    double mLFOInc1 = 0.0001;
    double mDepth1 = 1.6;

    double mLFOPhase2 = 0.0;
    double mLFOInc2 = 0.0002;
    double mDepth2 = 2.4;

    size_t mWriteIndex = 0;
    std::vector<double> mDelayBufferL;
    std::vector<double> mDelayBufferR;
};

} // namespace Synth108
