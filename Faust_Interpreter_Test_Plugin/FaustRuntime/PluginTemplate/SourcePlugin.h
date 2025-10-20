#include "abstract.h"

class WwiseSourcePlugin: public AbstractPlugin
{
public:
    WwiseSourcePlugin(std::vector<Parameter>&);
    ~WwiseSourcePlugin() = default;
    void setup(PluginConfiguration&) override final;
    void callback(std::vector<FAUSTFLOAT*>&, const AkUInt32) override final;
    void fillRestOfBuffersWithSilence(const AkUInt32) override final;
};