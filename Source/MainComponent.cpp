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

#include "MainComponent.h"

using namespace juce;

MainComponent::MainComponent( Component& par) : factory(this), parent(par)
{
	
    settingsButton.onClick = [this] { EngineHelpers::showAudioDeviceSettings(engine); };
    pluginsButton.onClick = [this]
    {
        DialogWindow::LaunchOptions o;
        o.dialogTitle = TRANS("Plugins");
        o.dialogBackgroundColour = Colours::black;
        o.escapeKeyTriggersCloseButton = true;
        o.useNativeTitleBar = true;
        o.resizable = true;
        o.useBottomRightCornerResizer = true;

        auto v = new PluginListComponent(engine.getPluginManager().pluginFormatManager,
            engine.getPluginManager().knownPluginList,
            engine.getTemporaryFileManager().getTempFile("PluginScanDeadMansPedal"),
            tracktion_engine::getApplicationSettings());
        v->setSize(800, 600);
        o.content.setOwned(v);
        o.launchAsync();
    };
    newEditButton.onClick = [this] { createOrLoadEdit(); };

	updatePlayButton();
    updateRecordButtonText();

    editNameLabel.setJustificationType(Justification::centred);

	Helpers::addAndMakeVisible(*this, {
		&settingsButton, &pluginsButton, /* &newEditButton, &playPauseButton, */ 
		&showEditButton,/* &recordButton, */ &newTrackButton,
		&deleteButton, /*&editNameLabel,*/ &clearTracksButton, &aboutButton });

    deleteButton.setEnabled(false);

    auto d = File::getSpecialLocation(File::tempDirectory).getChildFile("MainComponent");
    d.createDirectory();

    auto f = Helpers::findRecentEdit(d);
    if (f.existsAsFile())
        createOrLoadEdit(f);
    else
        createOrLoadEdit(d.getNonexistentChildFile("Test", ".tracktionedit", false));

    selectionManager.addChangeListener(this);

    setupGUI();

    setSize(600, 400);
}

MainComponent::~MainComponent()
{
    tracktion_engine::EditFileOperations(*edit).save(true, true, false);
    engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
}

void MainComponent::paint(Graphics& g)
{
	auto col = // getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
	        juce::Colour(0xff101525); 
    g.fillAll(col);
}

void MainComponent::resized()
{
    const auto toolbarThickness = 32;

    auto r = getLocalBounds();
    r.removeFromTop(toolbarThickness);
    auto topR = r.removeFromTop(30);
	auto w = r.getWidth() / 7;

	toolbar.setBounds(toolbar.isVertical() ? getLocalBounds().removeFromLeft(toolbarThickness) : getLocalBounds().removeFromTop(toolbarThickness));

    settingsButton.setBounds(topR.removeFromLeft(w).reduced(2));
    pluginsButton.setBounds(topR.removeFromLeft(w).reduced(2));
	// newEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
	// playPauseButton.setBounds(topR.removeFromLeft(w).reduced(2));
	// recordButton.setBounds(topR.removeFromLeft(w).reduced(2)); 
    showEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
    newTrackButton.setBounds(topR.removeFromLeft(w).reduced(2));
    deleteButton.setBounds(topR.removeFromLeft(w).reduced(2));
	clearTracksButton.setBounds(topR.removeFromLeft(w).reduced(2));
    aboutButton.setBounds(topR.removeFromLeft(w).reduced(2));
    topR = r.removeFromTop(30);
    editNameLabel.setBounds(topR);

	if (editComponent != nullptr) editComponent->setBounds(r);
}

