//
// Created by alteik on 04/09/2024.
//

#include "Reach.hpp"
#include <SDK/SigManager.hpp>

float* Reach::reachPtr = nullptr;

void Reach::onInit() {
    uintptr_t addr = SigManager::Reach;
    addr += 4;
    int offset = *reinterpret_cast<int*>(addr);
    reachPtr = reinterpret_cast<float*>(addr + offset + 4);
}

void Reach::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Reach::onBaseTickEvent>(this);
}

void Reach::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Reach::onBaseTickEvent>(this);
    MemUtils::Write((uintptr_t) reachPtr, 3.f);
}

void Reach::onBaseTickEvent(class BaseTickEvent &event) {
    MemUtils::Write((uintptr_t) reachPtr, mDistance.mValue);
}