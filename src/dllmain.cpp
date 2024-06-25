#pragma once

#include <iostream>
#include <Solstice.hpp>
#include <thread>
#include <Windows.h>
#include <Utils/Logger.hpp>

bool __stdcall DllMain(const HMODULE hModule, const DWORD fdwReason, [[maybe_unused]] LPVOID lpReserved) {
    if(fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Solstice::init), hModule, 0, nullptr);
    }
    return true;  // Successful DLL_PROCESS_ATTACH
}
