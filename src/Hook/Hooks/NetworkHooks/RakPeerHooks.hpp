#pragma once
//
// Created by vastrakai on 7/10/2024.
//

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>

class RakPeerHooks : public Hook {
public:
    RakPeerHooks() : Hook()
    {

    }

    static std::unique_ptr<Detour> RunUpdateCycleDetour;
    static std::unique_ptr<Detour> GetLastPingDetour;

    static float LastPing;

    static void runUpdateCycle(void* _this, void* a2);
    static __int64 getLastPing(void* _this, void* a2);
    static void init(uintptr_t addr);

    void init() override;
};

REGISTER_HOOK(RakPeerHooks);