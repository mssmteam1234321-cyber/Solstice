#pragma once
//
// Created by vastrakai on 6/25/2024.
//

#include <Hook/Hook.hpp>



class BaseTickHook : public Hook {
public:
    BaseTickHook() : Hook() {
        mName = "Actor::baseTick";
    }

    static std::unique_ptr<Detour> mDetour;

    static void onBaseTick(class Actor* actor);
    void init() override;
};

