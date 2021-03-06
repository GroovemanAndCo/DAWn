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

