#pragma once
//
// Created by vastrakai on 6/25/2024.
//

#include <Solstice.hpp>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>


class BaseTickHook : public Hook {
public:
    BaseTickHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void onBaseTick(Actor* actor);
    void init() override;
};

REGISTER_LP_HOOK(BaseTickHook);