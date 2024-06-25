//
// Created by vastrakai on 6/25/2024.
//

#include "OffsetProvider.hpp"

#include <Solstice.hpp>
#include <Utils/MemUtils.hpp>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()

hat::scan_result OffsetProvider::scanSig(hat::signature_view sig, const std::string& name, int offset)
{
    if (mSigScanCount == 0)
    {
        mSigScanStart = NOW;
    }

    mSigScanCount++;
    hat::process::module_t minecraft = hat::process::get_module("Minecraft.Windows.exe");
    auto result = hat::find_pattern(sig, ".text", minecraft);

    if (!result.has_result()) {
        mSigs[name] = 0;
        return {};
    }

    mSigs[name] = reinterpret_cast<uintptr_t>(result.get()) + offset;
    return result;
}

void OffsetProvider::initialize()
{
    for (auto& future : futures)
    {
        future.get();
    }

    futures.clear();

    uint64_t end = NOW;

    for (const auto& sig : mSigs)
    {
        if (sig.second != 0)
        {
            Solstice::console->info("[offsets] found {} @ {}", sig.first, MemUtils::getMbMemoryString(sig.second));
        }
    }

    for (const auto& sig : mSigs)
    {
        if (sig.second == 0)
        {
            Solstice::console->critical("[offsets] failed to find {}", sig.first);
        }
    }

    Solstice::console->info("[offsets] initialized in {}ms, {} total sigs scanned", end - mSigScanStart, mSigScanCount);


}

