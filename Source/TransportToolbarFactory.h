#pragma once
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
#include <JuceHeader.h>

/**
* Transport toolbar for the DAW main window
*/
class TransportToolbarItemFactory : public juce::ToolbarItemFactory
{
public:
    TransportToolbarItemFactory() {}

    //==============================================================================
    // Each type of item a toolbar can contain must be given a unique ID. These
    // are the ones we'll use in this demo.
    enum TransportToolbarItemIds
    {
        doc_new = 1,
        doc_open = 2,
        doc_save = 3,
        doc_saveAs = 4,

        edit_copy = 11,
        edit_cut  = 12,
        edit_paste     = 13,

        media_start    = 21,
        media_record   = 22,
        media_stop     = 23, 
        media_pause    = 24,
        media_forward  = 25,
        media_backward = 26

    };

    void getAllToolbarItemIds(juce::Array<int>& ids) override
    {
        // This returns the complete list of all item IDs that are allowed to
        // go in our toolbar. Any items you might want to add must be listed here. The
        // order in which they are listed will be used by the toolbar customisation panel.

        ids.add(doc_new        );
        ids.add(doc_open       );
        ids.add(doc_save       );
        ids.add(doc_saveAs     );
        
        ids.add(spacerId       );
 
        //ids.add(media_backward );
        ids.add(media_stop     );
        ids.add(media_start    );
        ids.add(media_record   );
        //ids.add(media_pause    );
        //ids.add(media_forward);

        //ids.add(edit_copy      );
        //ids.add(edit_cut       );
        //ids.add(edit_paste     );
        // If you're going to use separators, then they must also be added explicitly
        // to the list.
        //ids.add(separatorBarId);
        //ids.add(spacerId);
        //ids.add(flexibleSpacerId);
    }

    void getDefaultItemSet(juce::Array<int>& ids) override
    {
        // This returns an ordered list of the set of items that make up a
        // toolbar's default set. Not all items need to be on this list, and
        // items can appear multiple times (e.g. the separators used here).
        ids.add(doc_new        );
        ids.add(doc_open       );
        ids.add(doc_save       );
        ids.add(doc_saveAs     );
        
        ids.add(spacerId       );
 
        //ids.add(media_backward );
        ids.add(media_stop     );
        ids.add(media_start    );
        ids.add(media_record   );
        //ids.add(media_pause    );
        //ids.add(media_forward);
    }

    juce::ToolbarItemComponent* createItem(int itemId) override
    {
        switch (itemId)
        {
            case doc_new:           return createButtonFromZipFileSVG(itemId, "new", "document-new.svg");
            case doc_open:          return createButtonFromZipFileSVG(itemId, "open", "document-open.svg");
            case doc_save:          return createButtonFromZipFileSVG(itemId, "save", "document-save.svg");
            case doc_saveAs:        return createButtonFromZipFileSVG(itemId, "save as", "document-save-as.svg");

            case edit_copy:         return createButtonFromZipFileSVG(itemId, "copy", "edit-copy.svg");
            case edit_cut:          return createButtonFromZipFileSVG(itemId, "cut", "edit-cut.svg");
            case edit_paste:        return createButtonFromZipFileSVG(itemId, "paste", "edit-paste.svg");

            case media_start:       return createButtonFromZipFileSVG(itemId, "start", "media-playback-start.svg");
            case media_record:      return createButtonFromZipFileSVG(itemId, "record", "media-record.svg");
            case media_stop:        return createButtonFromZipFileSVG(itemId, "stop", "media-playback-stop.svg");
            case media_pause:       return createButtonFromZipFileSVG(itemId, "pause", "media-playback-pause.svg");
            case media_forward:     return createButtonFromZipFileSVG(itemId, "forward", "media-seek-forward.svg");
            case media_backward:    return createButtonFromZipFileSVG(itemId, "backward", "media-seek-backward.svg");

            default:                break;
        }

        return nullptr;
    }

private:
    juce::StringArray iconNames;
    juce::OwnedArray<juce::Drawable> iconsFromZipFile;

    // This is a little utility to create a button with one of the SVG images in
    // our embedded ZIP file "icons.zip"
    juce::ToolbarButton* createButtonFromZipFileSVG(const int itemId, const juce::String& text, const juce::String& filename)
    {
        if (iconsFromZipFile.size() == 0)
        {
            // If we've not already done so, load all the images from the zip file..
            juce::ZipFile icons(Helpers::createZipStreamFromEmbeddedResource(BinaryData::namedResourceList[0]).release(), true);

            for (int i = 0; i < icons.getNumEntries(); ++i)
            {
                std::unique_ptr<juce::InputStream> svgFileStream(icons.createStreamForEntry(i));

                if (svgFileStream.get() != nullptr)
                {
                    iconNames.add(icons.getEntry(i)->filename);
                    iconsFromZipFile.add(juce::Drawable::createFromImageDataStream(*svgFileStream));
                }
            }
        }

        auto* image = iconsFromZipFile[iconNames.indexOf(filename)];
        return new juce::ToolbarButton(itemId, text, image->createCopy(), {});
    }
};

