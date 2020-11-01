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
class AudioClipComponent : public ClipComponent
{
public:
    AudioClipComponent (EditViewState&, tracktion_engine::Clip::Ptr);
    
    tracktion_engine::WaveAudioClip* getWaveAudioClip() { return dynamic_cast<tracktion_engine::WaveAudioClip*> (clip.get()); }
    
    void paint (juce::Graphics& g) override;
    
private:
    void updateThumbnail();
    void drawWaveform (juce::Graphics& g, tracktion_engine::AudioClipBase& c, tracktion_engine::SmartThumbnail& thumb, juce::Colour colour,
                       int left, int right, int y, int h, int xOffset);
    void drawChannels (juce::Graphics& g, tracktion_engine::SmartThumbnail& thumb, juce::Rectangle<int> area, bool useHighRes,
                       tracktion_engine::EditTimeRange time, bool useLeft, bool useRight,
                       float leftGain, float rightGain);

    std::unique_ptr<tracktion_engine::SmartThumbnail> thumbnail;
};
