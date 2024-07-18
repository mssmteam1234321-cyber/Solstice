//
// Created by vastrakai on 7/18/2024.
//

#include "SafeWalk.hpp"

DEFINE_NOP_PATCH_FUNC(patchSafeWalk, SigManager::SneakMovementSystem_tickSneakMovementSystem, 2);

void SafeWalk::onEnable()
{
    patchSafeWalk(true);
}

void SafeWalk::onDisable()
{
    patchSafeWalk(false);
}

void SafeWalk::onBaseTickEvent(BaseTickEvent& event)
{
    patchSafeWalk(mEnabled);
}
