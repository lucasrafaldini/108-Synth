/**
 * 108 Synth - Web Audio Engine & UI Controller
 * Open Source Synthesizer for Hacktoberfest
 */

// Factory Presets
const PRESETS = [
  {
    name: "108 Init Lead",
    category: "Lead",
    osc1Wave: "sawtooth", osc1Vol: 0.8, osc1Oct: 0,
    osc2Wave: "sawtooth", osc2Vol: 0.6, osc2Detune: 8,
    subVol: 0.2, noiseVol: 0.0,
    cutoff: 4000, reso: 3, fltEnv: 2500,
    ampA: 0.01, ampD: 0.3, ampS: 0.8, ampR: 0.3,
    fltA: 0.05, fltD: 0.4, fltS: 0.3, fltR: 0.4,
    lfoRate: 2.5, chorusMode: 1, masterVol: 0.8
  },
  {
    name: "Juno 106 Bass",
    category: "Bass",
    osc1Wave: "square", osc1Vol: 0.8, osc1Oct: -1,
    osc2Wave: "sawtooth", osc2Vol: 0.7, osc2Detune: 4,
    subVol: 0.7, noiseVol: 0.0,
    cutoff: 750, reso: 6, fltEnv: 2000,
    ampA: 0.005, ampD: 0.35, ampS: 0.4, ampR: 0.15,
    fltA: 0.005, fltD: 0.25, fltS: 0.0, fltR: 0.2,
    lfoRate: 1.0, chorusMode: 1, masterVol: 0.85
  },
  {
    name: "Poly Brass 80s",
    category: "Brass",
    osc1Wave: "sawtooth", osc1Vol: 0.8, osc1Oct: 0,
    osc2Wave: "sawtooth", osc2Vol: 0.8, osc2Detune: 12,
    subVol: 0.1, noiseVol: 0.0,
    cutoff: 1400, reso: 4, fltEnv: 3500,
    ampA: 0.04, ampD: 0.4, ampS: 0.7, ampR: 0.4,
    fltA: 0.08, fltD: 0.5, fltS: 0.4, fltR: 0.4,
    lfoRate: 3.0, chorusMode: 2, masterVol: 0.8
  },
  {
    name: "Dreamy Juno Pad",
    category: "Pad",
    osc1Wave: "sawtooth", osc1Vol: 0.7, osc1Oct: 0,
    osc2Wave: "square", osc2Vol: 0.7, osc2Detune: 7,
    subVol: 0.3, noiseVol: 0.02,
    cutoff: 1600, reso: 4, fltEnv: 1500,
    ampA: 0.5, ampD: 0.8, ampS: 0.8, ampR: 1.5,
    fltA: 0.6, fltD: 1.0, fltS: 0.6, fltR: 1.2,
    lfoRate: 0.8, chorusMode: 3, masterVol: 0.8
  },
  {
    name: "Acid 303 Reso",
    category: "Bass",
    osc1Wave: "sawtooth", osc1Vol: 1.0, osc1Oct: 0,
    osc2Wave: "sawtooth", osc2Vol: 0.0, osc2Detune: 0,
    subVol: 0.0, noiseVol: 0.0,
    cutoff: 450, reso: 14, fltEnv: 4500,
    ampA: 0.002, ampD: 0.28, ampS: 0.2, ampR: 0.08,
    fltA: 0.002, fltD: 0.22, fltS: 0.0, fltR: 0.1,
    lfoRate: 2.0, chorusMode: 0, masterVol: 0.9
  }
];

class WebSynth108 {
  constructor() {
    this.audioCtx = null;
    this.activeVoices = new Map();
    this.chorusNode = null;
    this.masterGain = null;
    this.analyser = null;

    // Current State
    this.state = { ...PRESETS[0] };
    this.chorusMode = 1; // 0: off, 1: I, 2: II, 3: I+II

    this.initUI();
    this.initMidi();
    this.initKeyboard();
  }

