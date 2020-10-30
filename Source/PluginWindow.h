#pragma once
#include <JuceHeader.h>

//==============================================================================
class PluginEditor : public juce::Component
{
public:
    virtual bool allowWindowResizing() = 0;
    virtual juce::ComponentBoundsConstrainer* getBoundsConstrainer() = 0;
};

//==============================================================================
struct AudioProcessorEditorContentComp : public PluginEditor
{
    AudioProcessorEditorContentComp (tracktion_engine::ExternalPlugin& plug) : plugin (plug)
    {
        JUCE_AUTORELEASEPOOL
        {
            if (auto pi = plugin.getAudioPluginInstance())
            {
                editor.reset (pi->createEditorIfNeeded());

                if (editor == nullptr)
                    editor = std::make_unique<juce::GenericAudioProcessorEditor> (*pi);

                addAndMakeVisible (*editor);
            }
        }

        resizeToFitEditor (true);
    }

    bool allowWindowResizing() override { return false; }

    juce::ComponentBoundsConstrainer* getBoundsConstrainer() override
    {
        if (editor == nullptr || allowWindowResizing())
            return {};

        return editor->getConstrainer();
    }

    void resized() override
    {
        if (editor != nullptr)
            editor->setBounds (getLocalBounds());
    }

    void childBoundsChanged (juce::Component* c) override
    {
        if (c == editor.get())
        {
            plugin.edit.pluginChanged (plugin);
            resizeToFitEditor();
        }
    }

    void resizeToFitEditor (bool force = false)
    {
        if (force || ! allowWindowResizing())
            setSize (juce::jmax (8, editor != nullptr ? editor->getWidth() : 0), juce::jmax (8, editor != nullptr ? editor->getHeight() : 0));
    }

    tracktion_engine::ExternalPlugin& plugin;
    std::unique_ptr<juce::AudioProcessorEditor> editor;

    AudioProcessorEditorContentComp() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorEditorContentComp)
};

//=============================================================================
class PluginWindow : public juce::DocumentWindow
{
public:
    PluginWindow (tracktion_engine::Plugin&);
    ~PluginWindow() override;

    static std::unique_ptr<Component> create (tracktion_engine::Plugin&);

    void show();

    void setEditor (std::unique_ptr<PluginEditor>);
    PluginEditor* getEditor() const         { return editor.get(); }
    
    void recreateEditor();
    void recreateEditorAsync();

    static bool isDPIAware(tracktion_engine::Plugin&)
    {
        // You should keep a DB of if plugins are DPI aware or not and recall that value
        // here. You should let the user toggle the value if the plugin appears tiny
        return true;
    }


private:
    void moved() override;
    void userTriedToCloseWindow() override          { plugin.windowState->closeWindowExplicitly(); }
    void closeButtonPressed() override              { userTriedToCloseWindow(); }
    float getDesktopScaleFactor() const override    { return 1.0f; }

    std::unique_ptr<PluginEditor> createContentComp();

    std::unique_ptr<PluginEditor> editor;
    
    tracktion_engine::Plugin& plugin;
    tracktion_engine::PluginWindowState& windowState;
};

//==============================================================================
#if JUCE_LINUX
 constexpr bool shouldAddPluginWindowToDesktop = false;
#else
 constexpr bool shouldAddPluginWindowToDesktop = true;
#endif


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
