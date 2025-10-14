#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cstdio>
#include <cstdlib>

/* IMPORTANT : These functions require running Wwise as administrator to run successfully. */

const char* getEnvVar(const std::string& varName);

std::string execCommand(const std::string& cmd);

#endif