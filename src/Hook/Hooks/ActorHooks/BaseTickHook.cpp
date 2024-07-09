//
// Created by vastrakai on 6/25/2024.
//

#include "BaseTickHook.hpp"

#include <SDK/Minecraft/Actor/Actor.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Features/Events/BaseTickEvent.hpp>


std::unique_ptr<Detour> BaseTickHook::mDetour = nullptr;

void BaseTickHook::onBaseTick(Actor* actor)
{
    auto oFunc = mDetour->getOriginal<decltype(&onBaseTick)>();
    if (actor != ClientInstance::get()->getLocalPlayer()) return oFunc(actor);

    auto holder = nes::make_holder<BaseTickEvent>(actor);
    gFeatureManager->mDispatcher->trigger(holder);

    return oFunc(actor);
}

void BaseTickHook::init()
{
    mName = "Actor::baseTick";
    mDetour = std::make_unique<Detour>("Actor::baseTick", reinterpret_cast<void*>(ClientInstance::get()->getLocalPlayer()->vtable[OffsetProvider::Actor_baseTick]), &BaseTickHook::onBaseTick);
    mDetour->enable();
}
