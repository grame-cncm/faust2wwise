#ifndef PLUGIN_UTILS_H
#define PLUGIN_UTILS_H

#include <string>

namespace PluginUtils{

    /*
        Converts the plugin name to a valid format accepted by Wwise.
        Adjustments include:
            - Prefixing with "Dsp_" if name starts with a number or is invalid
            - Replacing spaces with underscores
            - Capitalizing the first character
    */
    std::string ensure_valid_plugin_name(const std::string& name);
}

#endif
