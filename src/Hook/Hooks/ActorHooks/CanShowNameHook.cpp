//
// Created by vastrakai on 8/11/2024.
//

#include "CanShowNameHook.hpp"

#include <Features/Events/CanShowNameTagEvent.hpp>

std::unique_ptr<Detour> CanShowNameHook::mDetour = nullptr;

bool CanShowNameHook::onCanShowNameTag(void* a1, Actor* actor)
{
    auto original = mDetour->getOriginal<decltype(&onCanShowNameTag)>();
    bool result = original(a1, actor);

    auto holder = nes::make_holder<CanShowNameTagEvent>(a1, actor, result);
    gFeatureManager->mDispatcher->trigger(holder);
    return holder->getResult();
}

void CanShowNameHook::init()
{
    mDetour = std::make_unique<Detour>("Unknown::canShowNameTag", reinterpret_cast<void*>(SigManager::Unknown_canShowNameTag), &onCanShowNameTag);
}