  ensureAudioContext() {
    if (!this.audioCtx) {
      const AudioContext = window.AudioContext || window.webkitAudioContext;
      this.audioCtx = new AudioContext();

      // Master output chain
      this.masterGain = this.audioCtx.createGain();
      this.masterGain.gain.setValueAtTime(this.state.masterVol, this.audioCtx.currentTime);

      this.analyser = this.audioCtx.createAnalyser();
      this.analyser.fftSize = 1024;

      this.initChorus();

      this.masterGain.connect(this.analyser);
      this.analyser.connect(this.audioCtx.destination);

      this.startOscilloscope();
    }

    if (this.audioCtx.state === 'suspended') {
      this.audioCtx.resume();
    }
  }

  initChorus() {
    const ctx = this.audioCtx;

    // Stereo BBD Chorus simulation
    this.chorusInput = ctx.createGain();
    this.chorusDry = ctx.createGain();
    this.chorusWet = ctx.createGain();

    this.delayL = ctx.createDelay();
    this.delayR = ctx.createDelay();
    this.delayL.delayTime.value = 0.0055;
    this.delayR.delayTime.value = 0.0055;

    // Modulating LFOs for left and right
    this.chorusLFO = ctx.createOscillator();
    this.chorusLFO.frequency.value = 0.513;

    this.lfoGainL = ctx.createGain();
    this.lfoGainR = ctx.createGain();
    this.lfoGainL.gain.value = 0.0016;
    this.lfoGainR.gain.value = -0.0016; // Inverted phase for wide stereo

    this.chorusLFO.connect(this.lfoGainL);
    this.chorusLFO.connect(this.lfoGainR);
    this.lfoGainL.connect(this.delayL.delayTime);
    this.lfoGainR.connect(this.delayR.delayTime);
    this.chorusLFO.start();

    // Merger
    this.merger = ctx.createChannelMerger(2);
    this.delayL.connect(this.merger, 0, 0);
    this.delayR.connect(this.merger, 0, 1);

    this.chorusInput.connect(this.chorusDry);
    this.chorusInput.connect(this.delayL);
    this.chorusInput.connect(this.delayR);

    this.merger.connect(this.chorusWet);

    this.chorusDry.connect(this.masterGain);
    this.chorusWet.connect(this.masterGain);

    this.updateChorusMode(this.chorusMode);
  }

  updateChorusMode(mode) {
    this.chorusMode = mode;
    if (!this.chorusDry || !this.chorusWet) return;

    if (mode === 0) {
      this.chorusDry.gain.setValueAtTime(1.0, this.audioCtx.currentTime);
      this.chorusWet.gain.setValueAtTime(0.0, this.audioCtx.currentTime);
    } else {
      this.chorusDry.gain.setValueAtTime(0.3, this.audioCtx.currentTime);
      this.chorusWet.gain.setValueAtTime(0.7, this.audioCtx.currentTime);

      if (mode === 1) {
        this.chorusLFO.frequency.setValueAtTime(0.513, this.audioCtx.currentTime);
        this.lfoGainL.gain.setValueAtTime(0.0016, this.audioCtx.currentTime);
        this.lfoGainR.gain.setValueAtTime(-0.0016, this.audioCtx.currentTime);
      } else if (mode === 2) {
        this.chorusLFO.frequency.setValueAtTime(0.863, this.audioCtx.currentTime);
        this.lfoGainL.gain.setValueAtTime(0.0024, this.audioCtx.currentTime);
        this.lfoGainR.gain.setValueAtTime(-0.0024, this.audioCtx.currentTime);
      } else if (mode === 3) {
        this.chorusLFO.frequency.setValueAtTime(1.2, this.audioCtx.currentTime);
        this.lfoGainL.gain.setValueAtTime(0.003, this.audioCtx.currentTime);
        this.lfoGainR.gain.setValueAtTime(-0.003, this.audioCtx.currentTime);
      }
    }
  }

