#include "PluginTreeItem.h"
#include "PluginDefinitions.h"

//==============================================================================

PluginTreeItem::PluginTreeItem (const juce::PluginDescription& d)
    : desc (d), xmlType (tracktion_engine::ExternalPlugin::xmlTypeName), isPlugin (true)
{
    jassert (xmlType.isNotEmpty());
}

PluginTreeItem::PluginTreeItem (const juce::String& uniqueId, const juce::String& name,
                                const juce::String& xmlType_, bool isSynth, bool isPlugin_)
    : xmlType (xmlType_), isPlugin (isPlugin_)
{
    jassert (xmlType.isNotEmpty());
    desc.name = name;
    desc.fileOrIdentifier = uniqueId;
    desc.pluginFormatName = (uniqueId.endsWith ("_trkbuiltin") || xmlType == tracktion_engine::RackInstance::xmlTypeName)
                                ? internal_plugin_format_name : "";
    desc.category = xmlType;
    desc.isInstrument = isSynth;
}

tracktion_engine::Plugin::Ptr PluginTreeItem::create (tracktion_engine::Edit& ed)
{
    return ed.getPluginCache().createNewPlugin (xmlType, desc);
}
