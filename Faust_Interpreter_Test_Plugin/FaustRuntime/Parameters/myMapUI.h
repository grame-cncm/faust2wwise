#ifndef MY_MAPUI_H
#define MY_MAPUI_H

#include <faust/gui/MapUI.h>
#include "parameter.h"
#include <unordered_map>

static int ctr{0};

struct myMapUI : public MapUI
{    
    struct itemInfo {
        std::string type;
        std::string label;
        std::string shortname;
        int index;
        double init;
        double fmin;
        double fmax;
        double step;
        
        itemInfo(std::string t, std::string l, int idx, double i, double mn, double mx, double s)
        :type(t), label(l), index(idx), init(i), fmin(mn), fmax(mx), step(s)
        {}
    };

    std::unordered_map<const FAUSTFLOAT*,itemInfo> controls;

    void fillShortNames()
    {
        for (const auto& it : fShortnameZoneMap) {
            const auto& shortName = it.first;
            const FAUSTFLOAT* zone = it.second;

            auto iter = controls.find(zone);  // only access existing item
            if (iter != controls.end()) {
                iter->second.shortname = shortName; // assign shortname
            }
        }
    }

    void addParameter(FAUSTFLOAT* zone, std::string type, const char* label, int index, double init = -DBL_MAX, double fmin = -DBL_MAX, double fmax = -DBL_MAX, double step = -DBL_MAX) {
        
        itemInfo item(type, std::string(label), index, init, fmin, fmax, step);
        controls.emplace(zone, std::move(item));
    }

    void clearParameters()
    {
        controls.clear();
        fLabelZoneMap.clear();
        fShortnameZoneMap.clear();
        fPathZoneMap.clear();
        ctr = 0;
    }

    // -- active widgets
    void addButton(const char* label, FAUSTFLOAT* zone)
    {
        MapUI::addZoneLabel(label, zone);
        addParameter(zone, "button", label, ctr++);
    }
    void addCheckButton(const char* label, FAUSTFLOAT* zone)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "checkbox", label, ctr++);
    }
    void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "slider", label, ctr++, init, fmin, fmax, step);
    }
    void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "slider", label, ctr++, init, fmin, fmax, step);
    }
    void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "slider", label, ctr++, init, fmin, fmax, step);
    }
    
    // -- passive widgets
    void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "bargraph", label, ctr++, -DBL_MAX, fmin, fmax, -DBL_MAX);
    }
    void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
    {
        addZoneLabel(label, zone);
        addParameter(zone, "bargraph", label, ctr++, -DBL_MAX, fmin, fmax, -DBL_MAX);
    }

    // -- soundfiles
    // virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
    
    // -- metadata declarations
    // virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val){}
};

#endif