  noteOn(midiNote, velocity = 0.8) {
    this.ensureAudioContext();

    // Release existing note if active
    if (this.activeVoices.has(midiNote)) {
      this.noteOff(midiNote);
    }

    const ctx = this.audioCtx;
    const now = ctx.currentTime;
    const freq = 440 * Math.pow(2, (midiNote - 69) / 12);

    // Osc 1
    const osc1 = ctx.createOscillator();
    osc1.type = this.state.osc1Wave;
    const osc1Freq = freq * Math.pow(2, this.state.osc1Oct);
    osc1.frequency.setValueAtTime(osc1Freq, now);

    const osc1Gain = ctx.createGain();
    osc1Gain.gain.setValueAtTime(this.state.osc1Vol, now);
    osc1.connect(osc1Gain);

    // Osc 2
    const osc2 = ctx.createOscillator();
    osc2.type = this.state.osc2Wave;
    osc2.frequency.setValueAtTime(freq, now);
    osc2.detune.setValueAtTime(this.state.osc2Detune, now);

    const osc2Gain = ctx.createGain();
    osc2Gain.gain.setValueAtTime(this.state.osc2Vol, now);
    osc2.connect(osc2Gain);

    // Sub-Oscillator (Square wave 1 octave down)
    const subOsc = ctx.createOscillator();
    subOsc.type = "square";
    subOsc.frequency.setValueAtTime(freq * 0.5, now);

    const subGain = ctx.createGain();
    subGain.gain.setValueAtTime(this.state.subVol, now);
    subOsc.connect(subGain);

    // Filter
    const filter = ctx.createBiquadFilter();
    filter.type = "lowpass";
    filter.Q.setValueAtTime(this.state.reso, now);

    // Key follow: centers at MIDI 60
    const keyFollowHz = (midiNote - 60) * 15;
    const baseCutoff = Math.max(20, this.state.cutoff + keyFollowHz);

    // Filter Envelope Sweep
    filter.frequency.setValueAtTime(baseCutoff, now);
    const fltPeak = Math.min(20000, baseCutoff + this.state.fltEnv);
    filter.frequency.linearRampToValueAtTime(fltPeak, now + this.state.fltA);
    const fltSustainLevel = Math.max(20, baseCutoff + (this.state.fltEnv * this.state.fltS));
    filter.frequency.exponentialRampToValueAtTime(fltSustainLevel, now + this.state.fltA + this.state.fltD);

    // Amp Envelope
    const ampGain = ctx.createGain();
    ampGain.gain.setValueAtTime(0.0001, now);
    ampGain.gain.linearRampToValueAtTime(velocity, now + this.state.ampA);
    ampGain.gain.exponentialRampToValueAtTime(Math.max(0.0001, velocity * this.state.ampS), now + this.state.ampA + this.state.ampD);

    // Mix Oscillators -> Filter -> Amp -> Chorus
    osc1Gain.connect(filter);
    osc2Gain.connect(filter);
    subGain.connect(filter);
    filter.connect(ampGain);
    ampGain.connect(this.chorusInput);

    osc1.start(now);
    osc2.start(now);
    subOsc.start(now);

    this.activeVoices.set(midiNote, {
      osc1, osc2, subOsc, filter, ampGain, baseCutoff
    });

    // UI key highlight
    const keyEl = document.querySelector(`.key[data-note="${midiNote}"]`);
    if (keyEl) keyEl.classList.add('active');
  }

