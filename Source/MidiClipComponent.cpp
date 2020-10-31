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
    
    if (auto mc = getMidiClip())
    {
        auto& seq = mc->getSequence();
        for (auto n : seq.getNotes())
        {
            double sBeat = mc->getStartBeat() + n->getStartBeat();
            double eBeat = mc->getStartBeat() + n->getEndBeat();
            
            auto s = editViewState.beatToTime (sBeat);
            auto e = editViewState.beatToTime (eBeat);
            
            if (auto p = getParentComponent())
            {
                double t1 = editViewState.timeToX (s, p->getWidth()) - getX();
                double t2 = editViewState.timeToX (e, p->getWidth()) - getX();
                
                double y = (1.0 - double (n->getNoteNumber()) / 127.0) * getHeight();
                
                g.setColour (Colours::white.withAlpha (n->getVelocity() / 127.0f));
                g.drawLine (float (t1), float (y), float (t2), float (y));
            }
        }
    }
}

