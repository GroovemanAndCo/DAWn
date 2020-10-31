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
class PlayheadComponent : public juce::Component, private juce::Timer
{
public:
    PlayheadComponent (tracktion_engine::Edit&, EditViewState&);
    
    void paint (juce::Graphics& g) override;
    bool hitTest (int x, int y) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseUp   (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    
    tracktion_engine::Edit& edit;
    EditViewState& editViewState;
    
    int xPosition = 0;
    bool firstTimer = true;
};

