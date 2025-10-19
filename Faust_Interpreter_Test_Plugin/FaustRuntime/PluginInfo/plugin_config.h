#ifndef PLUGIN_CONFIG_H
#define PLUGIN_CONFIG_H

#include <string>

struct PluginConfiguration {
    
    std::string name_app;
    
    struct Paths {
        std::string wwiseRoot;
        std::string faust_includedir;
        std::string faust_dspdir;
        std::string exportPath;
        std::string tempDir;
        std::string cppfile;
        std::string archfile;  
        std::string dllPath; 
        std::string json_file;
        std::string config = "Debug"; 
    }path;
    
    std::string plugin_type;
    std::string plugin_name;
    std::string author;
    std::string description;
    int num_inputs = 0;
    int num_outputs = 0;

    int sampleRate=0;
    int bufferSize=0;
    int speakersAvail=0;
    int maxSpeakersAvail=0;

    void reset()
    {
        name_app=""; 
        path.tempDir="";
        path.cppfile="";
        path.dllPath=""; 
        path.json_file="";
        plugin_type="";
        plugin_name="";
        author="";
        description="";
        num_inputs = 0;
        num_outputs = 0;
        sampleRate=0;
        speakersAvail = (maxSpeakersAvail != 0) ? maxSpeakersAvail : 0;
    }
};

#endif