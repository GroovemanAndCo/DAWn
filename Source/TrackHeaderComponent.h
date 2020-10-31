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
class TrackHeaderComponent : public juce::Component,
                             private tracktion_engine::ValueTreeAllEventListener
{
public:
    TrackHeaderComponent (EditViewState&, tracktion_engine::Track::Ptr);
    virtual ~TrackHeaderComponent();
    
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void resized() override;
    
private:
    void valueTreeChanged() override {}
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    
    EditViewState& editViewState;
    tracktion_engine::Track::Ptr track;
    
    juce::ValueTree inputsState;
    juce::Label trackName;
    juce::TextButton armRecordButton {"R"}, muteButton {"M"}, soloButton {"S"}, inputButton {"I"};
};


