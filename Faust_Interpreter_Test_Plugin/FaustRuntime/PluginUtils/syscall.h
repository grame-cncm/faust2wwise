#ifndef FPLUGIN_SYSCALL_H
#define FPLUGIN_SYSCALL_H

#include <string>

namespace SysCall{
    
    std::string getEnvVar(const std::string& varName);

    std::string execCommand(const std::string& cmd);

}

#endif