  noteOff(midiNote) {
    if (!this.activeVoices.has(midiNote) || !this.audioCtx) return;

    const voice = this.activeVoices.get(midiNote);
    this.activeVoices.delete(midiNote);

    const now = this.audioCtx.currentTime;
    const releaseTime = this.state.ampR;

    // Smooth release
    voice.ampGain.gain.cancelScheduledValues(now);
    voice.ampGain.gain.setValueAtTime(voice.ampGain.gain.value, now);
    voice.ampGain.gain.exponentialRampToValueAtTime(0.0001, now + releaseTime);

    voice.filter.frequency.cancelScheduledValues(now);
    voice.filter.frequency.setValueAtTime(voice.filter.frequency.value, now);
    voice.filter.frequency.exponentialRampToValueAtTime(Math.max(20, voice.baseCutoff), now + releaseTime);

    // Stop nodes after release
    setTimeout(() => {
      try {
        voice.osc1.stop();
        voice.osc2.stop();
        voice.subOsc.stop();
        voice.osc1.disconnect();
        voice.osc2.disconnect();
        voice.subOsc.disconnect();
        voice.filter.disconnect();
        voice.ampGain.disconnect();
      } catch (e) {}
    }, releaseTime * 1000 + 50);

    const keyEl = document.querySelector(`.key[data-note="${midiNote}"]`);
    if (keyEl) keyEl.classList.remove('active');
  }

  loadPreset(index) {
    const p = PRESETS[index];
    if (!p) return;
    this.state = { ...p };

    // Update UI elements
    document.getElementById('osc1Wave').value = p.osc1Wave;
    document.getElementById('osc1Vol').value = p.osc1Vol;
    document.getElementById('osc2Wave').value = p.osc2Wave;
    document.getElementById('osc2Detune').value = p.osc2Detune;
    document.getElementById('subVol').value = p.subVol;
    document.getElementById('cutoff').value = p.cutoff;
    document.getElementById('reso').value = p.reso;
    document.getElementById('fltEnv').value = p.fltEnv;
    document.getElementById('ampA').value = p.ampA;
    document.getElementById('ampD').value = p.ampD;
    document.getElementById('ampS').value = p.ampS;
    document.getElementById('ampR').value = p.ampR;

    this.updateChorusMode(p.chorusMode);
    document.querySelectorAll('.btn-toggle').forEach(b => {
      b.classList.toggle('active', parseInt(b.dataset.mode) === p.chorusMode);
    });

    this.updateLabels();
  }

  initUI() {
    // Preset dropdown
    const presetSelect = document.getElementById('presetSelect');
    PRESETS.forEach((p, idx) => {
      const opt = document.createElement('option');
      opt.value = idx;
      opt.textContent = `${p.category}: ${p.name}`;
      presetSelect.appendChild(opt);
    });

    presetSelect.addEventListener('change', (e) => {
      this.loadPreset(parseInt(e.target.value));
    });

    // Sliders & Controls
    const bind = (id, prop, transform = v => parseFloat(v)) => {
      const el = document.getElementById(id);
      if (!el) return;
      el.addEventListener('input', (e) => {
        this.state[prop] = transform(e.target.value);
        this.updateLabels();
      });
    };

    bind('osc1Wave', 'osc1Wave', v => v);
    bind('osc1Vol', 'osc1Vol');
    bind('osc2Wave', 'osc2Wave', v => v);
    bind('osc2Detune', 'osc2Detune');
    bind('subVol', 'subVol');
    bind('cutoff', 'cutoff');
    bind('reso', 'reso');
    bind('fltEnv', 'fltEnv');
    bind('ampA', 'ampA');
    bind('ampD', 'ampD');
    bind('ampS', 'ampS');
    bind('ampR', 'ampR');

    // Chorus Buttons
    document.querySelectorAll('.btn-toggle').forEach(btn => {
      btn.addEventListener('click', () => {
        const mode = parseInt(btn.dataset.mode);
        this.updateChorusMode(mode);
        document.querySelectorAll('.btn-toggle').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
      });
    });

    this.updateLabels();
  }

