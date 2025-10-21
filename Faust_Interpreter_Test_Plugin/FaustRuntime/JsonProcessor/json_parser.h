#include "nlohmann/json.hpp"
#include "PluginInfo/plugin_config.h"
#include "parameter.h"

bool process_json_configuration(PluginConfiguration& cfg, ParameterList &parameters);

void extract_parameters(const nlohmann::json &json_data, ParameterList &parameters);

void parse_param_metadata(param_meta_items& meta, const nlohmann::json& meta_items);