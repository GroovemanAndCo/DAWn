#pragma once
#include <JuceHeader.h>

//==============================================================================
class PluginEditor : public juce::Component
{
public:
    virtual bool allowWindowResizing() = 0;
    virtual juce::ComponentBoundsConstrainer* getBoundsConstrainer() = 0;
};