  updateLabels() {
    const setTxt = (id, val) => {
      const el = document.getElementById(id);
      if (el) el.textContent = val;
    };

    setTxt('v-cutoff', Math.round(this.state.cutoff) + ' Hz');
    setTxt('v-reso', (this.state.reso * 10).toFixed(0) + '%');
    setTxt('v-detune', this.state.osc2Detune + ' cents');
    setTxt('v-ampA', Math.round(this.state.ampA * 1000) + ' ms');
    setTxt('v-ampR', Math.round(this.state.ampR * 1000) + ' ms');
  }

  initKeyboard() {
    // Generate piano keys from C3 (MIDI 48) to C6 (MIDI 84)
    const kb = document.getElementById('pianoKeyboard');
    if (!kb) return;

    const isBlackKey = [false, true, false, true, false, false, true, false, true, false, true, false];

    for (let note = 48; note <= 77; note++) {
      const isBlack = isBlackKey[note % 12];
      const key = document.createElement('div');
      key.className = `key ${isBlack ? 'black' : 'white'}`;
      key.dataset.note = note;

      const trigger = () => this.noteOn(note);
      const release = () => this.noteOff(note);

      key.addEventListener('mousedown', (e) => { e.preventDefault(); trigger(); });
      key.addEventListener('mouseup', release);
      key.addEventListener('mouseleave', release);
      key.addEventListener('touchstart', (e) => { e.preventDefault(); trigger(); });
      key.addEventListener('touchend', release);

      kb.appendChild(key);
    }

    // Computer keyboard mapping
    const keyMap = {
      'a': 60, 'w': 61, 's': 62, 'e': 63, 'd': 64, 'f': 65, 't': 66,
      'g': 67, 'y': 68, 'h': 69, 'u': 70, 'j': 71, 'k': 72, 'o': 73, 'l': 74
    };

    window.addEventListener('keydown', (e) => {
      if (e.repeat) return;
      const note = keyMap[e.key.toLowerCase()];
      if (note) this.noteOn(note);
    });

    window.addEventListener('keyup', (e) => {
      const note = keyMap[e.key.toLowerCase()];
      if (note) this.noteOff(note);
    });
  }

  initMidi() {
    if (navigator.requestMIDIAccess) {
      navigator.requestMIDIAccess().then((midi) => {
        const midiStatus = document.getElementById('midiStatus');
        if (midiStatus) midiStatus.textContent = "MIDI Ready";

        for (const input of midi.inputs.values()) {
          input.onmidimessage = (msg) => this.handleMidiMessage(msg);
        }

        midi.onstatechange = (e) => {
          if (e.port.type === 'input') {
            e.port.onmidimessage = (msg) => this.handleMidiMessage(msg);
          }
        };
      }).catch(() => {
        const midiStatus = document.getElementById('midiStatus');
        if (midiStatus) midiStatus.textContent = "No MIDI";
      });
    }
  }

  handleMidiMessage(msg) {
    const [status, note, velocity] = msg.data;
    const command = status >> 4;

    if (command === 9 && velocity > 0) { // Note on
      this.noteOn(note, velocity / 127);
    } else if (command === 8 || (command === 9 && velocity === 0)) { // Note off
      this.noteOff(note);
    }
  }

  startOscilloscope() {
    const canvas = document.getElementById('scope');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const buffer = new Uint8Array(this.analyser.frequencyBinCount);

    const draw = () => {
      requestAnimationFrame(draw);
      this.analyser.getByteTimeDomainData(buffer);

      ctx.fillStyle = '#0d0f12';
      ctx.fillRect(0, 0, canvas.width, canvas.height);

      ctx.lineWidth = 2;
      ctx.strokeStyle = '#00c4cc';
      ctx.beginPath();

      const sliceWidth = canvas.width / buffer.length;
      let x = 0;

      for (let i = 0; i < buffer.length; i++) {
        const v = buffer[i] / 128.0;
        const y = (v * canvas.height) / 2;

        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);

        x += sliceWidth;
      }

      ctx.stroke();
    };

    draw();
  }
}

window.addEventListener('DOMContentLoaded', () => {
  window.synth108 = new WebSynth108();
});
