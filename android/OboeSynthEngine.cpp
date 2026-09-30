/**
 * Android Oboe Audio Stream Callback for 108 Synth
 * Bridges Google Oboe (AAudio/OpenSL ES) with Synth108Engine (C++17)
 */

#include "../src/dsp/Synth108Engine.h"
#include <vector>

// Forward declarations for Android Oboe types
// Note: When building inside Android Studio, #include <oboe/Oboe.h> is used.
namespace AndroidAudio {

class OboeSynthCallback {
public:
    OboeSynthCallback() {
        mEngine.SetSampleRate(48000.0); // Default Android audio sample rate
    }

    void SetSampleRate(double sampleRate) {
        mEngine.SetSampleRate(sampleRate);
    }

    void HandleNoteOn(int note, float velocity) {
        mEngine.NoteOn(note, static_cast<double>(velocity));
    }

    void HandleNoteOff(int note) {
        mEngine.NoteOff(note);
    }

    void RenderAudio(float* outputBuffer, int numFrames, int numChannels) {
        // Prepare stereo channel pointers
        mChannelBuffers[0].resize(numFrames);
        mChannelBuffers[1].resize(numFrames);

        float* channelPtrs[2] = {
            mChannelBuffers[0].data(),
            mChannelBuffers[1].data()
        };

        // Render stereo block from Synth108Engine
        mEngine.ProcessBlock(channelPtrs, numFrames);

        // Interleave into Android output buffer
        for (int i = 0; i < numFrames; ++i) {
            outputBuffer[i * numChannels + 0] = channelPtrs[0][i];
            if (numChannels > 1) {
                outputBuffer[i * numChannels + 1] = channelPtrs[1][i];
            }
        }
    }

    Synth108::Synth108Engine& GetEngine() { return mEngine; }

private:
    Synth108::Synth108Engine mEngine;
    std::vector<float> mChannelBuffers[2];
};

} // namespace AndroidAudio
