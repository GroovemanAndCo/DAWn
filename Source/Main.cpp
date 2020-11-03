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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.\
 *
 */

 /*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.
  ==============================================================================
*/

#include "Main.h"

using namespace juce;

//==============================================================================
    
/// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (DAWnApplication)


void DAWnApplication::parseCommandLine(const String& /* commandLine */)
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

void DAWnApplication::anotherInstanceStarted(const juce::String& /* commandLine */)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.

    // TODO Implement load song functionality here
}
