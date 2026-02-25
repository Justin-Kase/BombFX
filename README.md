# Bomb FX 💣

A modern VST3/AU audio effects plugin built with JUCE, featuring three high-quality effects that can be mixed independently:

- **Reverb** — Spacious, smooth reverb with room size, damping, and stereo width controls
- **Delay** — Clean digital delay with feedback and adjustable time (1-2000ms)
- **Chorus** — Rich modulation with rate, depth, and center delay controls

## Features

- **Parallel FX routing** — All three effects process in parallel and mix independently
- **Modern UI** — Sleek dark gradient with rotary knobs
- **VST3 / AU / Standalone** — Build for all major plugin formats
- **Parameter automation** — Full DAW automation support
- **State persistence** — All settings save/restore with your project

## Build

### Requirements

- CMake 3.22+
- C++20 compiler
- JUCE 8.0.0 (fetched automatically)

### macOS

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Plugins installed to:
- AU: `~/Library/Audio/Plug-Ins/Components/Bomb FX.component`
- VST3: `~/Library/Audio/Plug-Ins/VST3/Bomb FX.vst3`

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

### Reverb
- **Mix** (0-100%) — Reverb wet level
- **Room Size** (0-100%) — Size of the simulated space
- **Damping** (0-100%) — High-frequency absorption
- **Width** (0-100%) — Stereo width

### Delay
- **Mix** (0-100%) — Delay wet level
- **Time** (1-2000ms) — Delay time
- **Feedback** (0-95%) — Delay feedback amount

### Chorus
- **Mix** (0-100%) — Chorus wet level
- **Rate** (0.1-10 Hz) — LFO modulation speed
- **Depth** (0-100%) — Modulation intensity
- **Center Delay** (7-30ms) — Base delay time

## License

MIT
