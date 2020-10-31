#pragma once
#include <JuceHeader.h>
#include "EditViewState.h"

//==============================================================================
class ClipComponent : public juce::Component
{
public:
    ClipComponent (EditViewState&, tracktion_engine::Clip::Ptr);
    
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    
    tracktion_engine::Clip& getClip() { return *clip; }
    
protected:
    EditViewState& editViewState;
    tracktion_engine::Clip::Ptr clip;
};

