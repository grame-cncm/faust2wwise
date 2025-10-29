#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <vector>
#include <utility>
#include <atomic>

typedef std::vector<std::pair<std::string, std::string>> param_meta_items;
struct Parameter;
typedef std::vector<Parameter> ParameterList;

struct Parameter{
    
    std::atomic<float> value{0.0f};  // runtime value of the parameter

    std::string type;
    std::string label;
    std::string shortname;
    float init;
    float pmin;
    float pmax;
    float step;
    // param_meta_items meta;

    Parameter()
        : type("")
        , label("")
        , shortname("")
        , init(0.0)
        , pmin(0.0)
        , pmax(1.0)
        , step(0.01)
        , value {0.0f}
    {}

    // delete copying
    Parameter(const Parameter &p) = delete;
    Parameter &operator=(const Parameter &p) = delete;

    Parameter(Parameter&& p) noexcept
        : type(p.type)
        , label(p.label)
        , shortname(p.shortname)
        , init(p.init)
        , pmin (p.pmin)
        , pmax (p.pmax)
        , step (p.step)
        , value{p.value.load()}
    {}
        
    Parameter& operator=(Parameter&& p) noexcept
    {
        type = std::move(p.type);
        label = std::move(p.label);
        shortname = std::move(p.shortname);
        init = p.init;
        pmin = p.pmin;
        pmax = p.pmax;
        step = p.step;
        value.store(p.value.load());
        return *this;
    }

};

#endif