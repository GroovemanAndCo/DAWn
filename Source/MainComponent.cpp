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

MainComponent::MainComponent()
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
            te::getApplicationSettings());
        v->setSize(800, 600);
        o.content.setOwned(v);
        o.launchAsync();
    };
    newEditButton.onClick = [this] { createOrLoadEdit(); };

    updatePlayButtonText();
    updateRecordButtonText();

    editNameLabel.setJustificationType(Justification::centred);

    Helpers::addAndMakeVisible(*this, { &settingsButton, &pluginsButton, &newEditButton, &playPauseButton, &showEditButton,
                                         &recordButton, &newTrackButton, &deleteButton, &editNameLabel });

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
    auto col = getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
    // auto col = Colour(0x00244249);
    g.fillAll(col);
}

void MainComponent::resized()
{
    const auto toolbarThickness = 32;

    auto r = getLocalBounds();
    auto w = r.getWidth() / 7;
    auto topTool = r.removeFromTop(toolbarThickness);
    auto topR = r.removeFromTop(30);

    toolbar.setBounds( toolbar.isVertical() ? getLocalBounds().removeFromLeft(toolbarThickness) : getLocalBounds().removeFromTop(toolbarThickness));

    settingsButton.setBounds(topR.removeFromLeft(w).reduced(2));
    pluginsButton.setBounds(topR.removeFromLeft(w).reduced(2));
    newEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
    playPauseButton.setBounds(topR.removeFromLeft(w).reduced(2));
    recordButton.setBounds(topR.removeFromLeft(w).reduced(2));
    showEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
    newTrackButton.setBounds(topR.removeFromLeft(w).reduced(2));
    deleteButton.setBounds(topR.removeFromLeft(w).reduced(2));
    topR = r.removeFromTop(30);
    editNameLabel.setBounds(topR);

    if (editComponent != nullptr)
        editComponent->setBounds(r);
}

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
    recordButton.onClick = [this]
    {
        bool wasRecording = edit->getTransport().isRecording();
        EngineHelpers::toggleRecord(*edit);
        if (wasRecording)
            te::EditFileOperations(*edit).save(true, true, false);
    };
    newTrackButton.onClick = [this]
    {
        edit->ensureNumberOfAudioTracks(getAudioTracks(*edit).size() + 1);
    };
    deleteButton.onClick = [this]
    {
        auto sel = selectionManager.getSelectedObject(0);
        if (auto clip = dynamic_cast<te::Clip*> (sel))
        {
            clip->removeFromParentTrack();
        }
        else if (auto track = dynamic_cast<te::Track*> (sel))
        {
            if (!(track->isMarkerTrack() || track->isTempoTrack() || track->isChordTrack()))
                edit->deleteTrack(track);
        }
        else if (auto plugin = dynamic_cast<te::Plugin*> (sel))
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
}
