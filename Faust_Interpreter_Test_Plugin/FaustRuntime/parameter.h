#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <vector>
#include <utility>

typedef std::vector<std::pair<std::string, std::string>> param_meta_items;

struct Parameter{
    std::string type;
    std::string label;
    std::string varname;
    std::string unq_shortname;
    std::string address;
    double init = 0.0;
    double min = 0.0;
    double max = 1.0;
    double step = 0.01;
    param_meta_items meta;
};


#endif