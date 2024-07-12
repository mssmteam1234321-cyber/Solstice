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
    std::vector<std::shared_ptr<Module>> mModules;
    static inline std::vector<std::future<void>> mModuleFutures;

    void init();
    void shutdown();
    void registerModule(const std::shared_ptr<Module>& module);
    std::vector<std::shared_ptr<Module>>& getModules();
    Module* getModule(const std::string& name) const;
    template <typename T>
    T* getModule()
    {
        // TypeID based search
        const auto& typeID = typeid(T).name();
        for (const auto& module : mModules)
        {
            if (module->getTypeID() == typeID)
            {
                return static_cast<T*>(module.get());
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<Module>>& getModulesInCategory(int catId);
    std::unordered_map<std::string, std::shared_ptr<Module>> getModuleCategoryMap();
    void onClientTick();
    nlohmann::json serialize() const;
    nlohmann::json serializeModule(Module* module);
    void deserialize(const nlohmann::json& j, bool showMessages = true);
};



// Macro for module registration
#define REGISTER_MODULE(MODULE_CLASS)
/*#define REGISTER_MODULE(MODULE_CLASS) \
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
                    gFeatureManager->mModuleManager->registerModule(std::make_shared<MODULE_CLASS>()); \
                })); \
            } \
        }; \
        static MODULE_CLASS ## _Registrator MODULE_CLASS ## _registrator; \
    }*/

#ifdef __DEBUG__
    #define REGISTER_DEBUG_MODULE(MODULE_CLASS) REGISTER_MODULE(MODULE_CLASS)
#else
    #define REGISTER_DEBUG_MODULE(MODULE_CLASS)
#endif