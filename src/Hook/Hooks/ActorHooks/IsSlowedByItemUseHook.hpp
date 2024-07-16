#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>

class IsSlowedByItemUseHook : public Hook {
public:
    IsSlowedByItemUseHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void* onIsSlowedByItemUse(void* a1, void* a2, void* a3);
    void init() override;
};

