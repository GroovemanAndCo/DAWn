#pragma once
#include <JuceHeader.h>
#include "IDs.h"

//==============================================================================
class EditViewState
{
public:
    EditViewState (tracktion_engine::Edit& e, tracktion_engine::SelectionManager& s)
        : edit (e), selectionManager (s)
    {
        state = edit.state.getOrCreateChildWithName (IDs::EDITVIEWSTATE, nullptr);
        
        auto um = &edit.getUndoManager();
        
        showGlobalTrack.referTo (state, IDs::showGlobalTrack, um, false);
        showMarkerTrack.referTo (state, IDs::showMarkerTrack, um, false);
        showChordTrack.referTo (state, IDs::showChordTrack, um, false);
        showArrangerTrack.referTo (state, IDs::showArranger, um, false);
        drawWaveforms.referTo (state, IDs::drawWaveforms, um, true);
        showHeaders.referTo (state, IDs::showHeaders, um, true);
        showFooters.referTo (state, IDs::showFooters, um, false);
        showMidiDevices.referTo (state, IDs::showMidiDevices, um, false);
        showWaveDevices.referTo (state, IDs::showWaveDevices, um, true);

        viewX1.referTo (state, IDs::viewX1, um, 0);
        viewX2.referTo (state, IDs::viewX2, um, 60);
        viewY.referTo (state, IDs::viewY, um, 0);
    }
    
    int timeToXPosition (double time, int width) const
    {
        return juce::roundToInt (((time - viewX1) * width) / (viewX2 - viewX1));
    }
    
    double xPositionToTime (int x, int width) const
    {
        return (double (x) / width) * (viewX2 - viewX1) + viewX1;
    }
    
    double beatToTime (double b) const
    {
        auto& ts = edit.tempoSequence;
        return ts.beatsToTime (b);
    }
    
    tracktion_engine::Edit& edit;
    tracktion_engine::SelectionManager& selectionManager;

    juce::CachedValue<bool> showGlobalTrack, showMarkerTrack, showChordTrack, showArrangerTrack,
                            drawWaveforms, showHeaders, showFooters, showMidiDevices, showWaveDevices;

    juce::CachedValue<double> viewX1, viewX2, viewY;

    juce::ValueTree state;
};

