//
// Created by vastrakai on 6/25/2024.
//

#include "KeyHook.hpp"

#include <Solstice.hpp>
#include <Utils/ActorUtils.hpp>
#include <Utils/ChatUtils.hpp>

std::unique_ptr<Detour> KeyHook::mDetour = nullptr;

void KeyHook::onKey(uint32_t key, bool isDown)
{
    auto oFunc = mDetour->getOriginal<decltype(&onKey)>();
    oFunc(key, isDown);

    if (key == VK_END)
    {
        if (ClientInstance::get()->getScreenName() != "chat_screen") Solstice::mRequestEject = true;
    }

    if (key == VK_HOME) {
        auto actors = ActorUtils::getActorList(false);
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        ChatUtils::displayClientMessage("actor count: " + std::to_string(actors.size()));
    }
}

void KeyHook::init()
{
    mName = "Keyboard::feed";
    mDetour = std::make_unique<Detour>("Keyboard::feed", reinterpret_cast<void*>(SigManager::Keyboard_feed), &KeyHook::onKey);
    mDetour->enable();
}
