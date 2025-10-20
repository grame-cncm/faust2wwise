
#include "json_parser.h"
#include "../PluginUtils/plugin_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>
#include <unordered_set>

bool process_json_configuration(PluginConfiguration& cfg, std::vector<Parameter> &parameters) {
    
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

        // extract ui parameters
        extract_parameters(json_data["ui"], parameters);

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

void extract_parameters(const nlohmann::json &parameter_list, std::vector<Parameter> &parameters)
{
    // for filtering out entities such as vgroup or hgroup
    static const std::unordered_set<std::string> param_types = {
        "hslider", "vslider", "nentry", "checkbox", "button", "hbargraph", "vbargraph"};

    for (const auto& item : parameter_list) {

        // recurse for nested parameters...
        if (item.contains("items") && item["items"].is_array()) {
            const nlohmann::json& child_items = item["items"];
            extract_parameters(child_items, parameters);
        }

        if (item.contains("type") && param_types.count(item["type"])) {
            Parameter param;
            param.type = item.value("type", "");
            param.label = item.value("label", "");
            param.varname = item.value("varname", "");
            param.address = item.value("address", "");
            param.init = item.value("init", 0.0);
            param.min = item.value("min", 0.0);
            param.max = item.value("max", 1.0);
            param.step = item.value("step", 0.01);
            if (item.contains("meta"))
                parse_param_metadata(param.meta, item["meta"]);
            parameters.push_back(std::move(param));
        }
    }
}

void parse_param_metadata(param_meta_items& meta, const nlohmann::json& meta_items)
{
    for (const auto& meta_item : meta_items) {
        if (meta_item.is_object()) {
            for (auto it = meta_item.begin(); it != meta_item.end(); ++it) {
                meta.emplace_back(
                    it.key(),
                    it.value().is_string() ? it.value().get<std::string>() : it.value().dump()
                );
            }
        }
    }
}
