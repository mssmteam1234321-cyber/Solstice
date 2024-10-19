//
// Created by alteik on 18/10/2024.
//

#include "NoFire.hpp"
#include <SDK/SigManager.hpp>

void NoFire::onInit()
{
    mFuncAddr = SigManager::FireRender;
}

void NoFire::onEnable()
{
    MemUtils::ReadBytes((void*)mFuncAddr, mOriginalData, sizeof(mOriginalData));
    MemUtils::writeBytes(mFuncAddr, "\xC3", 1);
}

void NoFire::onDisable()
{
    MemUtils::writeBytes(mFuncAddr, mOriginalData, sizeof(mOriginalData));
}