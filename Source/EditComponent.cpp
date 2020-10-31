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

#include <JuceHeader.h>
#include "EditComponent.h"

//==============================================================================
EditComponent::EditComponent (tracktion_engine::Edit& e, tracktion_engine::SelectionManager& sm)
    : edit (e), editViewState (e, sm)
{
    edit.state.addListener (this);
    editViewState.selectionManager.addChangeListener (this);
    
    addAndMakeVisible (playhead);
    
    markAndUpdate (updateTracks);
}

EditComponent::~EditComponent()
{
    editViewState.selectionManager.removeChangeListener (this);
    edit.state.removeListener (this);
}

void EditComponent::valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i)
{
    if (v.hasType (IDs::EDITVIEWSTATE))
    {
        if (i == IDs::viewX1
            || i == IDs::viewX2
            || i == IDs::viewY)
        {
            markAndUpdate (updateZoom);
        }
        else if (i == IDs::showHeaders
                 || i == IDs::showFooters)
        {
            markAndUpdate (updateZoom);
        }
        else if (i == IDs::drawWaveforms)
        {
            repaint();
        }
    }
}

void EditComponent::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree& c)
{
    if (tracktion_engine::TrackList::isTrack (c))
        markAndUpdate (updateTracks);
}

void EditComponent::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree& c, int)
{
    if (tracktion_engine::TrackList::isTrack (c))
        markAndUpdate (updateTracks);
}

void EditComponent::valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b)
{
    if (tracktion_engine::TrackList::isTrack (v.getChild (a)))
        markAndUpdate (updateTracks);
    else if (tracktion_engine::TrackList::isTrack (v.getChild (b)))
        markAndUpdate (updateTracks);
}

void EditComponent::handleAsyncUpdate()
{
    if (compareAndReset (updateTracks))
        buildTracks();
    if (compareAndReset (updateZoom))
        resized();
}

void EditComponent::resized()
{
    jassert (headers.size() == tracks.size());
    
    const int trackHeight = 50, trackGap = 2;
    const int headerWidth = editViewState.showHeaders ? 150 : 0;
    const int footerWidth = editViewState.showFooters ? 150 : 0;
    
    playhead.setBounds (getLocalBounds().withTrimmedLeft (headerWidth).withTrimmedRight (footerWidth));
    
    int y = juce::roundToInt (editViewState.viewY.get());
    for (int i = 0; i < juce::jmin (headers.size(), tracks.size()); i++)
    {
        auto h = headers[i];
        auto t = tracks[i];
        auto f = footers[i];
        
        h->setBounds (0, y, headerWidth, trackHeight);
        t->setBounds (headerWidth, y, getWidth() - headerWidth - footerWidth, trackHeight);
        f->setBounds (getWidth() - footerWidth, y, footerWidth, trackHeight);
        
        y += trackHeight + trackGap;
    }
    
    for (auto t : tracks)
        t->resized();
}

void EditComponent::buildTracks()
{
    tracks.clear();
    headers.clear();
    footers.clear();
    
    for (auto t : getAllTracks (edit))
    {
        TrackComponent* c = nullptr;
        
        if (t->isTempoTrack())
        {
            if (editViewState.showGlobalTrack)
                c = new TrackComponent (editViewState, t);
        }
        else if (t->isMarkerTrack())
        {
            if (editViewState.showMarkerTrack)
                c = new TrackComponent (editViewState, t);
        }
        else if (t->isChordTrack())
        {
            if (editViewState.showChordTrack)
                c = new TrackComponent (editViewState, t);
        }
        else if (t->isArrangerTrack())
        {
            if (editViewState.showArrangerTrack)
                c = new TrackComponent (editViewState, t);
        }
        else
        {
            c = new TrackComponent (editViewState, t);
        }
        
        if (c != nullptr)
        {
            tracks.add (c);
            addAndMakeVisible (c);
            
            auto h = new TrackHeaderComponent (editViewState, t);
            headers.add (h);
            addAndMakeVisible (h);
            
            auto f = new TrackFooterComponent (editViewState, t);
            footers.add (f);
            addAndMakeVisible (f);
        }
    }
    
    playhead.toFront (false);
    resized();
}
