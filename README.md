# Bomb FX 💣

A modern VST3/AU audio effects plugin built with JUCE, featuring four high-quality effects with independent mix controls, a striking chrome-and-purple UI, and full preset management.

## Features

### 🎛️ Four Effect Processors

- **Reverb** — Spacious, smooth reverb with room size, damping, and stereo width controls
- **Delay** — Clean digital delay with feedback and adjustable time (1-2000ms)
- **Chorus** — Rich modulation with rate, depth, and center delay controls
- **Filter** — Multi-mode filter (LP/HP/BP) with cutoff, resonance, and drive

### 🎨 Premium UI

- **Chrome knobs** with metallic gradients, 3D highlights, and center caps
- **Purple LED indicators** with glow effects and white pointers
- **Vertical mix faders** for each effect (mixer console style)
- **Enhanced gradient background** with noise texture and section glows
- **Section backgrounds** with rounded corners and purple glow borders
- **Gradient dividers** with elegant fade effects
- **Custom branding** with logo and enhanced title styling
- **Parameter labels** above every knob for clarity

### 🎚️ Signal Chain

```
Input → [Reverb | Delay | Chorus] (parallel) → Mix → Filter → Output
```

All three time-based effects process in parallel with independent mix controls, then the combined signal passes through the filter for final shaping.

### 🎯 Presets

**27 factory presets** organized by category:

**Reverb (6):**
- Cathedral — Large hall with long decay
- Small Room — Tight, intimate space
- Vocal Plate — Classic plate reverb
- Huge Hall — Massive concert hall
- Dark Chamber — Dark, damped reverb with LP filter
- Spring Tank — Vintage spring reverb with BP filter

**Delay (6):**
- Slapback — Quick, tight echo
- Dub Echo — Spacey feedback delay
- Ping Pong — Stereo bouncing delay
- Dotted Eighth — Rhythmic echo
- Triplet Delay — 333ms triplet timing
- Long Tail — 1.2s long delay with LP filter

**Chorus (6):**
- Lush Chorus — Thick, wide modulation
- Subtle Shimmer — Light movement
- Wide Stereo — Expansive stereo field
- Studio Thickener — Professional doubling
- Vibrato — Heavy modulation (5Hz, 90% depth)
- Ensemble — Classic ensemble chorus

**Combo (9):**
- Ambient Space — Reverb + delay + chorus blend
- Dream Machine — Ethereal, atmospheric
- Infinite Void — Deep, endless space
- Vocal Air — Transparent space for vocals with HP filter
- Synth Pad — Lush reverb+chorus+LP filter
- Guitar Shimmer — Guitar-optimized reverb+delay+chorus
- Lo-Fi Vibes — Dark, filtered, warbled character
- Psychedelic — Heavy effects with BP filter and resonance
- Epic Drums — Big room for drums with HP filter

### 💾 User Preset Management

**Save your own presets:**
1. Dial in your perfect sound
2. Click **Save** button (top-right)
3. Enter a custom name in the dialog
4. Preset saved to `~/Documents/BombFX/Presets/`

**Load user presets:**
1. Click **Refresh** button to scan for new presets
2. User presets appear under "--- USER PRESETS ---" in dropdown
3. Select any preset to load instantly

**Share presets:**
- Presets are saved as `.bombfx` XML files
- Share files with other users
- Edit manually if needed (all parameters are human-readable)

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

**Total: 17 parameters** (16 float + 1 choice)

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

### Preset System

- **Factory presets:** Hardcoded in C++ for instant loading and reliability
- **User presets:** XML files stored in `~/Documents/BombFX/Presets/`
- **XML format:** All parameters saved as attributes with float values
- **Auto-load:** User presets loaded on plugin startup
- **Real-time animation:** UI controls animate smoothly when presets change

## UI Layout

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  [Logo]  BOMB FX              PRESET: [dropdown ▼] [Save] [Refresh]         │
├─────────────┬──────────────┬──────────────┬──────────────┐
│   REVERB    │    DELAY     │    CHORUS    │    FILTER    │
│             │              │              │   [Type▼]    │
│    MIX      │     MIX      │     MIX      │     MIX      │
│    [█]      │     [█]      │     [█]      │     [█]      │
│             │              │              │              │
│   ROOM      │    TIME      │    RATE      │   CUTOFF     │
│    [◯]      │     [◯]      │     [◯]      │     [◯]      │
│   DAMP      │    FDBK      │    DEPTH     │    RESO      │
│    [◯]      │              │     [◯]      │     [◯]      │
│   WIDTH     │              │    DELAY     │    DRIVE     │
│    [◯]      │              │     [◯]      │     [◯]      │
└─────────────┴──────────────┴──────────────┴──────────────┘
```

- **Width:** 1100px
- **Height:** 550px
- **Chrome knobs:** 70px diameter with purple LED indicators, 3D gradients, white pointers
- **Mix faders:** 60px wide × 280px tall, vertical linear sliders
- **Labels:** 11pt font, light gray (0xffaaaaaa), positioned 15px above controls

## Development

### Project Structure

```
BombFX/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h/cpp      # Main audio processor + APVTS
│   ├── PluginEditor.h/cpp         # UI, layout, presets
│   ├── BombFXLookAndFeel.h        # Custom knob/fader styling
│   └── FX/
│       ├── ReverbProcessor.h/cpp
│       ├── DelayProcessor.h/cpp
│       ├── ChorusProcessor.h/cpp
│       └── FilterProcessor.h/cpp
├── Presets/
│   ├── Reverb/    # 6 presets
│   ├── Delay/     # 6 presets
│   ├── Chorus/    # 6 presets
│   └── Combo/     # 9 presets
└── Resources/
    └── logo.png   # UI branding (embedded via CMake)
```

### Key Decisions

- **JUCE 7.0.12** chosen over 8.0.0 for macOS 15 SDK compatibility
- **Parallel FX routing** for independent mix control
- **Filter at end** for creative shaping and character
- **Hardcoded factory presets** for instant loading (faster than XML parsing)
- **AudioParameterFloat operator=** for clean parameter updates and UI notification
- **Custom processors** for delay and chorus (precise control)
- **Soft clipping** in filter drive for musical distortion
- **Binary data** for logo embedding (juce_add_binary_data)
- **User preset directory** in Documents folder for easy access/sharing

## Testing

Load into your DAW (tested with Bitwig Studio):
1. Scan for new plugins
2. Load "Bomb FX" from VST3/AU menu
3. Select a preset from the dropdown
4. Watch knobs/faders update in real-time
5. Tweak parameters to taste
6. Click **Save** to store your settings
7. Click **Refresh** to reload after adding presets
8. Automate any parameter from your DAW

## Known Issues

None! User preset loading bug fixed in commit 49b6836.

## Contributing

Pull requests welcome! Areas for enhancement:
- Tempo sync for delay
- Envelope follower or LFO modulation for filter cutoff
- Additional combo presets
- Preset browser UI
- Undo/redo for preset changes

## License

MIT

## Links

- **Repository:** https://github.com/Justin-Kase/BombFX
- **Issues:** https://github.com/Justin-Kase/BombFX/issues

---

**Built with JUCE 7.0.12** • **VST is a trademark of Steinberg Media Technologies GmbH**
