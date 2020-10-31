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

#include <JuceHeader.h>
#include "PluginWindow.h"

using namespace juce;

PluginWindow::PluginWindow(tracktion_engine::Plugin& plug)
    : DocumentWindow(plug.getName(), Colours::black, DocumentWindow::closeButton, shouldAddPluginWindowToDesktop),
    plugin(plug), windowState(*plug.windowState)
{
    getConstrainer()->setMinimumOnscreenAmounts(0x10000, 50, 30, 50);

    auto position = plugin.windowState->lastWindowBounds.getPosition();
    setBounds(getLocalBounds() + position);

    setResizeLimits(100, 50, 4000, 4000);
    setBoundsConstrained(getLocalBounds() + position);

    recreateEditor();

#if JUCE_LINUX
    setAlwaysOnTop(true);
    addToDesktop();
#endif
}

PluginWindow::~PluginWindow()
{
    plugin.edit.flushPluginStateIfNeeded(plugin);
    setEditor(nullptr);
}

void PluginWindow::show()
{
    setVisible(true);
    toFront(false);
    setBoundsConstrained(getBounds());
}

void PluginWindow::setEditor(std::unique_ptr<PluginEditor> newEditor)
{
    JUCE_AUTORELEASEPOOL
    {
        setConstrainer(nullptr);
        editor.reset();

        if (newEditor != nullptr)
        {
            editor = std::move(newEditor);
            setContentNonOwned(editor.get(), true);
        }

        setResizable(editor == nullptr || editor->allowWindowResizing(), false);

        if (editor != nullptr && editor->allowWindowResizing())
            setConstrainer(editor->getBoundsConstrainer());
    }
}

std::unique_ptr<Component> PluginWindow::create(tracktion_engine::Plugin& plugin)
{
    if (auto externalPlugin = dynamic_cast<tracktion_engine::ExternalPlugin*> (&plugin))
        if (externalPlugin->getAudioPluginInstance() == nullptr)
            return nullptr;

    std::unique_ptr<PluginWindow> w;

    {
        struct Blocker : public Component { void inputAttemptWhenModal() override {} };

        Blocker blocker;
        blocker.enterModalState(false);

#if JUCE_WINDOWS && JUCE_WIN_PER_MONITOR_DPI_AWARE
        if (!isDPIAware(plugin))
        {
            ScopedDPIAwarenessDisabler disableDPIAwareness;
            w = std::make_unique<PluginWindow>(plugin);
        }
        else
#endif
        {
            w = std::make_unique<PluginWindow>(plugin);
        }
    }

    if (w == nullptr || w->getEditor() == nullptr)
        return {};

    w->show();

    return w;
}

std::unique_ptr<PluginEditor> PluginWindow::createContentComp()
{
    if (auto ex = dynamic_cast<tracktion_engine::ExternalPlugin*> (&plugin))
        return std::make_unique<AudioProcessorEditorContentComp>(*ex);

    return nullptr;
}

void PluginWindow::recreateEditor()
{
    setEditor(nullptr);
    setEditor(createContentComp());
}

void PluginWindow::recreateEditorAsync()
{
    setEditor(nullptr);

    Timer::callAfterDelay(50, [this, sp = SafePointer<Component>(this)]
        {
            if (sp != nullptr)
                recreateEditor();
        });
}

void PluginWindow::moved()
{
    plugin.windowState->lastWindowBounds = getBounds();
    plugin.edit.pluginChanged(plugin);
}
