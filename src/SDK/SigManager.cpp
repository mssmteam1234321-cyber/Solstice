//
// Created by vastrakai on 6/24/2024.
//

#include "SigManager.hpp"

#include <iostream>
#include <Solstice.hpp>
#include <Utils/Logger.hpp>
#include <Utils/MemUtils.hpp>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()

hat::scan_result SigManager::scanSig(hat::signature_view sig, const std::string& name, int offset)
{
    if (mSigScanCount == 0)
    {
        // (milliseconds)
        mSigScanStart = NOW;
    }
    mSigScanCount++;

    hat::process::module_t minecraft = hat::process::get_module("Minecraft.Windows.exe");
    auto result = hat::find_pattern(sig, ".text", minecraft);

    if (!result.has_result()) {
        mSigs[name] = 0;
        return {};
    }

    if (offset == 0) mSigs[name] = reinterpret_cast<uintptr_t>(result.get());
    else mSigs[name] = reinterpret_cast<uintptr_t>(result.rel(offset));

    return result;
}

void SigManager::initialize()
{
    // Wait for all futures to complete
    for (auto& future : futures) {
        future.get();
    }
    futures.clear(); // Clear the futures vector once initialization is complete

    for (const auto& sig : mSigs) {
        //if (sig.second != 0) Solstice::console->info("found {} @ 0x{:X}", sig.first, sig.second);
        if (sig.second != 0) Solstice::console->info("[signatures] found {} @ {}", sig.first, MemUtils::getMbMemoryString(sig.second));
    }

    for (const auto& sig : mSigs) {
        if (sig.second == 0) Solstice::console->critical("[signatures] failed to find {}", sig.first);

    }
    Solstice::console->info("[signatures] initialized in {}ms, {} total sigs scanned", NOW - mSigScanStart, mSigScanCount);
}


