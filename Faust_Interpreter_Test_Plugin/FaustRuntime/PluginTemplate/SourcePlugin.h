#include "abstract.h"

class WwiseSourcePlugin: public AbstractPlugin
{
public:
    WwiseSourcePlugin(PluginConfiguration&, ParameterList&, InterpreterWrapper&);
    ~WwiseSourcePlugin() = default;
    bool setup() override final;
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32) override final;
    void fillRestOfBuffersWithSilence(const AkUInt32) override final;
};