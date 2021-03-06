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

 /*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

  name:             MainComponent
  version:          0.0.1
  vendor:           GroovemanAndCo
  website:          https://github.com/GroovemanAndCo
  description:      This example simply creates a new project and records from one midi input. It also allows a synth plugin to be added to the track

  dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats, juce_audio_processors, juce_audio_utils,
                    juce_core, juce_data_structures, juce_dsp, juce_events, juce_graphics,
                    juce_gui_basics, juce_gui_extra, juce_osc, tracktion_engine
  exporters:        linux_make, vs2017, xcode_iphone, xcode_mac

  moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1, JUCE_PLUGINHOST_AU=1, JUCE_PLUGINHOST_VST3=1, JUCE_ASIO=1

  type:             Component
  mainClass:        MainComponent

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once
#include <JuceHeader.h>


#include "EditComponent.h"
#include "EngineHelpers.h"
#include "ExtendedUIBehaviour.h"
#include "TransportToolbarFactory.h"


/**
 This is the top level JUCE Component window that is created from the MainWindow instance
*/
class MainComponent : public juce::Component
    , public  juce::Button::Listener
    , public  juce::Slider::Listener
    , private juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent(juce::Component& parent);
    ~MainComponent();
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    void buttonClicked (juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    //==============================================================================

    tracktion_engine::Engine engine{ ProjectInfo::projectName, std::make_unique<ExtendedUIBehaviour>(), nullptr };
    tracktion_engine::SelectionManager selectionManager{ engine };
    std::unique_ptr<tracktion_engine::Edit> edit;
    std::unique_ptr<EditComponent> editComponent;

    juce::TextButton settingsButton{ "Settings" }, pluginsButton{ "Plugins" },
	    newEditButton{ "New" }, playPauseButton{ "Play" }, recordButton{ "Record" }, 
	    showEditButton{ "Show Project" }, newTrackButton{ "New Track" },
		deleteButton{ "Delete" }, clearTracksButton { "Clear Tracks" }, aboutButton{"?"};
	
    juce::Label editNameLabel{ "No Edit Loaded" };
    juce::ToggleButton showWaveformButton{ "Show Waveforms" };

    //==============================================================================
    void setupGUI();

    void updatePlayButton()
    {
        if (edit == nullptr) return;
        playPauseButton.setButtonText(edit->getTransport().isPlaying() ? "Stop" : "Play");
    }

    void updateRecordButtonText()
    {
        if (edit == nullptr) return;
        recordButton.setButtonText(edit->getTransport().isRecording() ? "Abort" : "Record");
    }

    void onRecordTracks(); ///< called when record action is emitted
    void setSongTitle(const juce::String& title); ///< Set the song title in the window caption
    void createOrLoadEdit(juce::File editFile = {}, bool loadOnly=false);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (edit != nullptr && source == &edit->getTransport())
        {
            updatePlayButton();
            updateRecordButtonText();
        }
        else if (source == &selectionManager)
        {
            auto sel = selectionManager.getSelectedObject(0);
            deleteButton.setEnabled(dynamic_cast<tracktion_engine::Clip*> (sel) != nullptr
                || dynamic_cast<tracktion_engine::Track*> (sel) != nullptr
                || dynamic_cast<tracktion_engine::Plugin*> (sel));
        }
    }

    void createTracksAndAssignInputs()
    {
        auto& dm = engine.getDeviceManager();

        for (int i = 0; i < dm.getNumMidiInDevices(); i++)
        {
            if (auto mip = dm.getMidiInDevice(i))
            {
                mip->setEndToEndEnabled(true);
                mip->setEnabled(true);
            }
        }

        edit->getTransport().ensureContextAllocated();

        if (tracktion_engine::getAudioTracks(*edit).size() == 0)
        {
            int trackNum = 0;
            for (auto* instance : edit->getAllInputDevices())
            {
                if (instance->getInputDevice().getDeviceType() == tracktion_engine::InputDevice::physicalMidiDevice)
                {
                    if (auto* t = EngineHelpers::getOrInsertAudioTrackAt(*edit, trackNum))
                    {
                        instance->setTargetTrack(*t, 0, true);
                        instance->setRecordingEnabled(*t, true);

                        trackNum++;
                    }
                }
            }
        }

        edit->restartPlayback();
    }
    
    juce::Toolbar toolbar;

    TransportToolbarItemFactory factory;
    Component& parent; // TODO investigate a better way to get the windows parent for setting the title, maybe decouple it with a callback instead
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
