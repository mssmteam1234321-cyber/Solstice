#pragma once
//
// Created by vastrakai on 8/11/2024.
//

#include <Hook/Hook.hpp>

class CanShowNameHook : public Hook {
public:
    CanShowNameHook() : Hook() {
        mName = "Unknown::canShowNameTag";
    }

    static std::unique_ptr<Detour> mDetour;

    static bool onCanShowNameTag(void* a1, class Actor* actor);
    void init() override;
};

