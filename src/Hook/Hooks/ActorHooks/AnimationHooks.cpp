//
// Created by vastrakai on 7/18/2024.
//

#include "AnimationHooks.hpp"

#include <Features/Events/SwingDurationEvent.hpp>

std::unique_ptr<Detour> AnimationHooks::mDetour;

int AnimationHooks::getCurrentSwingDuration(Actor* actor)
{
    auto original = mDetour->getOriginal<decltype(&getCurrentSwingDuration)>();
    int result = original(actor);

    auto holder = nes::make_holder<SwingDurationEvent>(result);
    gFeatureManager->mDispatcher->trigger(holder);
    result = holder->mSwingDuration;

    return result;
}

void AnimationHooks::init()
{
    auto func = SigManager::Mob_getCurrentSwingDuration;
    mDetour = std::make_unique<Detour>("Mob::getCurrentSwingDuration", reinterpret_cast<void*>(func), &getCurrentSwingDuration);
    mDetour->enable();
}
