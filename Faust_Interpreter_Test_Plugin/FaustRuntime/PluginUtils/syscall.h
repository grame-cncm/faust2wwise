#ifndef FPLUGIN_SYSCALL_H
#define FPLUGIN_SYSCALL_H

#include <string>

/* IMPORTANT : These functions require running Wwise as administrator to run successfully. */
namespace SysCall{
    
    std::string getEnvVar(const std::string& varName);

    std::string execCommand(const std::string& cmd);

}

#endif