/** Called when the button's state changes. */
//virtual void buttonStateChanged (Button*)  {}
void MainComponent::setupGUI()
{
    // Create and add the toolbar...
    addAndMakeVisible(toolbar);

    // And use our item factory to add a set of default icons to it...
    toolbar.addDefaultItems(factory);

    playPauseButton.onClick = [this]
    {
        EngineHelpers::togglePlay(*edit);
    };

	recordButton.onClick = [this]	{ onRecordTracks();	}; 

	newTrackButton.onClick = [this]
    {
        edit->ensureNumberOfAudioTracks(getAudioTracks(*edit).size() + 1);
    };

	deleteButton.onClick = [this]
    {
        auto* sel = selectionManager.getSelectedObject(0);
        if (auto* clip = dynamic_cast<tracktion_engine::Clip*> (sel))
        {
            clip->removeFromParentTrack();
        }
        else if (auto* track = dynamic_cast<tracktion_engine::Track*> (sel))
        {
            if (!(track->isMarkerTrack() || track->isTempoTrack() || track->isChordTrack()))
                edit->deleteTrack(track);
        }
        else if (auto* plugin = dynamic_cast<tracktion_engine::Plugin*> (sel))
        {
            plugin->deleteFromParent();
        }
    };
	
    showWaveformButton.onClick = [this]
    {
        auto& evs = editComponent->getEditViewState();
        evs.drawWaveforms = !evs.drawWaveforms.get();
        showWaveformButton.setToggleState(evs.drawWaveforms, dontSendNotification);
    };

	clearTracksButton.onClick = [this]
	{
		const auto userIsSure = engine.getUIBehaviour()
			.showOkCancelAlertBox (TRANS("MIDI Clip"),
				TRANS("Are you sure you want clear all tracks?"),
				TRANS("Clear them all"),
				TRANS("Ignore")) == 1;
		if (!userIsSure) return;
		for (auto* t : tracktion_engine::getAudioTracks(*edit)) edit->deleteTrack(t);
	};
	
	aboutButton.onClick = [this]
	{
        AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
              TRANS(String("About DAWn version ") + ProjectInfo::versionString + " ..."),
              TRANS(
                  "DAWn stands for Digital Audio Workstation Next-gen.                                      \n\n"
                  "Copyright (c) 2020 Fabien (https://github.com/fab672000).                                \n"
                  "Powered by the trackion engine framework (https://github.com/Tracktion/tracktion_engine).\n"
                  "                                                                                         \n"
                  "This program is free software: you can redistribute it and/or modify                     \n"
                  "it under the terms of the GNU General Public License as published by                     \n"
                  "the Free Software Foundation, version 3.                                                 \n"
                  "                                                                                         \n"
                  "This program is distributed in the hope that it will be useful, but                      \n"
                  "WITHOUT ANY WARRANTY; without even the implied warranty of                               \n"
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU                         \n"
                  "General Public License for more details.                                                 \n"
                  "                                                                                         \n"
                  "You should have received a copy of the GNU General Public License                        \n"
                  "along with this program. If not, see <http://www.gnu.org/licenses/>.                     \n"

              ));
	};
}

/** Called when the button is clicked. */
void MainComponent::buttonClicked(juce::Button* button)
{
	const auto name = button->getName().toLowerCase();
	if (name == "new")  createOrLoadEdit();
    else if (name == "open") createOrLoadEdit(File{}, true);
	else if (name == "save")
	{
		tracktion_engine::EditFileOperations(*edit).save(true, true, false);
		TRACKTION_LOG("Edit File " + edit->getName() + " saved.");
	}
	else if (name == "save as")
	{
		juce::FileChooser fc("Save As...",
			juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
			"*.tracktionedit");
		if (fc.browseForFileToSave(true))
		{
			auto editFile = fc.getResult();
			tracktion_engine::EditFileOperations(*edit).saveAs(editFile);
			TRACKTION_LOG("Edit file saved as " + edit->getName() + " saved.");
		}

	}
	else if (name == "start")
	{ // pay/pause
		// TODO change icon to pause icon after starting and vice-versa
		EngineHelpers::togglePlay(*edit);
	}
	else if (name == "stop") EngineHelpers::stop(*edit);
	else if (name == "record") onRecordTracks();
	else
	{
		TRACKTION_LOG(juce::String("Unknown Button " + button->getName() + " pressed"));
	}
}

void MainComponent::onRecordTracks()
{
	const auto wasRecording = edit->getTransport().isRecording();
	EngineHelpers::toggleRecord(*edit);
	if (wasRecording) tracktion_engine::EditFileOperations(*edit).save(true, true, false);
}

void MainComponent::setSongTitle(const juce::String& title )
{
    parent.setName ("DAWn Project - " + title );
}

void MainComponent::createOrLoadEdit(juce::File editFile, bool loadOnly)
{
    if (editFile == juce::File())
    {
        auto title = juce::String(loadOnly ? "Load" : "New") + " Project";
        juce::FileChooser fc(title, juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.tracktionedit");
        auto result = loadOnly ? fc.browseForFileToOpen(): fc.browseForFileToSave(false);
		if (result)
            editFile = fc.getResult();
        else
            return;
    }

    selectionManager.deselectAll();
    editComponent = nullptr;

    if (editFile.existsAsFile())
        edit = tracktion_engine::loadEditFromFile(engine, editFile);
    else
        edit = tracktion_engine::createEmptyEdit(engine, editFile);

    edit->editFileRetriever = [editFile] { return editFile; };
    edit->playInStopEnabled = true;

    auto& transport = edit->getTransport();
    transport.addChangeListener(this);
	setSongTitle(editFile.getFileNameWithoutExtension() );
    // editNameLabel.setText(editFile.getFileNameWithoutExtension(), juce::dontSendNotification);
    showEditButton.onClick = [this, editFile]
    {
        tracktion_engine::EditFileOperations(*edit).save(true, true, false);
        editFile.revealToUser();
    };

    createTracksAndAssignInputs();

    tracktion_engine::EditFileOperations(*edit).save(true, true, false);

    editComponent = std::make_unique<EditComponent>(*edit, selectionManager);
    editComponent->getEditViewState().showFooters = true;
    editComponent->getEditViewState().showMidiDevices = true;
	editComponent->getEditViewState().showWaveDevices = true;

    addAndMakeVisible(*editComponent);
}

