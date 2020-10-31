#pragma once
#include <JuceHeader.h>
#include "ClipComponent.h"

//==============================================================================
class MidiClipComponent : public ClipComponent
{
public:
    MidiClipComponent (EditViewState&, tracktion_engine::Clip::Ptr);

    [[nodiscard]] tracktion_engine::MidiClip* getMidiClip() const { return dynamic_cast<tracktion_engine::MidiClip*> (clip.get()); }
    
    void paint (juce::Graphics& g) override;
};

