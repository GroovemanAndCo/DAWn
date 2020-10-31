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
#include "PluginTreeGroup.h"

#include "PluginTreeItem.h"

//==============================================================================
PluginTreeGroup::PluginTreeGroup (tracktion_engine::Edit& edit, juce::KnownPluginList::PluginTree& tree, tracktion_engine::Plugin::Type types)
    : name ("Plugins")
{
    {
        int num = 1;

        auto builtinFolder = new PluginTreeGroup (TRANS("Builtin Plugins"));
        addSubItem (builtinFolder);
        builtinFolder->createBuiltInItems (num, types);
    }

    {
        auto racksFolder = new PluginTreeGroup (TRANS("Plugin Racks"));
        addSubItem (racksFolder);

        racksFolder->addSubItem (new PluginTreeItem (juce::String (tracktion_engine::RackType::getRackPresetPrefix()) + "-1",
                                                     TRANS("Create New Empty Rack"),
                                                     tracktion_engine::RackInstance::xmlTypeName, false, false));

        int i = 0;
        for (auto rf : edit.getRackList().getTypes())
            racksFolder->addSubItem (new PluginTreeItem ("RACK__" + juce::String (i++), rf->rackName,
                                                         tracktion_engine::RackInstance::xmlTypeName, false, false));
    }

    populateFrom (tree);
}

PluginTreeGroup::PluginTreeGroup (const juce::String& s)  : name (s)
{
    jassert (name.isNotEmpty());
}

void PluginTreeGroup::populateFrom (juce::KnownPluginList::PluginTree& tree)
{
    for (auto subTree : tree.subFolders)
    {
        if (subTree->plugins.size() > 0 || subTree->subFolders.size() > 0)
        {
            auto fs = new PluginTreeGroup (subTree->folder);
            addSubItem (fs);

            fs->populateFrom (*subTree);
        }
    }

    for (const auto& pd : tree.plugins)
        addSubItem (new PluginTreeItem (pd));
}


template<class FilterClass>
void addInternalPlugin (PluginTreeBase& item, int& num, bool synth = false)
{
    item.addSubItem (new PluginTreeItem (juce::String (num++) + "_trkbuiltin",
                                         TRANS (FilterClass::getPluginName()),
                                         FilterClass::xmlTypeName, synth, false));
}

void PluginTreeGroup::createBuiltInItems (int& num, tracktion_engine::Plugin::Type types)
{
    addInternalPlugin<tracktion_engine::VolumeAndPanPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::LevelMeterPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::EqualiserPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::ReverbPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::DelayPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::ChorusPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::PhaserPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::CompressorPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::PitchShiftPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::LowPassPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::MidiModifierPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::MidiPatchBayPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::PatchBayPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::AuxSendPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::AuxReturnPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::TextPlugin> (*this, num);
    addInternalPlugin<tracktion_engine::FreezePointPlugin> (*this, num);

   #if TRACKTION_ENABLE_REWIRE
    addInternalPlugin<tracktion_engine::ReWirePlugin> (*this, num, true);
   #endif

    if (types == tracktion_engine::Plugin::Type::allPlugins)
    {
        addInternalPlugin<tracktion_engine::SamplerPlugin> (*this, num, true);
        addInternalPlugin<tracktion_engine::FourOscPlugin> (*this, num, true);
    }

    addInternalPlugin<tracktion_engine::InsertPlugin> (*this, num);

   #if ENABLE_INTERNAL_PLUGINS
    for (auto& d : PluginTypeBase::getAllPluginDescriptions())
        if (isPluginAuthorised (d))
            addSubItem (new PluginTreeItem (d));
   #endif
}
