//
// Created by alteik on 25/09/2024.
//

#include "EditionFaker.hpp"
#include <SDK/SigManager.hpp>
#include <Utils/Buffer.hpp>
#include <Features/Events/ConnectionRequestEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/ConnectionRequest.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

static uintptr_t func;
static uintptr_t func2;
static uintptr_t func3;
static uintptr_t func4;

void EditionFaker::onInit() {
    func = SigManager::ConnectionRequest_create_DeviceOS;
    func2 = SigManager::ConnectionRequest_create_DefaultInputMode;
    func3 = SigManager::GetInputModeBypass;
    func4 = SigManager::MobileRotations;
}

void EditionFaker::inject() {

    MemUtils::ReadBytes((void*)func, originalData, sizeof(originalData));

    MemUtils::writeBytes(func, "\xEB", 1);

    int32_t DeviceOS = mOs.as<int>();

    MemUtils::writeBytes(func + 27, &DeviceOS, sizeof(int32_t));

    MemUtils::ReadBytes((void*)func2, originalDefaultInputMode, sizeof(originalDefaultInputMode));

    MemUtils::writeBytes(func2, "\xEB\x18", 2);

    MemUtils::NopBytes(func2 + 2, 29 - 5);

    MemUtils::writeBytes(func2 + 26, "\xB8", 1);

    int32_t DefaultInputMode = mInputMethod.as<int>();

    MemUtils::writeBytes(func2 + 27, &DefaultInputMode, sizeof(int32_t));

    MemUtils::ReadBytes((void*)func3, originalData1, sizeof(originalData1));
    MemUtils::ReadBytes((void*)(func3+18), originalData2, sizeof(originalData2));
    MemUtils::ReadBytes((void*)func4, originalData3, sizeof(originalData3));

    patch1Ptr = AllocateBuffer((void*)func3);
    patch2Ptr = AllocateBuffer((void*)(func3+18));
    patch3Ptr = AllocateBuffer((void*)func4);

    MemUtils::writeBytes((uintptr_t)patch1Ptr, patch1, sizeof(patch1));
    MemUtils::writeBytes((uintptr_t)patch2Ptr, patch2, sizeof(patch2));
    MemUtils::writeBytes((uintptr_t)patch3Ptr, patch3, sizeof(patch3));

    MemUtils::writeBytes((uintptr_t)patch1Ptr+2, &DefaultInputMode, sizeof(int32_t));
    MemUtils::writeBytes((uintptr_t)patch3Ptr+2, &DefaultInputMode, sizeof(int32_t));

    auto toOriginalAddrRip = MemUtils::GetRelativeAddress((uintptr_t)patch1Ptr+sizeof(patch1)+1, func3+5);
    auto toOriginalAddrRip2 = MemUtils::GetRelativeAddress((uintptr_t)patch2Ptr+sizeof(patch2)+1, func3+18+5);
    auto toOriginalAddrRip3 = MemUtils::GetRelativeAddress((uintptr_t)patch3Ptr+sizeof(patch3)+1, func4+5);

    MemUtils::writeBytes((uintptr_t)patch1Ptr + sizeof(patch1), "\xE9", 1);
    MemUtils::writeBytes((uintptr_t)patch2Ptr + sizeof(patch2), "\xE9", 1);
    MemUtils::writeBytes((uintptr_t)patch3Ptr + sizeof(patch3), "\xE9", 1);

    MemUtils::writeBytes((uintptr_t)patch1Ptr + sizeof(patch1) + 1, &toOriginalAddrRip, sizeof(int32_t));
    MemUtils::writeBytes((uintptr_t)patch2Ptr + sizeof(patch2) + 1, &toOriginalAddrRip2, sizeof(int32_t));
    MemUtils::writeBytes((uintptr_t)patch3Ptr + sizeof(patch3) + 1, &toOriginalAddrRip3, sizeof(int32_t));
    
    auto newRelRip1 = MemUtils::GetRelativeAddress(func3 + 1, (uintptr_t)patch1Ptr);
    auto newRelRip2 = MemUtils::GetRelativeAddress(func3+18 + 1, (uintptr_t)patch2Ptr);
    auto newRelRip3 = MemUtils::GetRelativeAddress(func4 + 1, (uintptr_t)patch3Ptr);

    MemUtils::writeBytes(func3, "\xE9", 1);
    MemUtils::writeBytes(func3 + 18, "\xE9", 1);
    MemUtils::writeBytes(func4, "\xE9", 1);

    MemUtils::writeBytes(func3+1, &newRelRip1, sizeof(int32_t));
    MemUtils::writeBytes(func3+18+1, &newRelRip2, sizeof(int32_t));
    MemUtils::writeBytes(func4+1, &newRelRip3, sizeof(int32_t));

    isInjected = true;
    spdlog::info("[EditionFaker] injected");
}

void EditionFaker::eject() {
    MemUtils::writeBytes(func, originalData, sizeof(originalData));
    MemUtils::writeBytes(func2, originalDefaultInputMode, sizeof(originalDefaultInputMode));
    MemUtils::writeBytes(func3, originalData1, sizeof(originalData1));
    MemUtils::writeBytes(func3+18, originalData2, sizeof(originalData2));
    MemUtils::writeBytes(func4, originalData3, sizeof(originalData3));

    FreeBuffer(patch1Ptr);
    FreeBuffer(patch2Ptr);
    FreeBuffer(patch3Ptr);

    isInjected = false;
    spdlog::info("[EditionFaker] ejected");
}

void EditionFaker::spoofEdition() {
    eject();
    inject();
}

void EditionFaker::onEnable() {
    spdlog::info("[EditionFaker] enable");
    inject();
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &EditionFaker::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent>(this);
}

void EditionFaker::onDisable() {
    spdlog::info("[EditionFaker] disable");
    if(isInjected) eject();
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &EditionFaker::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent>(this);
}

void EditionFaker::onConnectionRequestEvent(ConnectionRequestEvent& event) {
    spdlog::info("[EditionFaker] spoof");
    spoofEdition();
}

void EditionFaker::onPacketOutEvent(PacketOutEvent &event) {
    if(event.mPacket->getId() == PacketID::PlayerAuthInput) {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        paip->mInputMode = (InputMode) mInputMethod.as<int>();
    }
}