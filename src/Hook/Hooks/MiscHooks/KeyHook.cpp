//
// Created by vastrakai on 6/25/2024.
//

#include "KeyHook.hpp"

#include <Solstice.hpp>
#include <Features/Modules/Visual/ClickGui.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>

std::unique_ptr<Detour> KeyHook::mDetour = nullptr;

void KeyHook::onKey(uint32_t key, bool isDown)
{
    auto oFunc = mDetour->getOriginal<decltype(&onKey)>();
    oFunc(key, isDown);

    if (key == VK_END && isDown)
    {
        if (ClientInstance::get()->getScreenName() != "chat_screen") Solstice::mRequestEject = true;
    }

    if (isDown)
    {
        // Look for modules
        const auto* clickGui = gFeatureManager->mModuleManager->getModule<ClickGui>();
        for (auto& module : gFeatureManager->mModuleManager->getModules())
        {
            if (!ClientInstance::get()->getMouseGrabbed() && module.get() != clickGui) continue;

            if (module->mKey == key)
            {
                module->toggle();
                return;
            };
        }
    }
}

void KeyHook::init()
{
    mName = "Keyboard::feed";
    mDetour = std::make_unique<Detour>("Keyboard::feed", reinterpret_cast<void*>(SigManager::Keyboard_feed), &KeyHook::onKey);
    mDetour->enable();
}
