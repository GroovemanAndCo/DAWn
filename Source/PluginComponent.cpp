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
#include "PluginComponent.h"
#include "EditViewState.h"

//==============================================================================
PluginComponent::PluginComponent (EditViewState& evs, tracktion_engine::Plugin::Ptr p)
    : editViewState (evs), plugin (p)
{
    setButtonText (plugin->getName().substring (0, 1));
}

PluginComponent::~PluginComponent()
{
}

void PluginComponent::clicked (const juce::ModifierKeys& modifiers)
{
    editViewState.selectionManager.selectOnly (plugin.get());
    if (modifiers.isPopupMenu())
    {
	    juce::PopupMenu m;
        m.addItem ("Delete", [this] { plugin->deleteFromParent(); });
        m.showAt (this);
    }
    else
    {
        plugin->showWindowExplicitly();
    }
}
