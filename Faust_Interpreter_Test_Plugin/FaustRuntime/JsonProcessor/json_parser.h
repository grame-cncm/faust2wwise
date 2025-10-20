#include "nlohmann/json.hpp"
#include "../PluginInfo/plugin_config.h"
#include "../parameter.h"

bool process_json_configuration(PluginConfiguration& cfg, std::vector<Parameter> &parameters);

void extract_parameters(const nlohmann::json &json_data, std::vector<Parameter> &parameters);

void parse_param_metadata(param_meta_items& meta, const nlohmann::json& meta_items);