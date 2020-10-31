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

#include "PluginTreeGroup.h"
#include "PluginTreeItem.h"

//==============================================================================
class PluginMenu : public juce::PopupMenu
{
public:
    PluginMenu() = default;

    PluginMenu (PluginTreeGroup& node)
    {
        for (int i = 0; i < node.getNumSubItems(); ++i)
            if (auto* subNode = dynamic_cast<PluginTreeGroup*> (node.getSubItem (i)))
                addSubMenu (subNode->name, PluginMenu (*subNode), true);

        for (int i = 0; i < node.getNumSubItems(); ++i)
            if (auto* subType = dynamic_cast<PluginTreeItem*> (node.getSubItem (i)))
                addItem (subType->getUniqueName().hashCode(), subType->desc.name, true, false);
    }

    static PluginTreeItem* findType (PluginTreeGroup& node, int hash)
    {
        for (int i = 0; i < node.getNumSubItems(); ++i)
            if (auto* subNode = dynamic_cast<PluginTreeGroup*> (node.getSubItem (i)))
                if (auto* t = findType (*subNode, hash))
                    return t;

        for (int i = 0; i < node.getNumSubItems(); ++i)
            if (auto* t = dynamic_cast<PluginTreeItem*> (node.getSubItem (i)))
                if (t->getUniqueName().hashCode() == hash)
                    return t;

        return nullptr;
    }

    PluginTreeItem* runMenu (PluginTreeGroup& node)
    {
        int res = show();

        if (res == 0)
            return nullptr;

        return findType (node, res);
    }
};

