//
// Created by vastrakai on 6/28/2024.
//

#include "Module.hpp"

#include <Features/FeatureManager.hpp>
#include <nes/event_dispatcher.hpp>

void Module::setEnabled(bool enabled)
{
    bool prevEnabled = mEnabled;
    bool newEnabled = enabled;

    if (newEnabled && !prevEnabled)
    {
        auto holder = nes::make_holder<ModuleStateChangeEvent>(this, newEnabled, prevEnabled);
        gFeatureManager->mDispatcher->trigger(holder);
        if (holder->isCancelled()) return;
    }
    if (!newEnabled && prevEnabled)
    {
        auto holder = nes::make_holder<ModuleStateChangeEvent>(this, newEnabled, prevEnabled);
        gFeatureManager->mDispatcher->trigger(holder);
        if (holder->isCancelled()) return;
    }

    mWantedState = newEnabled;
}

void Module::toggle()
{
    setEnabled(!mEnabled);
}

void Module::enable()
{
    if (!mEnabled) setEnabled(true);
}

void Module::disable()
{
    if (mEnabled) setEnabled(false);
}

void Module::addSetting(Setting* setting)
{
    this->mSettings.emplace_back(setting);
}

std::string& Module::getCategory()
{
    return ModuleCategoryNames[static_cast<int>(mCategory)];
}
