/*
 * This file is part of the DAWn distribution (https://github.com/GroovemanAndCo/DAWn).
 * Copyright (c) 2020 Fabien (https://github.com/fab672000)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

