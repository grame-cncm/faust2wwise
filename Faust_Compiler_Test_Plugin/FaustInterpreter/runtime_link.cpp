#include "runtime_link.h"

#include <windows.h>
#include <iostream>

namespace RuntimeLink {

    void* loadDynamicLibrary(const std::string& dlibpath) 
    {
        HMODULE handle = LoadLibraryA(dlibpath.c_str());
        if (!handle) {
            std::cerr << "Failed to load library: " << dlibpath << "\n";
        }
        return reinterpret_cast<void*>(handle);
    }

    void unloadDynamicLibrary(void* handle) 
    {
        if (!handle) return;

        #if defined(_WIN32)
            FreeLibrary(reinterpret_cast<HMODULE>(handle));
        #elif defined(__APPLE__) || defined(__linux__)
            dlclose(handle);
        #endif
    }

    void* getSymbol(void* handle, const char* symbolName) 
    {
        if (!handle || !symbolName) {
            return nullptr;
        }

        #if defined(_WIN32)
            return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), symbolName));
        #elif defined(__APPLE__) || defined(__linux__)
            return dlsym(handle, symbolName);
        #else
            return nullptr;
        #endif
    }
}
