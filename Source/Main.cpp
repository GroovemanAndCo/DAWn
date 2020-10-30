/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "Main.h"

using namespace juce;

//==============================================================================

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (DAWnApplication)


void DAWnApplication::parseCommandLine(const String& commandLine)
{
    // TODO implement autoload of songs by command line parameters
}

void DAWnApplication::initialise(const juce::String& commandLine) 
{
    // This method is where you should put your application's initialisation code..
    parseCommandLine(commandLine);
    mainWindow.reset(new MainWindow(getApplicationName()));
}

void DAWnApplication::shutdown()
{
    mainWindow = nullptr; // (deletes our window)
}

void DAWnApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
}

void DAWnApplication::anotherInstanceStarted(const juce::String& commandLine)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.

    // TODO Implement load song functionality here
}
