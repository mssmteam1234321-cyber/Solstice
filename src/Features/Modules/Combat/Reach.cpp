//
// Created by alteik on 04/09/2024.
//

#include "Reach.hpp"
#include <SDK/SigManager.hpp>

void Reach::onInit() {
    uintptr_t addr = SigManager::Reach;
    addr += 4;
    int offset = *reinterpret_cast<int*>(addr);
    mReachPtr = reinterpret_cast<float*>(addr + offset + 4);

    uintptr_t addr2 = SigManager::BlockReach;
    addr2 += 4;
    int offset2 = *reinterpret_cast<int*>(addr2);
    mBlockReachPtr = reinterpret_cast<float*>(addr2 + offset2 + 4);
}

void Reach::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Reach::onBaseTickEvent>(this);
}

void Reach::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Reach::onBaseTickEvent>(this);
    MemUtils::Write((uintptr_t) mReachPtr, 3.f);
    MemUtils::Write((uintptr_t) mBlockReachPtr, 5.7f);
}

void Reach::onBaseTickEvent(class BaseTickEvent &event) {
    MemUtils::Write((uintptr_t) mReachPtr, mCombatReach.mValue);
    MemUtils::Write((uintptr_t) mBlockReachPtr, mBlockReach.mValue);
}