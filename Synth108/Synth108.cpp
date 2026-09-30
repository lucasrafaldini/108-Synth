#include "Synth108.h"
#include "IPlug_include_in_plug_src.h"

#if IPLUG_EDITOR
#include "IControls.h"
#endif

Synth108::Synth108(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  // 1. Init Parameters
  // Osc 1
  GetParam(kParamOsc1Wave)->InitEnum("Osc 1 Wave", 0, {"Saw", "Pulse", "Triangle", "Sine"});
  GetParam(kParamOsc1PulseWidth)->InitDouble("Osc 1 PW", 50.0, 5.0, 95.0, 0.1, "%");
  GetParam(kParamOsc1Octave)->InitInt("Osc 1 Octave", 0, -2, 2, "oct");
  GetParam(kParamOsc1Level)->InitDouble("Osc 1 Vol", 80.0, 0.0, 100.0, 0.1, "%");

  // Osc 2
  GetParam(kParamOsc2Wave)->InitEnum("Osc 2 Wave", 0, {"Saw", "Pulse", "Triangle", "Sine"});
  GetParam(kParamOsc2Detune)->InitDouble("Osc 2 Detune", 5.0, -50.0, 50.0, 0.1, "cents");
  GetParam(kParamOsc2Level)->InitDouble("Osc 2 Vol", 70.0, 0.0, 100.0, 0.1, "%");

  // Sub & Noise
  GetParam(kParamSubLevel)->InitDouble("Sub Vol", 30.0, 0.0, 100.0, 0.1, "%");
  GetParam(kParamNoiseLevel)->InitDouble("Noise Vol", 0.0, 0.0, 100.0, 0.1, "%");

  // Filter
  GetParam(kParamCutoff)->InitFrequency("Cutoff", 2500.0, 20.0, 20000.0, 1.0);
  GetParam(kParamResonance)->InitDouble("Resonance", 25.0, 0.0, 100.0, 0.1, "%");
  GetParam(kParamFilterEnvAmount)->InitDouble("Filter Env", 50.0, -100.0, 100.0, 0.1, "%");
  GetParam(kParamFilterKeyFollow)->InitDouble("Key Follow", 50.0, 0.0, 100.0, 0.1, "%");

  // Amp ADSR
  GetParam(kParamAmpAttack)->InitDouble("Amp Attack", 10.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));
  GetParam(kParamAmpDecay)->InitDouble("Amp Decay", 300.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));
  GetParam(kParamAmpSustain)->InitDouble("Amp Sustain", 80.0, 0.0, 100.0, 0.1, "%");
  GetParam(kParamAmpRelease)->InitDouble("Amp Release", 300.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));

  // Filter ADSR
  GetParam(kParamFltAttack)->InitDouble("Flt Attack", 50.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));
  GetParam(kParamFltDecay)->InitDouble("Flt Decay", 400.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));
  GetParam(kParamFltSustain)->InitDouble("Flt Sustain", 30.0, 0.0, 100.0, 0.1, "%");
  GetParam(kParamFltRelease)->InitDouble("Flt Release", 500.0, 1.0, 5000.0, 0.1, "ms", IParam::kFlagsNone, "Envelope", IParam::ShapePowCurve(3.0));

  // LFO
  GetParam(kParamLFORate)->InitFrequency("LFO Rate", 2.5, 0.1, 20.0, 0.01);
  GetParam(kParamLFOPitchMod)->InitDouble("LFO Pitch", 0.0, 0.0, 12.0, 0.1, "semi");
  GetParam(kParamLFOFilterMod)->InitDouble("LFO Filter", 20.0, 0.0, 100.0, 0.1, "%");
  GetParam(kParamLFOPWMod)->InitDouble("LFO PWM", 20.0, 0.0, 100.0, 0.1, "%");

  // Chorus
  GetParam(kParamChorusMode)->InitEnum("Chorus", 1, {"Off", "I", "II", "I+II"});
  GetParam(kParamChorusMix)->InitDouble("Chorus Mix", 70.0, 0.0, 100.0, 0.1, "%");

  // Master
  GetParam(kParamMasterVol)->InitDouble("Master", 80.0, 0.0, 100.0, 0.1, "%");

  // Make Presets
  MakePreset("108 Init Lead");
  MakePreset("Juno 106 Bass");
  MakePreset("Poly Brass 80s");
  MakePreset("Dreamy Juno Pad");
  MakePreset("Acid 303 Reso");

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachCornerResizer(EUIResizerMode::Size, true);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Background - Dark metallic 80s synth faceplate
    pGraphics->AttachPanelBackground(IColor(255, 26, 28, 34));

    // Top Header
    const IRECT headerRect = b.GetFromTop(70);
    pGraphics->AttachControl(new ITextControl(headerRect.GetCentredInside(400, 40), "108 SYNTH", IText(34, IColor(255, 255, 120, 40)).WithWeight(EFontWeight::Bold)));
    pGraphics->AttachControl(new ITextControl(headerRect.GetFromRight(300).GetCentredInside(260, 20), "Polyphonic Analog Synthesizer", IText(14, IColor(255, 180, 180, 180))));

    // Main Control Area
    const IRECT mainArea = b.GetPadded(-15).GetReducedFromTop(70).GetReducedFromBottom(120);
    const auto cols = mainArea.SubDividents(5, EDirection::Horizontal);

    // Column 0: DCO 1 & DCO 2
    pGraphics->AttachControl(new IVKnobControl(cols[0].GetGridCell(0, 4, 1), kParamOsc1Wave, "DCO 1 Wave"));
    pGraphics->AttachControl(new IVKnobControl(cols[0].GetGridCell(1, 4, 1), kParamOsc1Level, "DCO 1 Vol"));
    pGraphics->AttachControl(new IVKnobControl(cols[0].GetGridCell(2, 4, 1), kParamOsc2Wave, "DCO 2 Wave"));
    pGraphics->AttachControl(new IVKnobControl(cols[0].GetGridCell(3, 4, 1), kParamOsc2Detune, "DCO 2 Detune"));

    // Column 1: Sub / Noise / Filter Cutoff & Reso
    pGraphics->AttachControl(new IVKnobControl(cols[1].GetGridCell(0, 4, 1), kParamSubLevel, "Sub Vol"));
    pGraphics->AttachControl(new IVKnobControl(cols[1].GetGridCell(1, 4, 1), kParamNoiseLevel, "Noise Vol"));
    pGraphics->AttachControl(new IVKnobControl(cols[1].GetGridCell(2, 4, 1), kParamCutoff, "VCF Cutoff"));
    pGraphics->AttachControl(new IVKnobControl(cols[1].GetGridCell(3, 4, 1), kParamResonance, "VCF Reso"));

    // Column 2: VCF Envelope
    pGraphics->AttachControl(new IVKnobControl(cols[2].GetGridCell(0, 4, 1), kParamFltAttack, "VCF Attack"));
    pGraphics->AttachControl(new IVKnobControl(cols[2].GetGridCell(1, 4, 1), kParamFltDecay, "VCF Decay"));
    pGraphics->AttachControl(new IVKnobControl(cols[2].GetGridCell(2, 4, 1), kParamFltSustain, "VCF Sustain"));
    pGraphics->AttachControl(new IVKnobControl(cols[2].GetGridCell(3, 4, 1), kParamFltRelease, "VCF Release"));

    // Column 3: VCA Envelope
    pGraphics->AttachControl(new IVKnobControl(cols[3].GetGridCell(0, 4, 1), kParamAmpAttack, "VCA Attack"));
    pGraphics->AttachControl(new IVKnobControl(cols[3].GetGridCell(1, 4, 1), kParamAmpDecay, "VCA Decay"));
    pGraphics->AttachControl(new IVKnobControl(cols[3].GetGridCell(2, 4, 1), kParamAmpSustain, "VCA Sustain"));
    pGraphics->AttachControl(new IVKnobControl(cols[3].GetGridCell(3, 4, 1), kParamAmpRelease, "VCA Release"));

    // Column 4: LFO, Chorus & Master
    pGraphics->AttachControl(new IVKnobControl(cols[4].GetGridCell(0, 4, 1), kParamLFORate, "LFO Rate"));
    pGraphics->AttachControl(new IVKnobControl(cols[4].GetGridCell(1, 4, 1), kParamChorusMode, "Chorus"));
    pGraphics->AttachControl(new IVKnobControl(cols[4].GetGridCell(2, 4, 1), kParamChorusMix, "Chorus Mix"));
    pGraphics->AttachControl(new IVKnobControl(cols[4].GetGridCell(3, 4, 1), kParamMasterVol, "Master Vol"));

    // Bottom Keyboard
    const IRECT keyboardRect = b.GetFromBottom(110).GetPadded(-10);
    pGraphics->AttachControl(new IVKeyboardControl(keyboardRect, 36, 84));
  };
