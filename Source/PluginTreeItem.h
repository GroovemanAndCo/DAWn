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
#include "PluginTreeBase.h"

//==============================================================================
class PluginTreeItem : public PluginTreeBase
{
public:
    PluginTreeItem (const juce::PluginDescription&);
    PluginTreeItem (const juce::String& uniqueId, const juce::String& name, const juce::String& xmlType, bool isSynth, bool isPlugin);

    tracktion_engine::Plugin::Ptr create (tracktion_engine::Edit&);
    
    juce::String getUniqueName() const override
    {
        if (desc.fileOrIdentifier.startsWith (tracktion_engine::RackType::getRackPresetPrefix()))
            return desc.fileOrIdentifier;

        return desc.createIdentifierString();
    }

    juce::PluginDescription desc;
    juce::String xmlType;
    bool isPlugin = true;

    JUCE_LEAK_DETECTOR (PluginTreeItem)
};
