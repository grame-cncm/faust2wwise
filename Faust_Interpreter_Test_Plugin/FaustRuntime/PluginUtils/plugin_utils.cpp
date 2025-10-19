#include "plugin_utils.h"
#include <cctype>

namespace PluginUtils
{

    std::string ensure_valid_plugin_name(const std::string& name) {
        std::string valid = name;

        // replace spaces with underscores
        for (auto& c : valid) {
            if (c == ' ') {
                c = '_';
            }
        }

        // ff empty or first char not alphabetic or '_' --> prepend "Dsp_"
        if (valid.empty() || !(std::isalpha(valid[0]) || valid[0] == '_')) {
            valid = "Dsp_" + valid;
        }

        // capitalize first char
        if (!valid.empty()) {
            valid[0] = static_cast<char>(std::toupper(valid[0]));
        }

        return valid;
    }


}