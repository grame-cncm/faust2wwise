#ifndef DLLSTATE_H
#define DLLSTATE_H

enum DLLState{
    UNSET,
    // IDDLE, // for when loading previously compiled plugins? i.e. when opening wwise?
    PENDING_COMPILE,
    FAIL_COMPILE_DSP,
    FAIL_EXPORT_CPP,
    FAIL_COMPILE_CPP,
    DLL_COMPILED,
    LINKED
};

#endif