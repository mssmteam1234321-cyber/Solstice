#pragma once
#include <future>
#include <memory>
#include <vector>

#include "Hook.hpp"
//
// Created by vastrakai on 6/25/2024.
//

class HookManager {
public:
    static inline std::vector<std::future<void>> mFutures;
    static inline std::vector<std::future<void>> mFutures2;
    static inline std::vector<std::unique_ptr<Hook>> mHooks;


    static void init();


};

// Automatically adds a hook after waiting for SigManager::mIsInitialized and OffsetProvider::mIsInitialized to be true
#define REGISTER_HOOK(HookType) \
    namespace { \
        struct HookType##Register { \
            HookType##Register() { \
                auto hook = std::make_unique<HookType>(); \
                hook->mLocalPlayerDependent = false; \
                std::future<void> future = std::async(std::launch::async, [hook = hook.get()]() { \
                    while (!SigManager::mIsInitialized || !OffsetProvider::mIsInitialized) {      \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    }           \
                    while (!ClientInstance::get()) {      \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    }           \
                    if (Solstice::mRequestEject) return; \
                    hook->init();   \
                });             \
                HookManager::mFutures.push_back(std::move(future));                               \
                HookManager::mHooks.push_back(std::move(hook));                                   \
            }                   \
        } HookType##RegisterInstance;                     \
    }
// Automatically adds a hook and creates a std::future to call the init function when the local player is valid
// to get LP, you can to ClientInstance::get()->getLocalPlayer() (make sure you wait for ClientInstance to be valid first)
#define REGISTER_LP_HOOK(HookType) \
    namespace { \
        struct HookType##Register { \
            HookType##Register() { \
                auto hook = std::make_unique<HookType>(); \
                hook->mLocalPlayerDependent = true; \
                std::future<void> future = std::async(std::launch::async, [hook = hook.get()]() { \
                    while (!SigManager::mIsInitialized || !OffsetProvider::mIsInitialized) {      \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    }              \
                    while (!ClientInstance::get() || !ClientInstance::get()->getLocalPlayer()) {      \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    }           \
                    if (Solstice::mRequestEject) return; \
                    hook->init();   \
                });             \
                HookManager::mFutures2.push_back(std::move(future));                               \
                HookManager::mHooks.push_back(std::move(hook));                                   \
            }                   \
        } HookType##RegisterInstance;                     \
    }

