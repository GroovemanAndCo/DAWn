/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   dawn_icon_lrg_png;
    const int            dawn_icon_lrg_pngSize = 440716;

    extern const char*   dawn_icon_sml_png;
    const int            dawn_icon_sml_pngSize = 17404;

    extern const char*   logotracktionenginepowered_3x_png;
    const int            logotracktionenginepowered_3x_pngSize = 25797;

    extern const char*   res_icons_zip;
    const int            res_icons_zipSize = 86071;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 4;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
