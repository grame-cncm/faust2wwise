
#include "json_parser.h"
#include "../PluginUtils/plugin_utils.h"

#include<iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>

bool process_json_configuration(PluginConfiguration& cfg) {
    
    // Parsing json configuration file..
    try {

        if (!std::filesystem::exists(cfg.path.json_file)) {
            std::cerr << "Error: JSON file not found: " << cfg.path.json_file << std::endl;
            return false;
        }

        std::ifstream inFile(cfg.path.json_file);
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string content = buffer.str();
        inFile.close();

        // Check for Windows-like paths and fix backslashes
        if (std::regex_search(content, std::regex("[A-Z]:\\\\+"))) {
            std::string fixed_content;
            for (size_t i = 0; i < content.size(); ++i) {
                if (content[i] == '\\') {
                    fixed_content += "\\\\";
                } else {
                    fixed_content += content[i];
                }
            }
            std::ofstream outFile(cfg.path.json_file);
            outFile << fixed_content;
            outFile.close();
            content = fixed_content;
        }

        // Parse JSON
        nlohmann::json json_data = nlohmann::json::parse(content);

        // Extract inputs/outputs
        cfg.num_inputs = json_data.value("inputs", 0);
        cfg.num_outputs = json_data.value("outputs", 0);

        // Determine plugin type based on inputs
        if (cfg.num_inputs > 0) {
            cfg.plugin_type = "effect";
        } else {
            cfg.plugin_type = "source";
        }

        // Extract project name & override in case it is provided as a declaration in faust dsp file
        // i.e. declare name "Name"
        if (json_data.contains("name")) {
            cfg.plugin_name = PluginUtils::ensure_valid_plugin_name(json_data["name"]);
            std::cout << "PLUGIN_NAME changed to " << cfg.plugin_name << std::endl;
        }

        // extract author from meta dict
        cfg.author = "Unknown";
        cfg.description = "No description provided";
        if (json_data.contains("meta") && json_data["meta"].is_array()) {
            for (const auto& item : json_data["meta"]) {
                if (item.is_object()) {
                    if (item.contains("author")) {
                        cfg.author = item["author"];
                    }
                    if (item.contains("description")) {
                        cfg.description = item["description"];
                    }
                }
            }
        }

        std::cout << "OK: " << cfg.path.json_file << " was parsed successfully!" << std::endl;

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Error: Failed to parse JSON: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to process JSON configuration: " << e.what() << std::endl;
        return false;
    }
    return true;
}