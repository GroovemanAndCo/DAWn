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

 // Demonstrates how to put a custom component into a toolbar - this one contains
 // a ComboBox.
class ToolbarSlider : public juce::ToolbarItemComponent
{
public:
    ToolbarSlider(juce::Slider* slider, int toolbarItemId, int sPref, int sMin, int sMax)
        : ToolbarItemComponent(toolbarItemId, "Toolbar Slider", false)
    {
        _sizeMin = sMin;
        _sizeMax = sMax;
        _sizePref = sPref;
        _slider =  std::unique_ptr<juce::Slider>(slider);
        addAndMakeVisible(slider);
    }

    bool getToolbarItemSizes(int /*toolbarDepth*/, bool /* isVertical */,
        int& preferredSize, int& minSize, int& maxSize) override
    {
        preferredSize = _sizePref;
        minSize = _sizeMin;
        maxSize = _sizeMax;
        return true;
    }

    void paintButtonArea(juce::Graphics&, int, int, bool, bool) override
    {
    }

    void contentAreaChanged(const juce::Rectangle<int>& newArea) override
    {
        _slider->setSize(newArea.getWidth() - 2, juce::jmin(newArea.getHeight() - 2, 80));
        _slider->setCentrePosition(newArea.getCentreX(), newArea.getCentreY());
    }

    [[nodiscard]] juce::Slider* getSlider() const noexcept {return _slider.get();}

private:
    std::unique_ptr<juce::Slider> _slider;
    int _sizeMin, _sizeMax, _sizePref;
};