#endif
}

#if IPLUG_EDITOR
void Synth108::OnParentWindowResize(int width, int height)
{
  if(GetUI())
    GetUI()->Resize(width, height, 1.f, false);
}
#endif

#if IPLUG_DSP
void Synth108::OnReset()
{
  mEngine.SetSampleRate(GetSampleRate());
}

void Synth108::OnParamChange(int paramIdx)
{
  auto& p = mEngine.GetParams();

  switch (paramIdx) {
    case kParamOsc1Wave: p.osc1Waveform = GetParam(paramIdx)->Int(); break;
    case kParamOsc1PulseWidth: p.osc1PulseWidth = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamOsc1Octave: p.osc1Octave = GetParam(paramIdx)->Int() * 12.0; break;
    case kParamOsc1Level: p.osc1Level = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamOsc2Wave: p.osc2Waveform = GetParam(paramIdx)->Int(); break;
    case kParamOsc2Detune: p.osc2DetuneSemi = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamOsc2Level: p.osc2Level = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamSubLevel: p.subLevel = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamNoiseLevel: p.noiseLevel = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamCutoff: p.filterCutoffHz = GetParam(paramIdx)->Value(); break;
    case kParamResonance: p.filterResonance = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamFilterEnvAmount: p.filterEnvAmount = (GetParam(paramIdx)->Value() / 100.0) * 5000.0; break;
    case kParamFilterKeyFollow: p.filterKeyFollow = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamAmpAttack: p.ampAttack = GetParam(paramIdx)->Value() / 1000.0; break;
    case kParamAmpDecay: p.ampDecay = GetParam(paramIdx)->Value() / 1000.0; break;
    case kParamAmpSustain: p.ampSustain = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamAmpRelease: p.ampRelease = GetParam(paramIdx)->Value() / 1000.0; break;

    case kParamFltAttack: p.fltAttack = GetParam(paramIdx)->Value() / 1000.0; break;
    case kParamFltDecay: p.fltDecay = GetParam(paramIdx)->Value() / 1000.0; break;
    case kParamFltSustain: p.fltSustain = GetParam(paramIdx)->Value() / 100.0; break;
    case kParamFltRelease: p.fltRelease = GetParam(paramIdx)->Value() / 1000.0; break;

    case kParamLFORate: p.lfoRateHz = GetParam(paramIdx)->Value(); break;
    case kParamLFOPitchMod: p.lfoToPitch = GetParam(paramIdx)->Value(); break;
    case kParamLFOFilterMod: p.lfoToFilter = (GetParam(paramIdx)->Value() / 100.0) * 2000.0; break;
    case kParamLFOPWMod: p.lfoToPWM = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamChorusMode: p.chorusMode = GetParam(paramIdx)->Int(); break;
    case kParamChorusMix: p.chorusMix = GetParam(paramIdx)->Value() / 100.0; break;

    case kParamMasterVol: p.masterVolume = GetParam(paramIdx)->Value() / 100.0; break;
    default: break;
  }

  mEngine.UpdateParams();
}

