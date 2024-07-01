//
// Created by vastrakai on 6/28/2024.
//

#include "Module.hpp"

#include <Features/FeatureManager.hpp>
#include <nes/event_dispatcher.hpp>

void Module::setEnabled(bool enabled)
{
    bool prevEnabled = mEnabled;
    mEnabled = enabled;

    if (mEnabled && !prevEnabled)
    {
        auto holder = nes::make_holder<ModuleStateChangeEvent>(mEnabled, prevEnabled);
        gFeatureManager->mDispatcher->trigger(holder);
        onEnable();
    }
    if (!mEnabled && prevEnabled)
    {
        auto holder = nes::make_holder<ModuleStateChangeEvent>(mEnabled, prevEnabled);
        gFeatureManager->mDispatcher->trigger(holder);
        onDisable();
    }
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
