//
// Created by vastrakai on 6/24/2024.
//

#include "SigManager.hpp"

#include <iostream>
#include <Solstice.hpp>

hat::scan_result SigManager::scanSig(hat::signature_view sig, const std::string& name)
{
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
    for (const auto& sig : mSigs) {
        if (sig.second != 0) Solstice::console->info("found {} @ 0x{:X}", sig.first, sig.second);
    }

    for (const auto& sig : mSigs) {
        if (sig.second == 0) Solstice::console->critical("failed to find {}", sig.first);
    }


}


