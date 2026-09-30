#pragma once

#include <cmath>
#include <algorithm>

namespace Synth108 {

/**
 * @brief Resonant 4-pole (24dB/oct) and 2-pole (12dB/oct) Low-Pass Filter
 * Features non-linear feedback drive saturation (analog warmth)
 * and self-oscillating resonance.
 */
class LadderFilter {
public:
    enum class FilterMode {
        LowPass24 = 0,
        LowPass12,
        HighPass,
        BandPass
    };

    LadderFilter() = default;

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        Reset();
    }

    void SetCutoff(double cutoffHz) {
        // Limit cutoff between 20Hz and Nyquist * 0.95
        mCutoffHz = std::clamp(cutoffHz, 20.0, mSampleRate * 0.475);
    }

    void SetResonance(double resonance) {
        // 0.0 to 1.0 (approaching self-oscillation at 1.0)
        mResonance = std::clamp(resonance, 0.0, 1.0);
    }

    void SetDrive(double drive) {
        mDrive = std::clamp(drive, 1.0, 5.0);
    }

    void SetMode(FilterMode mode) {
        mMode = mode;
    }

    void Reset() {
        for (int i = 0; i < 4; ++i) {
            s[i] = 0.0;
        }
    }

    double Process(double input) {
        // 2x oversampled computation for stability near Nyquist
        double out = 0.0;
        for (int step = 0; step < 2; ++step) {
            out = ProcessSampleInternal(input);
        }
        return out;
    }

private:
    double FastTanh(double x) const {
        if (x < -3.0) return -1.0;
        if (x > 3.0) return 1.0;
        double x2 = x * x;
        return x * (27.0 + x2) / (27.0 + 9.0 * x2);
    }

    double ProcessSampleInternal(double input) {
        // Half sample rate step for oversampling
        double fs = mSampleRate * 2.0;
        double wd = 2.0 * M_PI * mCutoffHz;
        double T = 1.0 / fs;
        double wa = (2.0 / T) * std::tan(wd * T * 0.5);
        double g = wa * T * 0.5;
        g = std::clamp(g, 0.0001, 0.9999);

        // Feedback resonance with non-linear saturation
        double k = 4.0 * mResonance;
        double feedback = FastTanh(s[3] * mDrive);
        double u = (input * mDrive) - (k * feedback);

        // 4 cascaded one-pole stages
        double v0 = (u - s[0]) * g / (1.0 + g);
        double y0 = v0 + s[0];
        s[0] = y0 + v0;

        double v1 = (y0 - s[1]) * g / (1.0 + g);
        double y1 = v1 + s[1];
        s[1] = y1 + v1;

        double v2 = (y1 - s[2]) * g / (1.0 + g);
        double y2 = v2 + s[2];
        s[2] = y2 + v2;

        double v3 = (y2 - s[3]) * g / (1.0 + g);
        double y3 = v3 + s[3];
        s[3] = y3 + v3;

        // Output mode selection
        switch (mMode) {
            case FilterMode::LowPass24:
                return y3;
            case FilterMode::LowPass12:
                return y1;
            case FilterMode::BandPass:
                return 2.0 * (y1 - y3);
            case FilterMode::HighPass:
                return input - y3;
        }

        return y3;
    }

    double mSampleRate = 44100.0;
    double mCutoffHz = 2000.0;
    double mResonance = 0.0;
    double mDrive = 1.0;
    FilterMode mMode = FilterMode::LowPass24;

    double s[4] = {0.0, 0.0, 0.0, 0.0};
};

} // namespace Synth108
