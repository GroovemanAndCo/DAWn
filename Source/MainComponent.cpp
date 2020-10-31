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

    setupButtons();

    setSize(600, 400);
}

MainComponent::~MainComponent()
{
    tracktion_engine::EditFileOperations(*edit).save(true, true, false);
    engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
}