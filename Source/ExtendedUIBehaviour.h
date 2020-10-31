#pragma once
#include <JuceHeader.h>
#include "PluginWindow.h"

//==============================================================================
class ExtendedUIBehaviour : public tracktion_engine::UIBehaviour
{
public:
    ExtendedUIBehaviour() = default;
    
    std::unique_ptr<juce::Component> createPluginWindow (tracktion_engine::PluginWindowState& pws) override
    {
        if (auto ws = dynamic_cast<tracktion_engine::Plugin::WindowState*> (&pws))
            return PluginWindow::create (ws->plugin);

        return {};
    }

    void recreatePluginWindowContentAsync (tracktion_engine::Plugin& p) override
    {
        if (auto* w = dynamic_cast<PluginWindow*> (p.windowState->pluginWindow.get()))
            return w->recreateEditorAsync();

        UIBehaviour::recreatePluginWindowContentAsync (p);
    }
};
