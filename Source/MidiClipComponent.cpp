#pragma once
#include "MidiClipComponent.h"
using namespace juce;

//==============================================================================
MidiClipComponent::MidiClipComponent (EditViewState& evs, tracktion_engine::Clip::Ptr c)
    : ClipComponent (evs, c)
{
}

void MidiClipComponent::paint (Graphics& g)
{
    ClipComponent::paint (g);
    
    if (auto* mc = getMidiClip())
    {
        auto& seq = mc->getSequence();
        for (auto* n : seq.getNotes())
        {
	        const auto startBeat = mc->getStartBeat() + n->getStartBeat();
	        const auto endBeat = mc->getStartBeat() + n->getEndBeat();
            const auto startTime = editViewState.beatsToTime (startBeat);
            const auto endTime = editViewState.beatsToTime (endBeat);
            
            if (auto* p = getParentComponent())
            {
                const auto left  = static_cast<float>(editViewState.timeToXPosition(startTime, p->getWidth()) - getX());
                const auto right = static_cast<float>(editViewState.timeToXPosition (endTime, p->getWidth()) - getX());
                const auto y     = static_cast<float>((1.0 - n->getNoteNumber() / 127.0) * getHeight());
                
                g.setColour (Colours::white.withAlpha (n->getVelocity() / 127.0f));
                g.drawLine (left, y, right, y);
            }
        }
    }
}

