/*
    ,--.                     ,--.     ,--.  ,--.
  ,-'  '-.,--.--.,--,--.,---.|  |,-.,-'  '-.`--' ,---. ,--,--,      Copyright 2018
  '-.  .-'|  .--' ,-.  | .--'|     /'-.  .-',--.| .-. ||      \   Tracktion Software
    |  |  |  |  \ '-'  \ `--.|  \  \  |  |  |  |' '-' '|  ||  |       Corporation
    `---' `--'   `--`--'`---'`--'`--' `---' `--' `---' `--''--'    www.tracktion.com
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================
namespace Helpers
{
    static inline void addAndMakeVisible (juce::Component& parent, const juce::Array<juce::Component*>& children)
    {
        for (auto c : children)
            parent.addAndMakeVisible (c);
    }

    static inline juce::String getStringOrDefault (const juce::String& stringToTest, const juce::String& stringToReturnIfEmpty)
    {
        return stringToTest.isEmpty() ? stringToReturnIfEmpty : stringToTest;
    }
    
    static inline juce::File findRecentEdit (const juce::File& dir)
    {
        auto files = dir.findChildFiles (juce::File::findFiles, false, "*.tracktionedit");
        
        if (files.size() > 0)
        {
            files.sort();
            return files.getLast();
        }
        
        return {};
    }
}

//==============================================================================
namespace PlayHeadHelpers
{
    // Quick-and-dirty function to format a timecode string
    static inline juce::String timeToTimecodeString (double seconds)
    {
        auto millisecs = juce::roundToInt (seconds * 1000.0);
        auto absMillisecs = std::abs (millisecs);

        return juce::String::formatted ("%02d:%02d:%02d.%03d",
                                  millisecs / 3600000,
                                  (absMillisecs / 60000) % 60,
                                  (absMillisecs / 1000)  % 60,
                                  absMillisecs % 1000);
    }

    // Quick-and-dirty function to format a bars/beats string
    static inline juce::String quarterNotePositionToBarsBeatsString (double quarterNotes, int numerator, int denominator)
    {
        if (numerator == 0 || denominator == 0)
            return "1|1|000";

        auto quarterNotesPerBar = ((double) numerator * 4.0 / (double) denominator);
        auto beats  = (fmod (quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

        auto bar    = ((int) quarterNotes) / quarterNotesPerBar + 1;
        auto beat   = ((int) beats) + 1;
        auto ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

        return juce::String::formatted ("%d|%d|%03d", bar, beat, ticks);
    }

    // Returns a textual description of a CurrentPositionInfo
    static inline juce::String getTimecodeDisplay (const juce::AudioPlayHead::CurrentPositionInfo& pos)
    {
        juce::MemoryOutputStream displayText;

        displayText << juce::String (pos.bpm, 2) << " bpm, "
                    << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                    << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                    << "  -  " << quarterNotePositionToBarsBeatsString (pos.ppqPosition,
                                                                        pos.timeSigNumerator,
                                                                        pos.timeSigDenominator);

        if (pos.isRecording)
            displayText << "  (recording)";
        else if (pos.isPlaying)
            displayText << "  (playing)";
        else
            displayText << "  (stopped)";

        return displayText.toString();
    }
}


//==============================================================================
class FlaggedAsyncUpdater : public juce::AsyncUpdater
{
public:
    //==============================================================================
    void markAndUpdate (bool& flag)     { flag = true; triggerAsyncUpdate(); }
    
    bool compareAndReset (bool& flag) noexcept
    {
        if (! flag)
            return false;
        
        flag = false;
        return true;
    }
};

//==============================================================================
struct Thumbnail    : public juce::Component
{
    Thumbnail (tracktion_engine::TransportControl& tc)
        : transport (tc)
    {
        cursorUpdater.setCallback ([this]
                                   {
                                       updateCursorPosition();

                                       if (smartThumbnail.isGeneratingProxy() || smartThumbnail.isOutOfDate())
                                           repaint();
                                   });
        cursor.setFill (findColour (juce::Label::textColourId));
        addAndMakeVisible (cursor);
    }

    void setFile (const tracktion_engine::AudioFile& file)
    {
        smartThumbnail.setNewFile (file);
        cursorUpdater.startTimerHz (25);
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto r = getLocalBounds();
        const auto colour = findColour (juce::Label::textColourId);

        if (smartThumbnail.isGeneratingProxy())
        {
            g.setColour (colour.withMultipliedBrightness (0.9f));
            g.drawText ("Creating proxy: " + juce::String (juce::roundToInt (smartThumbnail.getProxyProgress() * 100.0f)) + "%",
                        r, juce::Justification::centred);

        }
        else
        {
            const float brightness = smartThumbnail.isOutOfDate() ? 0.4f : 0.66f;
            g.setColour (colour.withMultipliedBrightness (brightness));
            smartThumbnail.drawChannels (g, r, true, { 0.0, smartThumbnail.getTotalLength() }, 1.0f);
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        transport.setUserDragging (true);
        mouseDrag (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        jassert (getWidth() > 0);
        const float proportion = e.position.x / getWidth();
        transport.position = proportion * transport.getLoopRange().getLength();
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        transport.setUserDragging (false);
    }

private:
    tracktion_engine::TransportControl& transport;
    tracktion_engine::SmartThumbnail smartThumbnail { transport.engine, tracktion_engine::AudioFile (transport.engine), *this, nullptr };
    juce::DrawableRectangle cursor;
    tracktion_engine::LambdaTimer cursorUpdater;

    void updateCursorPosition()
    {
        const double loopLength = transport.getLoopRange().getLength();
        const double proportion = loopLength == 0.0 ? 0.0 : transport.getCurrentPosition() / loopLength;

        auto r = getLocalBounds().toFloat();
        const float x = r.getWidth() * float (proportion);
        cursor.setRectangle (r.withWidth (2.0f).withX (x));
    }
};
