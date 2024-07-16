#pragma once
#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
//
// Created by vastrakai on 6/25/2024.
//


class KeyHook : public Hook {
public:
    KeyHook() : Hook() {

    }

    static std::unique_ptr<Detour> mDetour;

    static void onKey(uint32_t key, bool isDown);
    void init() override;
};

