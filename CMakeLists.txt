cmake_minimum_required(VERSION 3.22)
project(BombFX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force macOS 14 SDK compatibility (JUCE 7.x not yet updated for macOS 15)
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version" FORCE)
endif()

# JUCE framework (using CPM or FetchContent)
include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

# Plugin target
juce_add_plugin(BombFX
    COMPANY_NAME "Justin-Kase"
    PLUGIN_MANUFACTURER_CODE JKas
    PLUGIN_CODE BfxP
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Bomb FX"
    PLUGIN_NAME "BombFX"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Fx Reverb Delay Modulation
    AU_MAIN_TYPE kAudioUnitType_Effect
)

# Binary resources (logo)
juce_add_binary_data(BombFXData SOURCES Resources/logo.png)

# Source files
target_sources(BombFX PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/FX/ReverbProcessor.cpp
    Source/FX/DelayProcessor.cpp
    Source/FX/ChorusProcessor.cpp
    Source/FX/FilterProcessor.cpp
)

# JUCE modules
target_compile_definitions(BombFX PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(BombFX
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
        BombFXData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
