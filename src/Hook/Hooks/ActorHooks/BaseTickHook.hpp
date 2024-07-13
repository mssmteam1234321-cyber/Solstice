#pragma once
//
// Created by vastrakai on 6/25/2024.
//

#include <Solstice.hpp>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>



class BaseTickHook : public Hook {
public:
    BaseTickHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void onBaseTick(class Actor* actor);
    void init() override;
};

REGISTER_LP_HOOK(BaseTickHook);