#pragma once
#include <Solstice.hpp>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
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

REGISTER_HOOK(KeyHook);