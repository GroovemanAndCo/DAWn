#pragma once
#include <JuceHeader.h>
#include "ClipComponent.h"

//==============================================================================
class AudioClipComponent : public ClipComponent
{
public:
    AudioClipComponent (EditViewState&, tracktion_engine::Clip::Ptr);
    
    tracktion_engine::WaveAudioClip* getWaveAudioClip() { return dynamic_cast<tracktion_engine::WaveAudioClip*> (clip.get()); }
    
    void paint (juce::Graphics& g) override;
    
private:
    void updateThumbnail();
    void drawWaveform (juce::Graphics& g, tracktion_engine::AudioClipBase& c, tracktion_engine::SmartThumbnail& thumb, juce::Colour colour,
                       int left, int right, int y, int h, int xOffset);
    void drawChannels (juce::Graphics& g, tracktion_engine::SmartThumbnail& thumb, juce::Rectangle<int> area, bool useHighRes,
                       tracktion_engine::EditTimeRange time, bool useLeft, bool useRight,
                       float leftGain, float rightGain);

    std::unique_ptr<tracktion_engine::SmartThumbnail> thumbnail;
};
