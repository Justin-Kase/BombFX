#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BombFXLookAndFeel : public juce::LookAndFeel_V4 {
public:
    BombFXLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffc0c0c0));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff8b5cf6));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff4a4a5e));
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override {
        
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;
        
        // LED indicator above knob
        auto ledBounds = bounds.withHeight(8).withY(bounds.getY() - 15);
        g.setColour(sliderPosProportional > 0.1f ? juce::Colour(0xff8b5cf6) : juce::Colour(0xff2a2a3e));
        g.fillEllipse(ledBounds.withSizeKeepingCentre(8, 8));
        
        // Outer ring (chrome/metallic)
        juce::ColourGradient outerGradient(
            juce::Colour(0xff9ca3af), bounds.getCentreX(), bounds.getY(),
            juce::Colour(0xff4b5563), bounds.getCentreX(), bounds.getBottom(),
            false);
        g.setGradientFill(outerGradient);
        g.fillEllipse(bounds);
        
        // Inner shadow ring
        g.setColour(juce::Colour(0xff1f2937).withAlpha(0.8f));
        g.fillEllipse(bounds.reduced(3));
        
        // Main knob body
        auto knobBounds = bounds.reduced(4);
        juce::ColourGradient knobGradient(
            juce::Colour(0xffd1d5db), knobBounds.getCentreX(), knobBounds.getY(),
            juce::Colour(0xff6b7280), knobBounds.getCentreX(), knobBounds.getBottom(),
            false);
        g.setGradientFill(knobGradient);
        g.fillEllipse(knobBounds);
        
        // Highlight
        auto highlightBounds = knobBounds.reduced(knobBounds.getHeight() * 0.15f).withHeight(knobBounds.getHeight() * 0.3f);
        g.setColour(juce::Colour(0xffffffff).withAlpha(0.4f));
        g.fillEllipse(highlightBounds);
        
        // Track arc (purple)
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                    arcRadius, arcRadius,
                                    0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff2a2a3e));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        if (slider.isEnabled()) {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                   arcRadius, arcRadius,
                                   0.0f, rotaryStartAngle, toAngle, true);
            g.setColour(juce::Colour(0xff8b5cf6));
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        
        // Pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + 8, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
        g.setColour(juce::Colour(0xff1f2937));
        g.fillPath(pointer);
    }
};
