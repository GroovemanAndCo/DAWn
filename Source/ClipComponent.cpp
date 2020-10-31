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
#include "ClipComponent.h"

//==============================================================================
ClipComponent::ClipComponent (EditViewState& evs, tracktion_engine::Clip::Ptr c)
    : editViewState (evs), clip (c)
{
}

void ClipComponent::paint (juce::Graphics& g)
{
    g.fillAll (clip->getColour().withAlpha (0.5f));
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds());
    
    if (editViewState.selectionManager.isSelected (clip.get()))
    {
        g.setColour (juce::Colours::red);
        g.drawRect (getLocalBounds(), 2);
    }
}

void ClipComponent::mouseDown (const juce::MouseEvent&)
{
    editViewState.selectionManager.selectOnly (clip.get());
}


