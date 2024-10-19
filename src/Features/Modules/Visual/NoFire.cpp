//
// Created by alteik on 18/10/2024.
//

#include "NoFire.hpp"
#include <SDK/SigManager.hpp>
#include <Utils/MemUtils.hpp>

std::vector<unsigned char> gFrBytes = {0xC3}; // 0xC3 is the opcode for "ret"
DEFINE_PATCH_FUNC(patchFireRender, SigManager::FireRender, gFrBytes);

void NoFire::onEnable()
{
    patchFireRender(true);
}

void NoFire::onDisable()
{
    patchFireRender(false);
}