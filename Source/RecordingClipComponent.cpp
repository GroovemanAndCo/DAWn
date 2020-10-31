#include "RecordingClipComponent.h"

using namespace juce;

//==============================================================================
RecordingClipComponent::RecordingClipComponent (tracktion_engine::Track::Ptr t, EditViewState& evs)
    : track (t), editViewState (evs)
{
    startTimerHz (10);
    initialiseThumbnailAndPunchTime();
}

void RecordingClipComponent::initialiseThumbnailAndPunchTime()
{
    if (auto at = dynamic_cast<tracktion_engine::AudioTrack*> (track.get()))
    {
        for (auto* idi : at->edit.getEditInputDevices().getDevicesForTargetTrack (*at))
        {
            punchInTime = idi->getPunchInTime();
            
            if (idi->getRecordingFile().exists())
                thumbnail = at->edit.engine.getRecordingThumbnailManager().getThumbnailFor (idi->getRecordingFile());
        }
    }
}

void RecordingClipComponent::paint (Graphics& g)
{
    g.fillAll (Colours::red.withAlpha (0.5f));
    g.setColour (Colours::black);
    g.drawRect (getLocalBounds());
    
    if (editViewState.drawWaveforms)
        drawThumbnail (g, Colours::black.withAlpha (0.5f));
}

void RecordingClipComponent::drawThumbnail (Graphics& g, Colour waveformColour) const
{
    if (thumbnail == nullptr)
        return;
    
    Rectangle<int> bounds;
    Range<double> times;
    getBoundsAndTime (bounds, times);
    auto w = bounds.getWidth();
    
    if (w > 0 && w < 10000)
    {
        g.setColour (waveformColour);
        thumbnail->thumb.drawChannels (g, bounds, w, times, 1.0f);
    }
}

bool RecordingClipComponent::getBoundsAndTime (Rectangle<int>& bounds, Range<double>& times) const
{
    auto editTimeToX = [this] (double t)
    {
        if (auto p = getParentComponent())
            return editViewState.timeToXPosition (t, p->getWidth()) - getX();
        return 0;
    };
    
    auto xToEditTime = [this] (int x)
    {
        if (auto p = getParentComponent())
            return editViewState.xPositionToTime (x + getX(), p->getWidth());
        return 0.0;
    };
    
    bool hasLooped = false;
    auto& edit = track->edit;
    
    if (auto* playhead = edit.getTransport().getCurrentPlayhead())
    {
        auto localBounds = getLocalBounds();
        
        auto timeStarted = thumbnail->punchInTime;
        auto unloopedPos = timeStarted + thumbnail->thumb.getTotalLength();
        
        auto t1 = timeStarted;
        auto t2 = unloopedPos;
        
        if (playhead->isLooping() && t2 >= playhead->getLoopTimes().end)
        {
            hasLooped = true;
            
            t1 = jmin (t1, playhead->getLoopTimes().start);
            t2 = playhead->getPosition();
            
            t1 = jmax (editViewState.viewX1.get(), t1);
            t2 = jmin (editViewState.viewX2.get(), t2);
        }
        else if (edit.recordingPunchInOut)
        {
            const double in  = thumbnail->punchInTime;
            const double out = edit.getTransport().getLoopRange().getEnd();
            
            t1 = jlimit (in, out, t1);
            t2 = jlimit (in, out, t2);
        }
        
        bounds = localBounds.withX (jmax (localBounds.getX(), editTimeToX (t1)))
                 .withRight (jmin (localBounds.getRight(), editTimeToX (t2)));
        
        auto loopRange = playhead->getLoopTimes();
        const double recordedTime = unloopedPos - playhead->getLoopTimes().start;
        const int numLoops = (int) (recordedTime / loopRange.getLength());
        
        const Range<double> editTimes (xToEditTime (bounds.getX()),
                                       xToEditTime (bounds.getRight()));
        
        times = (editTimes + (numLoops * loopRange.getLength())) - timeStarted;
    }
    
    return hasLooped;
}

void RecordingClipComponent::timerCallback()
{
    updatePosition();
}

void RecordingClipComponent::updatePosition()
{
    auto& edit = track->edit;
    
    if (auto playhead = edit.getTransport().getCurrentPlayhead())
    {
        double t1 = punchInTime >= 0 ? punchInTime : edit.getTransport().getTimeWhenStarted();
        double t2 = jmax (t1, playhead->getUnloopedPosition());
        
        if (playhead->isLooping())
        {
            auto loopTimes = playhead->getLoopTimes();
            
            if (t2 >= loopTimes.end)
            {
                t1 = jmin (t1, loopTimes.start);
                t2 = loopTimes.end;
            }
        }
        else if (edit.recordingPunchInOut)
        {
            auto mr = edit.getTransport().getLoopRange();
            auto in  = mr.getStart();
            auto out = mr.getEnd();
            
            t1 = jlimit (in, out, t1);
            t2 = jlimit (in, out, t2);
        }
        
        t1 = jmax (t1, editViewState.viewX1.get());
        t2 = jmin (t2, editViewState.viewX2.get());
    
        if (auto p = getParentComponent())
        {
            int x1 = editViewState.timeToXPosition (t1, p->getWidth());
            int x2 = editViewState.timeToXPosition (t2, p->getWidth());
            
            setBounds (x1, 0, x2 - x1, p->getHeight());
            return;
        }
    }
    
    setBounds ({});
}
