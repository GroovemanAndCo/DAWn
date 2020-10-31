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
#include "TrackHeaderComponent.h"


#include "EngineHelpers.h"
#include "Utilities.h"

//==============================================================================
TrackHeaderComponent::TrackHeaderComponent (EditViewState& evs, tracktion_engine::Track::Ptr t)
    : editViewState (evs), track (t)
{
    Helpers::addAndMakeVisible (*this, { &trackName, &armButton, &muteButton, &soloButton, &inputButton });
    
    armButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);
    muteButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);
    soloButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::green);

    trackName.setText (t->getName(), juce::dontSendNotification);
    
    if (auto at = dynamic_cast<tracktion_engine::AudioTrack*> (track.get()))
    {
        inputButton.onClick = [this, at]
        {
	        juce::PopupMenu m;
            
            if (EngineHelpers::trackHasInput (*at))
            {
                bool ticked = EngineHelpers::isInputMonitoringEnabled (*at);
                m.addItem (1000, "Input Monitoring", true, ticked);
                m.addSeparator();
            }
            
            if (editViewState.showWaveDevices)
            {
                int id = 1;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == tracktion_engine::InputDevice::waveDevice)
                    {
                        bool ticked = instance->getTargetTracks().getFirst() == at;
                        m.addItem (id++, instance->getInputDevice().getName(), true, ticked);
                    }
                }
            }
            
            if (editViewState.showMidiDevices)
            {
                m.addSeparator();
                
                int id = 100;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == tracktion_engine::InputDevice::physicalMidiDevice)
                    {
                        bool ticked = instance->getTargetTracks().getFirst() == at;
                        m.addItem (id++, instance->getInputDevice().getName(), true, ticked);
                    }
                }
            }

            int res = m.show();

            if (res == 1000)
            {
                EngineHelpers::enableInputMonitoring (*at, ! EngineHelpers::isInputMonitoringEnabled (*at));
            }
            else if (res >= 100)
            {
                int id = 100;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == tracktion_engine::InputDevice::physicalMidiDevice)
                    {
                        if (id == res)
                            instance->setTargetTrack (*at, 0, true);
                        id++;
                    }
                }
            }
            else if (res >= 1)
            {
                int id = 1;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == tracktion_engine::InputDevice::waveDevice)
                    {
                        if (id == res)
                            instance->setTargetTrack (*at, 0, true);
                        id++;
                    }
                }
            }
        };
        armButton.onClick = [this, at]
        {
            EngineHelpers::armTrack (*at, ! EngineHelpers::isTrackArmed (*at));
            armButton.setToggleState (EngineHelpers::isTrackArmed (*at), juce::dontSendNotification);
        };
        muteButton.onClick = [at] { at->setMute (! at->isMuted (false)); };
        soloButton.onClick = [at] { at->setSolo (! at->isSolo (false)); };
        
        armButton.setToggleState (EngineHelpers::isTrackArmed (*at), juce::dontSendNotification);
    }
    else
    {
        armButton.setVisible (false);
        muteButton.setVisible (false);
        soloButton.setVisible (false);
    }
    
    track->state.addListener (this);
    inputsState = track->edit.state.getChildWithName (tracktion_engine::IDs::INPUTDEVICES);
    inputsState.addListener (this);
    
    valueTreePropertyChanged (track->state, tracktion_engine::IDs::mute);
    valueTreePropertyChanged (track->state, tracktion_engine::IDs::solo);
    valueTreePropertyChanged (inputsState, tracktion_engine::IDs::targetIndex);
}

TrackHeaderComponent::~TrackHeaderComponent()
{
    track->state.removeListener (this);
}

void TrackHeaderComponent::valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i)
{
    if (tracktion_engine::TrackList::isTrack (v))
    {
        if (i == tracktion_engine::IDs::mute)
            muteButton.setToggleState ((bool)v[i], juce::dontSendNotification);
        else if (i == tracktion_engine::IDs::solo)
            soloButton.setToggleState ((bool)v[i], juce::dontSendNotification);
    }
    else if (v.hasType (tracktion_engine::IDs::INPUTDEVICES)
             || v.hasType (tracktion_engine::IDs::INPUTDEVICE)
             || v.hasType (tracktion_engine::IDs::INPUTDEVICEDESTINATION))
    {
        if (auto at = dynamic_cast<tracktion_engine::AudioTrack*> (track.get()))
        {
            armButton.setEnabled (EngineHelpers::trackHasInput (*at));
            armButton.setToggleState (EngineHelpers::isTrackArmed (*at), juce::dontSendNotification);
        }
    }
}

void TrackHeaderComponent::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::grey);
    g.fillRect (getLocalBounds().withTrimmedRight (2));
    
    if (editViewState.selectionManager.isSelected (track.get()))
    {
        g.setColour (juce::Colours::red);
        g.drawRect (getLocalBounds().withTrimmedRight (-4), 2);
    }
}

void TrackHeaderComponent::mouseDown (const juce::MouseEvent&)
{
    editViewState.selectionManager.selectOnly (track.get());
}

void TrackHeaderComponent::resized()
{
    auto r = getLocalBounds().reduced (4);
    trackName.setBounds (r.removeFromTop (r.getHeight() / 2));
    
    int w = r.getHeight();
    inputButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    armButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    muteButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    soloButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
}

