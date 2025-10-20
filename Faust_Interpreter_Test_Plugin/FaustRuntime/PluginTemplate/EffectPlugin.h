#include "abstract.h"

class WwiseEffectPlugin: public AbstractPlugin
{
public:
    WwiseEffectPlugin(std::vector<Parameter>&);
    ~WwiseEffectPlugin() override final;
    void setup(PluginConfiguration&) override final;
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32) override final;
    void fillRestOfBuffersWithSilence(const AkUInt32) override final;
};