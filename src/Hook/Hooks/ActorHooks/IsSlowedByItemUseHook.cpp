//
// Created by vastrakai on 7/3/2024.
//

#include "IsSlowedByItemUseHook.hpp"
#include <Features/Events/ItemSlowdownEvent.hpp>

std::unique_ptr<Detour> IsSlowedByItemUseHook::mDetour = nullptr;

void* IsSlowedByItemUseHook::onIsSlowedByItemUse(void* a1, void* a2, void* a3)
{
    auto oFunc = mDetour->getOriginal<decltype(&onIsSlowedByItemUse)>();

    auto holder = nes::make_holder<ItemSlowdownEvent>();
    spdlog::trace("ItemUseSlowdownSystem::isSlowedByItemUse triggered slowdown event");
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return nullptr;

    return oFunc(a1, a2, a3);
}

void IsSlowedByItemUseHook::init()
{
    mDetour = std::make_unique<Detour>("ItemUseSlowdownSystem::isSlowedByItemUse", reinterpret_cast<void*>(SigManager::ItemUseSlowdownSystem_isSlowedByItemUse), &onIsSlowedByItemUse);
}
