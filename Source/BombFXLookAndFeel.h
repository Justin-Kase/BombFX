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
        auto lineW = juce::jmin(6.0f, radius * 0.4f);
        auto arcRadius = radius - lineW * 0.5f;
        
        // LED indicator above knob (purple when active)
        auto ledBounds = bounds.withHeight(10).withY(bounds.getY() - 18).withSizeKeepingCentre(10, 10);
        
        // LED glow effect
        if (sliderPosProportional > 0.05f) {
            g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.3f));
            g.fillEllipse(ledBounds.expanded(4));
            g.setColour(juce::Colour(0xff8b5cf6));
        } else {
            g.setColour(juce::Colour(0xff2a2a3e));
        }
        g.fillEllipse(ledBounds);
        
        // Chrome outer ring with metallic gradient
        juce::ColourGradient outerGradient(
            juce::Colour(0xffb0b0b0), bounds.getCentreX(), bounds.getY(),
            juce::Colour(0xff505050), bounds.getCentreX(), bounds.getBottom(),
            false);
        g.setGradientFill(outerGradient);
        g.fillEllipse(bounds);
        
        // Dark inner shadow ring
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillEllipse(bounds.reduced(4));
        
        // Main knob body with metallic shine
        auto knobBounds = bounds.reduced(6);
        juce::ColourGradient knobGradient(
            juce::Colour(0xffe0e0e0), knobBounds.getCentreX(), knobBounds.getY(),
            juce::Colour(0xff707070), knobBounds.getCentreX(), knobBounds.getBottom(),
            false);
        g.setGradientFill(knobGradient);
        g.fillEllipse(knobBounds);
        
        // Top highlight (gives 3D depth)
        auto highlightBounds = knobBounds.reduced(knobBounds.getHeight() * 0.15f)
                                         .withHeight(knobBounds.getHeight() * 0.25f);
        g.setColour(juce::Colour(0xffffffff).withAlpha(0.5f));
        g.fillEllipse(highlightBounds);
        
        // Center cap (darker)
        auto centerCap = knobBounds.reduced(knobBounds.getWidth() * 0.25f);
        juce::ColourGradient centerGradient(
            juce::Colour(0xff888888), centerCap.getCentreX(), centerCap.getY(),
            juce::Colour(0xff404040), centerCap.getCentreX(), centerCap.getBottom(),
            false);
        g.setGradientFill(centerGradient);
        g.fillEllipse(centerCap);
        
        // Track arc (dark background)
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                    arcRadius, arcRadius,
                                    0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff1a1a2e));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Active arc (purple)
        if (slider.isEnabled()) {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                   arcRadius, arcRadius,
                                   0.0f, rotaryStartAngle, toAngle, true);
            
            // Purple glow
            g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.3f));
            g.strokePath(valueArc, juce::PathStrokeType(lineW + 2, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            
            g.setColour(juce::Colour(0xff8b5cf6));
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        
        // Pointer indicator (white line from center)
        juce::Path pointer;
        auto pointerLength = radius * 0.4f;
        auto pointerThickness = 2.5f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + 12, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
        
        // White pointer with slight shadow
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillPath(pointer, juce::AffineTransform::translation(1, 1));
        g.setColour(juce::Colours::white);
        g.fillPath(pointer);
    }
};
