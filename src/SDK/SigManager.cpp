//
// Created by vastrakai on 6/24/2024.
//

#include "SigManager.hpp"

#include <iostream>
#include <Solstice.hpp>
#include <Utils/Logger.hpp>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()

hat::scan_result SigManager::scanSig(hat::signature_view sig, const std::string& name)
{
    if(!Logger::initialized) Logger::initialize(); // Necessary because this can be called before the init thread is created :3

    if (mSigScanCount == 0)
    {
        // (milliseconds)
        mSigScanStart = NOW;
    }
    mSigScanCount++;

    auto result = hat::find_pattern(sig);
    if (!result.has_result()) {
        mSigs[name] = 0;
        return {};
    }

    mSigs[name] = reinterpret_cast<uintptr_t>(result.get());

    return result;
}

void SigManager::initialize()
{
    // Wait for all futures to complete
    for (auto& future : futures) {
        future.get();
    }
    futures.clear(); // Clear the futures vector once initialization is complete

    uint64_t end = NOW;

    for (const auto& sig : mSigs) {
        if (sig.second != 0) Solstice::console->info("found {} @ 0x{:X}", sig.first, sig.second);
    }

    for (const auto& sig : mSigs) {
        if (sig.second == 0) Solstice::console->critical("failed to find {}", sig.first);
    }
    Solstice::console->info("MainView_instance @ 0x{:X}", MainView_instance);

    Solstice::console->info("SigManager initialized in {}ms", end - mSigScanStart);
}


