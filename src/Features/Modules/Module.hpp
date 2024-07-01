#pragma once
//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleCategory.hpp"
#include <string>
#include <utility>
#include <vector>
#include <glm/glm.hpp>

#include "Features/Events/ModuleStateChangeEvent.hpp"

class Module {
public:
    std::string mName;
    std::string mDescription;
    ModuleCategory mCategory;
    bool mEnabled;
    int mKey;
    std::vector<class Setting*> mSettings;

    Module(std::string name, std::string description, const ModuleCategory category, const int key = 0, const bool enabled = false)
        : mName(std::move(name)), mDescription(std::move(description)), mCategory(category), mEnabled(enabled), mKey(key) {}

    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void onTick() {}
    [[nodiscard]] virtual const char* getTypeID() const = 0;

    void setEnabled(bool enabled);
    void toggle();
    void enable();
    void disable();
    void addSetting(class Setting* setting);
    std::string& getCategory();


    float animPercentage = 0.f;
    glm::vec2 location = glm::vec2(0, 0);
    glm::vec2 size = glm::vec2(0, 0);
    glm::vec2 pos = glm::vec2(0, 0);
    bool showSettings = false;
    float cAnim = 0;
    float cFade = 0;
    float cScale = 0;
};

template <typename T>
class ModuleBase : public Module
{
public:
    ModuleBase(std::string name, std::string description, const ModuleCategory category, const int key = 0, const bool enabled = false)
        : Module(std::move(name), std::move(description), category, key, enabled) {}

    static const char* TypeID() {
        return typeid(T).name();
    }

    const char* getTypeID() const override {
        return TypeID();
    }
};

