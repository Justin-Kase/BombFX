# Bomb FX 💣

A modern VST3/AU audio effects plugin built with JUCE, featuring four high-quality effects with independent mix controls and a striking chrome-and-purple UI.

## Features

### 🎛️ Four Effect Processors

- **Reverb** — Spacious, smooth reverb with room size, damping, and stereo width controls
- **Delay** — Clean digital delay with feedback and adjustable time (1-2000ms)
- **Chorus** — Rich modulation with rate, depth, and center delay controls
- **Filter** — Multi-mode filter (LP/HP/BP) with cutoff, resonance, and drive

### 🎨 Premium UI

- **Chrome knobs** with metallic gradients and 3D highlights
- **Purple LED indicators** with glow effects
- **Vertical mix faders** for each effect (mixer console style)
- **Dark gradient background** with cyan section dividers
- **Custom branding** with logo integration

### 🎚️ Signal Chain

```
Input → [Reverb | Delay | Chorus] (parallel) → Mix → Filter → Output
```

All three time-based effects process in parallel with independent mix controls, then the combined signal passes through the filter for final shaping.

### 🎯 Presets

**15 factory presets** organized by category:

**Reverb:**
- Cathedral — Large hall with long decay
- Small Room — Tight, intimate space
- Vocal Plate — Classic plate reverb
- Huge Hall — Massive concert hall

**Delay:**
- Slapback — Quick, tight echo
- Dub Echo — Spacey feedback delay
- Ping Pong — Stereo bouncing delay
- Dotted Eighth — Rhythmic echo

**Chorus:**
- Lush Chorus — Thick, wide modulation
- Subtle Shimmer — Light movement
- Wide Stereo — Expansive stereo field
- Studio Thickener — Professional doubling

**Combo:**
- Ambient Space — Reverb + delay + chorus blend
- Dream Machine — Ethereal, atmospheric
- Infinite Void — Deep, endless space

### 🔧 Format Support

- **VST3** — Industry-standard plugin format
- **AU** — macOS Audio Unit
- **Standalone** — Run as a desktop application

## Build

### Requirements

- CMake 3.22+
- C++20 compiler
- JUCE 7.0.12 (fetched automatically via CMake)

### macOS

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

Plugins auto-install to:
- AU: `~/Library/Audio/Plug-Ins/Components/Bomb FX.component`
- VST3: `~/Library/Audio/Plug-Ins/VST3/Bomb FX.vst3`
- Standalone: `build/BombFX_artefacts/Release/Standalone/Bomb FX.app`

### Windows

```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### Linux

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Parameters

### Reverb (4 parameters)
- **Mix** (0-100%) — Reverb wet level (vertical fader)
- **Room Size** (0-100%) — Size of the simulated space
- **Damping** (0-100%) — High-frequency absorption
- **Width** (0-100%) — Stereo width

### Delay (3 parameters)
- **Mix** (0-100%) — Delay wet level (vertical fader)
- **Time** (1-2000ms) — Delay time
- **Feedback** (0-95%) — Delay feedback amount

### Chorus (4 parameters)
- **Mix** (0-100%) — Chorus wet level (vertical fader)
- **Rate** (0.1-10 Hz) — LFO modulation speed
- **Depth** (0-100%) — Modulation intensity
- **Center Delay** (7-30ms) — Base delay time

### Filter (5 parameters)
- **Mix** (0-100%) — Filter wet level (vertical fader)
- **Cutoff** (20-20,000 Hz) — Filter cutoff frequency
- **Resonance** (0-100%) — Filter resonance/Q (0.7-20)
- **Drive** (1-10x) — Saturation/distortion amount (soft clipping)
- **Type** — Filter mode: Low Pass / High Pass / Band Pass

## Architecture

### Signal Processing

1. **Input** is split into three parallel paths
2. **Reverb**, **Delay**, and **Chorus** process independently
3. Each effect's wet signal is scaled by its **Mix** fader
4. All three wet signals + remaining dry signal are summed
5. Combined signal passes through the **Filter** (cutoff, resonance, drive)
6. Filter's wet/dry mix applied for final output

### DSP Implementation

- **Reverb:** JUCE DSP Reverb module
- **Delay:** Custom delay line with feedback
- **Chorus:** Custom LFO-driven delay modulation
- **Filter:** JUCE StateVariableTPTFilter with tanh saturation

## UI Layout

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo]  BOMB FX                    PRESET: [dropdown ▼]        │
├─────────┬──────────┬──────────┬──────────┐
│ REVERB  │  DELAY   │  CHORUS  │  FILTER  │
│         │          │          │  [Type▼] │
│  MIX    │   MIX    │   MIX    │   MIX    │
│  [█]    │   [█]    │   [█]    │   [█]    │
│  [◯]    │   [◯]    │   [◯]    │   [◯]    │
│  [◯]    │   [◯]    │   [◯]    │   [◯]    │
│  [◯]    │          │   [◯]    │   [◯]    │
└─────────┴──────────┴──────────┴──────────┘
```

- **Width:** 1100px
- **Height:** 550px
- **Chrome knobs:** 70px diameter with purple LED indicators
- **Mix faders:** 60px wide × 280px tall

## Development

### Project Structure

```
BombFX/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h/cpp      # Main audio processor
│   ├── PluginEditor.h/cpp         # UI and layout
│   ├── BombFXLookAndFeel.h        # Custom knob styling
│   └── FX/
│       ├── ReverbProcessor.h/cpp
│       ├── DelayProcessor.h/cpp
│       ├── ChorusProcessor.h/cpp
│       └── FilterProcessor.h/cpp
├── Presets/
│   ├── Reverb/    # 4 presets
│   ├── Delay/     # 4 presets
│   ├── Chorus/    # 4 presets
│   └── Combo/     # 3 presets
└── Resources/
    └── logo.png   # UI branding
```

### Key Decisions

- **JUCE 7.0.12** chosen over 8.0.0 for macOS 15 SDK compatibility
- **Parallel FX routing** for independent mix control
- **Filter at end** for creative shaping and character
- **Direct parameter updates** via `setValueNotifyingHost()` for real-time preset loading
- **Custom processors** for delay and chorus (precise control)
- **Soft clipping** in filter drive for musical distortion

## Testing

Load into your DAW (tested with Bitwig Studio):
1. Scan for new plugins
2. Load "Bomb FX" from VST3/AU menu
3. Select a preset from the dropdown
4. Watch knobs/faders update in real-time
5. Tweak parameters or switch presets
6. Automate any parameter from your DAW

## License

MIT

## Links

- **Repository:** https://github.com/Justin-Kase/BombFX
- **Issues:** https://github.com/Justin-Kase/BombFX/issues

---

**Built with JUCE 7.0.12** • **VST is a trademark of Steinberg Media Technologies GmbH**
