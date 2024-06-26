#pragma once
#include <Windows.h>

#include "spdlog/logger.h"
//
// Created by vastrakai on 6/24/2024.
//


class Solstice {
public:
    /* Fields */
    static inline HMODULE mModule;
    static inline bool mInitialized = false;
    static inline bool mRequestEject = false;
    static inline std::shared_ptr<spdlog::logger> console;

    /* Functions */
    static void init(HMODULE hModule);
    static void shutdownThread();
};