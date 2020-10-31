#pragma once
#include <JuceHeader.h>
#include "EditViewState.h"

//==============================================================================
class PluginComponent : public juce::TextButton
{
public:
    PluginComponent (EditViewState&, tracktion_engine::Plugin::Ptr);
    ~PluginComponent();
    
    void clicked (const juce::ModifierKeys& modifiers) override;
    
private:
    EditViewState& editViewState;
    tracktion_engine::Plugin::Ptr plugin;
};
