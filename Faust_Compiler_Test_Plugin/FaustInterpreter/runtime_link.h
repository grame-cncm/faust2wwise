#ifndef RUNTIME_LINK_H
#define RUNTIME_LINK_H

#include <string>

namespace RuntimeLink {

    void* loadDynamicLibrary(const std::string&);

    void unloadDynamicLibrary(void* handle);

    void* getSymbol(void* handle, const char* symbolName);

}

#endif