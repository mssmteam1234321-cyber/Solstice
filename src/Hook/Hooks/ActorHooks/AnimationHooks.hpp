#pragma once
//
// Created by vastrakai on 7/18/2024.
//

#include <Hook/Hook.hpp>

class AnimationHooks : public Hook {
public:
    AnimationHooks() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static int getCurrentSwingDuration(class Actor* actor);
    void init() override;
};