void Synth108::ProcessMidiMsg(const IMidiMsg& msg)
{
  switch (msg.StatusMsg()) {
    case IMidiMsg::kNoteOn:
      mEngine.NoteOn(msg.NoteNumber(), msg.Velocity());
      break;
    case IMidiMsg::kNoteOff:
      mEngine.NoteOff(msg.NoteNumber());
      break;
    case IMidiMsg::kPitchBend:
      mEngine.PitchBend(msg.PitchWheel());
      break;
    case IMidiMsg::kControlChange:
      if (msg.Controller() == IMidiMsg::EControlChangeMsg::kModulationWheel) {
        mEngine.ModWheel(msg.CControllerValue());
      } else if (msg.Controller() == IMidiMsg::EControlChangeMsg::kSustainPedal) {
        mEngine.SustainPedal(msg.CControllerValue() > 0.5);
      } else if (msg.Controller() == IMidiMsg::EControlChangeMsg::kAllNotesOff) {
        mEngine.AllNotesOff();
      }
      break;
    default:
      break;
  }
}

void Synth108::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  // Clear buffers
  memset(outputs[0], 0, nFrames * sizeof(sample));
  memset(outputs[1], 0, nFrames * sizeof(sample));

  // Render stereo audio from Synth108 engine
  mEngine.ProcessBlock(outputs, nFrames);
}
#endif
