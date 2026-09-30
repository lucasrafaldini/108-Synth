#pragma once

#include <cmath>
#include <algorithm>

namespace Synth108 {

/**
 * @brief Exponential ADSR Envelope Generator
 * Provides analog-modelled exponential curves for punchy attacks,
 * natural decays, and smooth releases.
 */
class ADSREnvelope {
public:
    enum class State {
        Idle = 0,
        Attack,
        Decay,
        Sustain,
        Release
    };

    ADSREnvelope() = default;

    void SetSampleRate(double sampleRate) {
        mSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        UpdateRates();
    }

    void SetAttackTime(double attackSec) {
        mAttackTime = std::max(0.001, attackSec);
        UpdateRates();
    }

    void SetDecayTime(double decaySec) {
        mDecayTime = std::max(0.001, decaySec);
        UpdateRates();
    }

    void SetSustainLevel(double sustainLevel) {
        mSustainLevel = std::clamp(sustainLevel, 0.0, 1.0);
    }

    void SetReleaseTime(double releaseSec) {
        mReleaseTime = std::max(0.001, releaseSec);
        UpdateRates();
    }

    void NoteOn() {
        mState = State::Attack;
        // Keep current output value if retriggered for click-free legato
    }

    void NoteOff() {
        if (mState != State::Idle) {
            mState = State::Release;
        }
    }

    void Reset() {
        mState = State::Idle;
        mCurrentValue = 0.0;
    }

    bool IsActive() const {
        return mState != State::Idle;
    }

    State GetState() const {
        return mState;
    }

    double Process() {
        switch (mState) {
            case State::Idle:
                mCurrentValue = 0.0;
                break;

            case State::Attack:
                mCurrentValue += mAttackCoeff * (1.05 - mCurrentValue);
                if (mCurrentValue >= 1.0) {
                    mCurrentValue = 1.0;
                    mState = State::Decay;
                }
                break;

            case State::Decay:
                mCurrentValue -= mDecayCoeff * (mCurrentValue - mSustainLevel);
                if (mCurrentValue <= mSustainLevel + 0.0001) {
                    mCurrentValue = mSustainLevel;
                    mState = State::Sustain;
                }
                break;

            case State::Sustain:
                mCurrentValue = mSustainLevel;
                break;

            case State::Release:
                mCurrentValue -= mReleaseCoeff * mCurrentValue;
                if (mCurrentValue <= 0.0001) {
                    mCurrentValue = 0.0;
                    mState = State::Idle;
                }
                break;
        }

        return mCurrentValue;
    }

private:
    void UpdateRates() {
        // Exponential coefficients for target convergence
        mAttackCoeff  = 1.0 - std::exp(-1.0 / (mAttackTime * mSampleRate * 0.3));
        mDecayCoeff   = 1.0 - std::exp(-1.0 / (mDecayTime * mSampleRate * 0.3));
        mReleaseCoeff = 1.0 - std::exp(-1.0 / (mReleaseTime * mSampleRate * 0.3));
    }

    double mSampleRate = 44100.0;
    double mAttackTime = 0.01;   // 10 ms
    double mDecayTime = 0.2;    // 200 ms
    double mSustainLevel = 0.7; // 70%
    double mReleaseTime = 0.3;  // 300 ms

    double mAttackCoeff = 0.01;
    double mDecayCoeff = 0.01;
    double mReleaseCoeff = 0.01;

    double mCurrentValue = 0.0;
    State mState = State::Idle;
};

} // namespace Synth108
