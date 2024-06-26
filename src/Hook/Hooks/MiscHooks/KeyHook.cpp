//
// Created by vastrakai on 6/25/2024.
//

#include "KeyHook.hpp"

#include <Solstice.hpp>

std::unique_ptr<Detour> KeyHook::mDetour = nullptr;

void KeyHook::onKey(uint32_t key, bool isDown)
{
    auto oFunc = mDetour->GetFastcall<void, uint32_t, bool>();
    oFunc(key, isDown);

    Solstice::console->info("KeyHook: key: {0}, isDown: {1}", key, isDown);

    if (key == VK_END)
    {
        if (ClientInstance::get()->getScreenName() != "chat_screen") Solstice::mRequestEject = true;
    }
}

void KeyHook::init()
{
    mName = "KeyHook";
    mDetour = std::make_unique<Detour>("Keyboard::feed", reinterpret_cast<void*>(SigManager::Keyboard_feed), &KeyHook::onKey);
    mDetour->Enable();
}
