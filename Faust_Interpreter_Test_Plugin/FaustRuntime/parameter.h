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
    std::string varname;
    std::string shortname;
    std::string address;
    float init;
    float pmin;
    float pmax;
    float step;
    param_meta_items meta;

    Parameter()
        : type("")
        , label("")
        , varname("")
        , shortname("")
        , address("")
        , init(0.0)
        , pmin(0.0)
        , pmax(1.0)
        , step(0.01)
        , meta()
        , value {0.0f}
    {}

    // delete copying
    Parameter(const Parameter &p) = delete;
    Parameter &operator=(const Parameter &p) = delete;

    Parameter(Parameter&& p) noexcept
        : type(p.type)
        , label(p.label)
        , varname(p.varname)
        , shortname(p.shortname)
        , address(p.address)
        , init(p.init)
        , pmin (p.pmin)
        , pmax (p.pmax)
        , step (p.step)
        , meta (p.meta)
        , value{p.value.load()}
    {}
        
    Parameter& operator=(Parameter&& p) noexcept
    {
        type = std::move(p.type);
        label = std::move(p.label);
        varname = std::move(p.varname);
        shortname = std::move(p.shortname);
        address = std::move(p.address);
        init = p.init;
        pmin = p.pmin;
        pmax = p.pmax;
        step = p.step;
        meta = std::move(p.meta);
        value.store(p.value.load());
        return *this;
    }

};

#endif