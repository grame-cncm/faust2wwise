#include "plugin_utils.h"
#include <cctype>
#include <filesystem>
#include <iostream>

// for the createTempDir
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <sstream>

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


    bool moveFile(const std::string& sourcePath, const std::string& destPath)
    {
        try {
            std::filesystem::path src(sourcePath);
            std::filesystem::path dst(destPath);

            if (!std::filesystem::exists(src)) {
                std::cerr << "Source file does not exist: " << src << '\n';
                return false;
            }

            std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing);
            std::filesystem::remove(src); // Delete original
            return true;
        } 
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << '\n';
            return false;
        }
    }

    // TODO: discard timestamp and use the plugin name for the new directory.
    std::string createTempDir(const std::string& exportPath)
    {
        std::ostringstream timestamp;
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        timestamp << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");

        std::filesystem::path tempDirectory = std::filesystem::path( exportPath + "/faust_" + timestamp.str());

        try {
            std::filesystem::create_directories(tempDirectory);
            // std::filesystem::remove(tempDirectory);          //TODO : uncomment later
        } catch (const std::filesystem::filesystem_error& e) {
            // handle failure to create directory
            return false;
        }
        return tempDirectory.string();
    }    
}