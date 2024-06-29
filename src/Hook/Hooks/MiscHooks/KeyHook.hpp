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

namespace { struct KeyHookRegister { KeyHookRegister() { static bool is_registered = false; if (!is_registered) { is_registered = true; auto hook = std::make_unique<KeyHook>(); hook->mLocalPlayerDependent = false; HookManager::mFutures.push_back(std::async(std::launch::async, [hook = hook.get()]() { while (!SigManager::mIsInitialized || !OffsetProvider::mIsInitialized) { if (Solstice::mRequestEject) return; std::this_thread::sleep_for(std::chrono::milliseconds(1)); } while (!ClientInstance::get()) { if (Solstice::mRequestEject) return; std::this_thread::sleep_for(std::chrono::milliseconds(1)); } if (Solstice::mRequestEject) return; hook->init(); })); HookManager::mHooks.push_back(std::move(hook)); } } } KeyHookRegisterInstance; };