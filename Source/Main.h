#pragma once
#include <JuceHeader.h>
#include <JuceHeader.h>
#include "MainWindow.h"

class DAWnApplication : public juce::JUCEApplication
{
    void parseCommandLine(const juce::String& commandLine);

public:
    //==============================================================================
    DAWnApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    /// Application initialization code
    void initialise(const juce::String& commandLine) override;

    /// Application shutdown code
    void shutdown() override;
    //==============================================================================
    void systemRequestedQuit() override;
    void anotherInstanceStarted(const juce::String& commandLine) override;

private:
    std::unique_ptr<MainWindow> mainWindow;
};