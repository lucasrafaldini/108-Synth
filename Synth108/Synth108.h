#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../src/dsp/Synth108Engine.h"
#include "../src/dsp/Preset.h"

const int kNumPresets = 5;

enum EParams
{
  // Osc 1
  kParamOsc1Wave = 0,
  kParamOsc1PulseWidth,
  kParamOsc1Octave,
  kParamOsc1Level,

  // Osc 2
  kParamOsc2Wave,
  kParamOsc2Detune,
  kParamOsc2Level,

  // Sub & Noise
  kParamSubLevel,
  kParamNoiseLevel,

  // Filter
  kParamCutoff,
  kParamResonance,
  kParamFilterEnvAmount,
  kParamFilterKeyFollow,

  // Amp ADSR
  kParamAmpAttack,
  kParamAmpDecay,
  kParamAmpSustain,
  kParamAmpRelease,

  // Filter ADSR
  kParamFltAttack,
  kParamFltDecay,
  kParamFltSustain,
  kParamFltRelease,

  // LFO
  kParamLFORate,
  kParamLFOPitchMod,
  kParamLFOFilterMod,
  kParamLFOPWMod,

  // Chorus
  kParamChorusMode,
  kParamChorusMix,

  // Master
  kParamMasterVol,

  kNumParams
};

using namespace iplug;
using namespace igraphics;

class Synth108 final : public Plugin
{
public:
  Synth108(const InstanceInfo& info);

#if IPLUG_EDITOR
  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
#endif
  
#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
#endif

private:
  Synth108::Synth108Engine mEngine;
};
