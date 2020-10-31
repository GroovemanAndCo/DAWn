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

 /*
    ,--.                     ,--.     ,--.  ,--.
  ,-'  '-.,--.--.,--,--.,---.|  |,-.,-'  '-.`--' ,---. ,--,--,      Copyright 2018
  '-.  .-'|  .--' ,-.  | .--'|     /'-.  .-',--.| .-. ||      \   Tracktion Software
    |  |  |  |  \ '-'  \ `--.|  \  \  |  |  |  |' '-' '|  ||  |       Corporation
    `---' `--'   `--`--'`---'`--'`--' `---' `--' `---' `--''--'    www.tracktion.com
*/

#pragma once
#include <JuceHeader.h>

#include "PlayheadComponent.h"

#include "TrackComponent.h"
#include "TrackHeaderComponent.h"
#include "TrackFooterComponent.h"

//==============================================================================
class EditComponent : public juce::Component,
                      private tracktion_engine::ValueTreeAllEventListener,
                      private FlaggedAsyncUpdater,
					  private juce::ChangeListener
{
public:
    EditComponent (tracktion_engine::Edit&, tracktion_engine::SelectionManager&);
    ~EditComponent();
    
    EditViewState& getEditViewState()   { return editViewState; }
    
private:
    void valueTreeChanged() override {}
    
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    
    void handleAsyncUpdate() override;
    void resized() override;
    
    void changeListenerCallback (juce::ChangeBroadcaster*) override { repaint(); }

    
    void buildTracks();
    
    tracktion_engine::Edit& edit;
    
    EditViewState editViewState;
    
    PlayheadComponent playhead {edit, editViewState};
    juce::OwnedArray<TrackComponent> tracks;
    juce::OwnedArray<TrackHeaderComponent> headers;
    juce::OwnedArray<TrackFooterComponent> footers;
    
    bool updateTracks = false, updateZoom = false;
};
