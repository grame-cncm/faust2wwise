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

    bool moveFile(const std::string&, const std::string&);

    std::string createTempDir(const std::string&);

    std::string wstring2string(const std::wstring);
    std::wstring string2wstring(const std::string);

    std::string parsePluginConfiguration(const std::string&, const int numLines = 7);

    bool store_utf16_file(const std::string& path, const std::wstring& buffer);
    bool load_utf16_file(const std::string& path, std::wstring& buffer);
}

#endif
