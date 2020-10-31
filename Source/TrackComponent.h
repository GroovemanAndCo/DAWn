#pragma once
#include <JuceHeader.h>

#include "Utilities.h"
#include "ClipComponent.h"
#include "RecordingClipComponent.h"

//==============================================================================
class TrackComponent : public juce::Component,
                       private tracktion_engine::ValueTreeAllEventListener,
                       private FlaggedAsyncUpdater,
					   private juce::ChangeListener
{
public:
    TrackComponent (EditViewState&, tracktion_engine::Track::Ptr);
    ~TrackComponent();
    
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void resized() override;

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    void valueTreeChanged() override {}
    
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    
    void handleAsyncUpdate() override;
    
    void buildClips();
    void buildRecordClips();
    
    EditViewState& editViewState;
    tracktion_engine::Track::Ptr track;
    
    juce::OwnedArray<ClipComponent> clips;
    std::unique_ptr<RecordingClipComponent> recordingClip;
    
    bool updateClips = false, updatePositions = false, updateRecordClips = false;
};

