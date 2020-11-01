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
#include "ClipComponent.h"

//==============================================================================
class RecordingClipComponent : public juce::Component,private juce::Timer
{
public:
    RecordingClipComponent (tracktion_engine::Track::Ptr t, EditViewState&);
    
    void paint (juce::Graphics& g) override;
    
private:
    void timerCallback() override;
    void updatePosition();
    void initialiseThumbnailAndPunchTime();
    void drawThumbnail (juce::Graphics& g, juce::Colour waveformColour) const;
    bool getBoundsAndTime (juce::Rectangle<int>& bounds, juce::Range<double>& times) const;
    
    tracktion_engine::Track::Ptr track;
    EditViewState& editViewState;
    
    tracktion_engine::RecordingThumbnailManager::Thumbnail::Ptr thumbnail;
    double punchInTime = -1.0;
};

