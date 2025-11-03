#include "abstract.h"

class WwiseEffectPlugin: public AbstractPlugin
{
public:
    WwiseEffectPlugin(PluginConfiguration&, ParameterList&, InterpreterWrapper&);
    ~WwiseEffectPlugin() override final;
    bool setup() override final;
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32) override final;
};