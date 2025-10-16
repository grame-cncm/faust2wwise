#include "syscall.h"

namespace SysCall{
    /* IMPORTANT : These functions require running Wwise as administrator to run successfully. */

    const char* getEnvVar(const std::string& varName) {
        const char* value = std::getenv(varName.c_str());
        return value ? value : "";  // return empty string if not found
    }

    std::string execCommand(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string result;

        // Open pipe using _popen
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        if (!pipe) {
            throw std::runtime_error("_popen() failed!");
        }

        // Read output
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }

        return result.substr(0, result.size()-1); // erase \n at the end.
    }

}
