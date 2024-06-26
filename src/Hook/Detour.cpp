//
// Created by vastrakai on 6/24/2024.
//

#include "Detour.hpp"

#include <magic_enum.hpp>
#include <MinHook.h>
#include <string>

#include "spdlog/spdlog.h"

Detour::~Detour()
{
    if (mFunc)
    {
        MH_DisableHook(mFunc);
        MH_RemoveHook(mFunc);
    }
}

Detour::Detour(const std::string& name, void* addr, void* detour)
    : mName(name), mFunc(addr)
{
    this->mFunc = addr;
    this->mName = name;

    const MH_STATUS result = MH_CreateHook(mFunc, detour, &mOriginalFunc);

    spdlog::info("Created detour for {} at: {}, Status: {}", name, addr, magic_enum::enum_name(result));
}

void Detour::Enable() const
{
    const MH_STATUS status = MH_EnableHook(mFunc);
    switch (status)
    {
    case MH_OK:
        spdlog::info("Enabled hook for {}", mName);
        break;
    default:
        spdlog::critical("Failed to enable hook for {}", mName);
        break;
    }
}

void Detour::Restore() const
{
    if (!mFunc)
    {
        spdlog::critical("Failed to restore hook for {} [mFunc == nullptr]", mName);
        return;
    }
    MH_STATUS status = MH_DisableHook(mFunc);
    switch (status)
    {
    case MH_OK:
        spdlog::info("Restored hook for {}", mName);
        break;
    default:
        spdlog::critical("Failed to restore hook for {}", mName);
        break;
    }
    status = MH_RemoveHook(mFunc);
    switch (status)
    {
    case MH_OK:
        spdlog::info("Removed hook for {}", mName);
        break;
    default:
        spdlog::critical("Failed to remove hook for {}", mName);
        break;
    }
}

