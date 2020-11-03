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
#include "MainWindow.h"

/**
  The main DAWn JUCEApplication derived class.
*/
class DAWnApplication : public juce::JUCEApplication
{
    void parseCommandLine(const juce::String& commandLine); ///< parse command line arguments 

public:
    DAWnApplication() {} ///< Add initialization code that must execute prior to initialise() here

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }

    /// Called when another instance of the app is launched while this one is running
    bool moreThanOneInstanceAllowed() override { return true; }

    /// Parse command line parameters and then create the main window
    void initialise(const juce::String& commandLine) override;

    /// Deletes the mainWindow unique ptr, add news cleanup code here
    void shutdown() override;

    /// called when system asks us to quit, reply with calling quit() if ok to quit
    void systemRequestedQuit() override; 

	/** When another instance of the app is launched while this one is running,
     *  this method is invoked, and the commandLine parameter tells you what the other instance's command-line arguments were.
     *  can be used to load a new song  when the user double clicks on a song
     */
    void anotherInstanceStarted(const juce::String& commandLine) override; //

private:
    std::unique_ptr<MainWindow> mainWindow;
};