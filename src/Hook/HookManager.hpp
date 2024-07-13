#pragma once
#include <future>
#include <memory>
#include <vector>
#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>


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
    static void shutdown();
    static void waitForHooks();
};

// Automatically adds a hook after waiting for SigManager::mIsInitialized and OffsetProvider::mIsInitialized to be true
#define REGISTER_HOOK(HookType) \
    namespace { \
        struct HookType##Register { \
            HookType##Register() { \
                static bool is_registered = false; \
                if (!is_registered) { \
                    is_registered = true; \
                    auto hook = std::make_unique<HookType>(); \
                    hook->mLocalPlayerDependent = false; \
                    HookManager::mFutures2.push_back(std::async(std::launch::async, [hook = hook.get()]() { \
                        while (!SigManager::mIsInitialized || !OffsetProvider::mIsInitialized) {      \
                            if (Solstice::mRequestEject) return; \
                            std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                        }           \
                        while (!ClientInstance::get()) {      \
                            if (Solstice::mRequestEject) return; \
                            std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                        }           \
                        while (!gFeatureManager || !gFeatureManager->mCommandManager) {      \
                            if (Solstice::mRequestEject) return; \
                            std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                        }           \
                        if (Solstice::mRequestEject) return; \
                        hook->init();   \
                    }));           \
                    HookManager::mHooks.push_back(std::move(hook));                                   \
                } else { \
                    spdlog::error("Hook {} already registered", #HookType); \
                } \
            }                   \
        } HookType##RegisterInstance;                     \
    }
// Automatically adds a hook and creates a std::future to call the init function when the local player is valid
// to get LP, you can to ClientInstance::get()->getLocalPlayer() (make sure you wait for ClientInstance to be valid first)

// Do an ifndef check on the Registrar to prevent multiple definitions
#define REGISTER_LP_HOOK(HookType) \
    class HookType; \
    static HookType global_##HookType##_instance; \
    struct HookType##_Registrar { \
        HookType##_Registrar() { \
            static bool is_registered = false; \
            if (!is_registered) { \
                is_registered = true; \
                global_##HookType##_instance.mLocalPlayerDependent = true; \
                HookManager::mFutures2.push_back(std::async(std::launch::async, []() { \
                    while (!SigManager::mIsInitialized || !OffsetProvider::mIsInitialized) { \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    } \
                    while (!ClientInstance::get()) { \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    } \
                    while (!ClientInstance::get()->getLocalPlayer()) { \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    } \
                    while (!gFeatureManager || !gFeatureManager->mCommandManager) {      \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    }           \
                    if (Solstice::mRequestEject) return; \
                    global_##HookType##_instance.init(); \
                })); \
                HookManager::mHooks.push_back(std::make_unique<HookType>(global_##HookType##_instance)); \
            } \
        } \
    }; \
    static HookType##_Registrar global_##HookType##_registrar;
