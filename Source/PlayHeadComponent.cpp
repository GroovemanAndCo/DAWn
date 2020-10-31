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
#include "PlayHeadComponent.h"
//==============================================================================
PlayheadComponent::PlayheadComponent (tracktion_engine::Edit& e , EditViewState& evs)
    : edit (e), editViewState (evs)
{
    startTimerHz (30);
}

void PlayheadComponent::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::yellow);
    g.drawRect (xPosition, 0, 2, getHeight());
}

bool PlayheadComponent::hitTest (int x, int)
{
    if (std::abs (x - xPosition) <= 3)
        return true;
    
    return false;
}

void PlayheadComponent::mouseDown (const juce::MouseEvent&)
{
    edit.getTransport().setUserDragging (true);
}

void PlayheadComponent::mouseUp (const juce::MouseEvent&)
{
    edit.getTransport().setUserDragging (false);
}

void PlayheadComponent::mouseDrag (const juce::MouseEvent& e)
{
    double t = editViewState.xPositionToTime (e.x, getWidth());
    edit.getTransport().setCurrentPosition (t);
    timerCallback();
}

void PlayheadComponent::timerCallback()
{
    if (firstTimer)
    {
        // On Linux, don't set the mouse cursor until after the Component has appeared
        firstTimer = false;
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
    }

    int newX = editViewState.timeToX (edit.getTransport().getCurrentPosition(), getWidth());
    if (newX != xPosition)
    {
        repaint (juce::jmin (newX, xPosition) - 1, 0, juce::jmax (newX, xPosition) - juce::jmin (newX, xPosition) + 3, getHeight());
        xPosition = newX;
    }
}

