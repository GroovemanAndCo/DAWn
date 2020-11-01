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
#include "AudioClipComponent.h"

using namespace juce;

//==============================================================================
AudioClipComponent::AudioClipComponent (EditViewState& evs, tracktion_engine::Clip::Ptr c)
    : ClipComponent (evs, c)
{
    updateThumbnail();
}

void AudioClipComponent::paint (Graphics& g)
{
    ClipComponent::paint (g);
    
    if (editViewState.drawWaveforms && thumbnail != nullptr)
        drawWaveform (g, *getWaveAudioClip(), *thumbnail, Colours::black.withAlpha (0.5f),
                      0, getWidth(), 0, getHeight(), 0);
}

void AudioClipComponent::drawWaveform (Graphics& g, tracktion_engine::AudioClipBase& c, tracktion_engine::SmartThumbnail& thumb, Colour colour,
                                       int left, int right, int y, int h, int xOffset)
{
    auto getTimeRangeForDrawing = [this] (const int l, const int r) -> tracktion_engine::EditTimeRange
    {
        if (auto* p = getParentComponent())
        {
	        const auto tLeft = editViewState.xPositionToTime (l, p->getWidth());
	        const auto tRight = editViewState.xPositionToTime (r, p->getWidth());
            
            return { tLeft, tRight };
        }
        
        return {};
    };
    
    jassert (left <= right);
    const auto gain = c.getGain();
    const auto pan = thumb.getNumChannels() == 1 ? 0.0f : c.getPan();
    
    const float pv = pan * gain;
    const float gainL = (gain - pv);
    const float gainR = (gain + pv);
    
    const bool usesTimeStretchedProxy = c.usesTimeStretchedProxy();
    
    const auto clipPos = c.getPosition();
    auto offset = clipPos.getOffset();
    auto speedRatio = c.getSpeedRatio();
    
    g.setColour (colour);
    
    if (usesTimeStretchedProxy)
    {
        const Rectangle<int> area (left + xOffset, y, right - left, h);
        
        if (! thumb.isOutOfDate())
        {
            drawChannels (g, thumb, area, false,
                          getTimeRangeForDrawing (left, right),
                          c.isLeftChannelActive(), c.isRightChannelActive(),
                          gainL, gainR);
        }
    }
    else if (c.getLoopLength() == 0)
    {
        auto region = getTimeRangeForDrawing (left, right);
        
        auto t1 = (region.getStart() + offset) * speedRatio;
        auto t2 = (region.getEnd()   + offset) * speedRatio;
        
        drawChannels (g, thumb,
                      { left + xOffset, y, right - left, h },
                      false, { t1, t2 },
                      c.isLeftChannelActive(), c.isRightChannelActive(),
                      gainL, gainR);
    }
}

void AudioClipComponent::drawChannels (Graphics& g, tracktion_engine::SmartThumbnail& thumb, Rectangle<int> area, bool useHighRes,
                                       tracktion_engine::EditTimeRange time, bool useLeft, bool useRight,
                                       float leftGain, float rightGain)
{
    if (useLeft && useRight && thumb.getNumChannels() > 1)
    {
        thumb.drawChannel (g, area.removeFromTop (area.getHeight() / 2), useHighRes, time, 0, leftGain);
        thumb.drawChannel (g, area, useHighRes, time, 1, rightGain);
    }
    else if (useLeft)
    {
        thumb.drawChannel (g, area, useHighRes, time, 0, leftGain);
    }
    else if (useRight)
    {
        thumb.drawChannel (g, area, useHighRes, time, 1, rightGain);
    }
}

void AudioClipComponent::updateThumbnail()
{
    if (auto* wac = getWaveAudioClip())
    {
        tracktion_engine::AudioFile af (wac->getAudioFile());
        
        if (af.getFile().existsAsFile() || (! wac->usesSourceFile()))
        {
            if (af.isValid())
            {
                const tracktion_engine::AudioFile proxy ((wac->hasAnyTakes() && wac->isShowingTakes()) ? wac->getAudioFile() : wac->getPlaybackFile());
                
                if (thumbnail == nullptr)
                    thumbnail = std::make_unique<tracktion_engine::SmartThumbnail> (wac->edit.engine, proxy, *this, &wac->edit);
                else
                    thumbnail->setNewFile (proxy);
            }
            else
            {
                thumbnail = nullptr;
            }
        }
    }
}
