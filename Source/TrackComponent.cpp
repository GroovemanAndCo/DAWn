#include "TrackComponent.h"
#include "AudioClipComponent.h"
#include "MidiClipComponent.h"

//==============================================================================
TrackComponent::TrackComponent (EditViewState& evs, tracktion_engine::Track::Ptr t)
    : editViewState (evs), track (t)
{
    track->state.addListener (this);
    track->edit.getTransport().addChangeListener (this);
    
    markAndUpdate (updateClips);
}

TrackComponent::~TrackComponent()
{
    track->state.removeListener (this);
    track->edit.getTransport().removeChangeListener (this);
}

void TrackComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::grey);
    
    if (editViewState.selectionManager.isSelected (track.get()))
    {
        g.setColour (juce::Colours::red);
        
        auto rc = getLocalBounds();
        if (editViewState.showHeaders) rc = rc.withTrimmedLeft (-4);
        if (editViewState.showFooters) rc = rc.withTrimmedRight (-4);

        g.drawRect (rc, 2);
    }
}

void TrackComponent::mouseDown (const juce::MouseEvent&)
{
    editViewState.selectionManager.selectOnly (track.get());
}

void TrackComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    markAndUpdate (updateRecordClips);
}

void TrackComponent::valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i)
{
    if (tracktion_engine::Clip::isClipState (v))
    {
        if (i == tracktion_engine::IDs::start
            || i == tracktion_engine::IDs::length)
        {
            markAndUpdate (updatePositions);
        }
    }
}

void TrackComponent::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree& c)
{
    if (tracktion_engine::Clip::isClipState (c))
        markAndUpdate (updateClips);
}

void TrackComponent::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree& c, int)
{
    if (tracktion_engine::Clip::isClipState (c))
        markAndUpdate (updateClips);
}

void TrackComponent::valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b)
{
    if (tracktion_engine::Clip::isClipState (v.getChild (a)))
        markAndUpdate (updatePositions);
    else if (tracktion_engine::Clip::isClipState (v.getChild (b)))
        markAndUpdate (updatePositions);
}

void TrackComponent::handleAsyncUpdate()
{
    if (compareAndReset (updateClips))
        buildClips();
    if (compareAndReset (updatePositions))
        resized();
    if (compareAndReset (updateRecordClips))
        buildRecordClips();
}

void TrackComponent::resized()
{
    for (auto cc : clips)
    {
        auto& c = cc->getClip();
        auto pos = c.getPosition();
        int x1 = editViewState.timeToXPosition (pos.getStart(), getWidth());
        int x2 = editViewState.timeToXPosition (pos.getEnd(), getWidth());
        
        cc->setBounds (x1, 0, x2 - x1, getHeight());
    }
}

void TrackComponent::buildClips()
{
    clips.clear();
    
    if (auto* ct = dynamic_cast<tracktion_engine::ClipTrack*> (track.get()))
    {
        for (auto* c : ct->getClips())
        {
            ClipComponent* cc = nullptr;
            
            if (dynamic_cast<tracktion_engine::WaveAudioClip*> (c))
                cc = new AudioClipComponent (editViewState, c);
            else if (dynamic_cast<tracktion_engine::MidiClip*> (c))
                cc = new MidiClipComponent (editViewState, c);
            else
                cc = new ClipComponent (editViewState, c);
            
            clips.add (cc);
            addAndMakeVisible (cc);
        }
    }
    
    resized();
}

void TrackComponent::buildRecordClips()
{
	auto needed = false;
    
    if (track->edit.getTransport().isRecording())
    {
        for (auto* in : track->edit.getAllInputDevices())
        {
            if (in->isRecordingActive() && track == in->getTargetTracks().getFirst())
            {
                needed = true;
                break;
            }
        }
    }
    
    if (needed)
    {
        recordingClip = std::make_unique<RecordingClipComponent> (track, editViewState);
        addAndMakeVisible (*recordingClip);
    }
    else
    {
        recordingClip = nullptr;
    }
}
