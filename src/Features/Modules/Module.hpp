#pragma once
//
// Created by vastrakai on 6/28/2024.
//

#include "ModuleCategory.hpp"
#include <string>
#include <utility>

class Module {
public:
    std::string mName;
    std::string mDescription;
    ModuleCategory mCategory;
    bool mEnabled;
    int mKey;

    Module(std::string name, std::string description, const ModuleCategory category, const int key = 0, const bool enabled = false)
        : mName(std::move(name)), mDescription(std::move(description)), mCategory(category), mEnabled(enabled), mKey(key) {}

    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void onTick() {}
    [[nodiscard]] virtual const char* getTypeID() const = 0;

    void setEnabled(bool enabled) {
        mEnabled = enabled;
        if (enabled) {
            onEnable();
        } else
        {
            onDisable();
        }
    };

    void toggle()
    {
        setEnabled(!mEnabled);
    }

    void enable()
    {
        if (!mEnabled) setEnabled(true);
    }

    void disable()
    {
        if (mEnabled) setEnabled(false);
    }



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

