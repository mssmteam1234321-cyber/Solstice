#pragma once
#include <future>
#include <memory>
#include <vector>

#include "Module.hpp"
//
// Created by vastrakai on 6/28/2024.
//



class ModuleManager {
public:
    std::vector<std::unique_ptr<Module>> mModules;
    static inline std::vector<std::future<void>> mModuleFutures;

    void init();
    void shutdown();
    void registerModule(std::unique_ptr<Module> module);
    std::vector<std::unique_ptr<Module>>& getModules();
};

// Macro for module registration
#define REGISTER_MODULE(MODULE_CLASS) \
    namespace { \
        struct MODULE_CLASS ## _Registrator { \
            MODULE_CLASS ## _Registrator() { \
                static bool registered = false; \
                if (registered) return; \
                registered = true; \
                ModuleManager::mModuleFutures.push_back(std::async(std::launch::async, []() { \
                    while (!gFeatureManager || !gFeatureManager->mModuleManager) { \
                        if (Solstice::mRequestEject) return; \
                        std::this_thread::sleep_for(std::chrono::milliseconds(1)); \
                    } \
                    if (Solstice::mRequestEject) return; \
                    gFeatureManager->mModuleManager->registerModule(std::make_unique<MODULE_CLASS>()); \
                    spdlog::info("Registered module: {}", #MODULE_CLASS); \
                })); \
            } \
        }; \
        static MODULE_CLASS ## _Registrator MODULE_CLASS ## _registrator; \
    }

#ifdef __DEBUG__
    #define REGISTER_DEBUG_MODULE(MODULE_CLASS) REGISTER_MODULE(MODULE_CLASS)
#else
    #define REGISTER_DEBUG_MODULE(MODULE_CLASS)
#endif