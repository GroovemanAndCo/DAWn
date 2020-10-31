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
  description:      This example simply creates a new project and records from one midi input. It also allows a synth plgin to be added to the track

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

#include "Utilities.h"
#include "Components.h"
#include "PluginWindow.h"

#include "EngineHelpers.h"
#include "TransportToolbarFactory.h"


//==============================================================================
class MainComponent : public juce::Component, private juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    te::Engine engine{ ProjectInfo::projectName, std::make_unique<ExtendedUIBehaviour>(), nullptr };
    te::SelectionManager selectionManager{ engine };
    std::unique_ptr<te::Edit> edit;
    std::unique_ptr<EditComponent> editComponent;

    TextButton settingsButton{ "Settings" }, pluginsButton{ "Plugins" }, newEditButton{ "New" }, playPauseButton{ "Play" },
        showEditButton{ "Show Edit" }, newTrackButton{ "New Track" }, deleteButton{ "Delete" }, recordButton{ "Record" };
    Label editNameLabel{ "No Edit Loaded" };
    ToggleButton showWaveformButton{ "Show Waveforms" };

    //==============================================================================
    void setupGUI();

    void updatePlayButtonText()
    {
        if (edit != nullptr)
            playPauseButton.setButtonText(edit->getTransport().isPlaying() ? "Stop" : "Play");
    }

    void updateRecordButtonText()
    {
        if (edit != nullptr)
            recordButton.setButtonText(edit->getTransport().isRecording() ? "Abort" : "Record");
    }

    void createOrLoadEdit(File editFile = {})
    {
        if (editFile == File())
        {
            FileChooser fc("New Edit", File::getSpecialLocation(File::userDocumentsDirectory), "*.tracktionedit");
            if (fc.browseForFileToSave(true))
                editFile = fc.getResult();
            else
                return;
        }

        selectionManager.deselectAll();
        editComponent = nullptr;

        if (editFile.existsAsFile())
            edit = te::loadEditFromFile(engine, editFile);
        else
            edit = te::createEmptyEdit(engine, editFile);

        edit->editFileRetriever = [editFile] { return editFile; };
        edit->playInStopEnabled = true;

        auto& transport = edit->getTransport();
        transport.addChangeListener(this);

        editNameLabel.setText(editFile.getFileNameWithoutExtension(), dontSendNotification);
        showEditButton.onClick = [this, editFile]
        {
            te::EditFileOperations(*edit).save(true, true, false);
            editFile.revealToUser();
        };

        createTracksAndAssignInputs();

        te::EditFileOperations(*edit).save(true, true, false);

        editComponent = std::make_unique<EditComponent>(*edit, selectionManager);
        editComponent->getEditViewState().showFooters = true;
        editComponent->getEditViewState().showMidiDevices = true;
        editComponent->getEditViewState().showWaveDevices = false;

        addAndMakeVisible(*editComponent);
    }

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        if (edit != nullptr && source == &edit->getTransport())
        {
            updatePlayButtonText();
            updateRecordButtonText();
        }
        else if (source == &selectionManager)
        {
            auto sel = selectionManager.getSelectedObject(0);
            deleteButton.setEnabled(dynamic_cast<te::Clip*> (sel) != nullptr
                || dynamic_cast<te::Track*> (sel) != nullptr
                || dynamic_cast<te::Plugin*> (sel));
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

        if (te::getAudioTracks(*edit).size() == 0)
        {
            int trackNum = 0;
            for (auto instance : edit->getAllInputDevices())
            {
                if (instance->getInputDevice().getDeviceType() == te::InputDevice::physicalMidiDevice)
                {
                    if (auto t = EngineHelpers::getOrInsertAudioTrackAt(*edit, trackNum))
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
