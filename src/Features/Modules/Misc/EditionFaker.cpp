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

    {
        MemUtils::ReadBytes((void*)func, mOriginalData, sizeof(mOriginalData));
        MemUtils::writeBytes(func, "\xEB", 1);
    }

    {
        MemUtils::ReadBytes((void*)func2, mOriginalDefaultInputMode, sizeof(mOriginalDefaultInputMode));

        MemUtils::writeBytes(func2, "\xEB\x18", 2);
        MemUtils::NopBytes(func2 + 2, 29 - 5);
        MemUtils::writeBytes(func2 + 26, "\xB8", 1);

        MemUtils::ReadBytes((void*)func3, mOriginalData1, sizeof(mOriginalData1));
        MemUtils::ReadBytes((void*)(func3+18), mOriginalData2, sizeof(mOriginalData2));
        MemUtils::ReadBytes((void*)func4, mOriginalData3, sizeof(mOriginalData3));

        mPatch1Ptr = AllocateBuffer((void*)func3);
        mPatch2Ptr = AllocateBuffer((void*)(func3+18));
        mPatch3Ptr = AllocateBuffer((void*)func4);

        MemUtils::writeBytes((uintptr_t)mPatch1Ptr, mPatch1, sizeof(mPatch1));
        MemUtils::writeBytes((uintptr_t)mPatch2Ptr, mPatch2, sizeof(mPatch2));
        MemUtils::writeBytes((uintptr_t)mPatch3Ptr, mPatch3, sizeof(mPatch3));

        auto toOriginalAddrRip = MemUtils::GetRelativeAddress((uintptr_t)mPatch1Ptr+sizeof(mPatch1)+1, func3+5);
        auto toOriginalAddrRip2 = MemUtils::GetRelativeAddress((uintptr_t)mPatch2Ptr+sizeof(mPatch2)+1, func3+18+5);
        auto toOriginalAddrRip3 = MemUtils::GetRelativeAddress((uintptr_t)mPatch3Ptr+sizeof(mPatch3)+1, func4+5);

        MemUtils::writeBytes((uintptr_t)mPatch1Ptr + sizeof(mPatch1), "\xE9", 1);
        MemUtils::writeBytes((uintptr_t)mPatch2Ptr + sizeof(mPatch2), "\xE9", 1);
        MemUtils::writeBytes((uintptr_t)mPatch3Ptr + sizeof(mPatch3), "\xE9", 1);

        MemUtils::writeBytes((uintptr_t)mPatch1Ptr + sizeof(mPatch1) + 1, &toOriginalAddrRip, sizeof(int32_t));
        MemUtils::writeBytes((uintptr_t)mPatch2Ptr + sizeof(mPatch2) + 1, &toOriginalAddrRip2, sizeof(int32_t));
        MemUtils::writeBytes((uintptr_t)mPatch3Ptr + sizeof(mPatch3) + 1, &toOriginalAddrRip3, sizeof(int32_t));

        auto newRelRip1 = MemUtils::GetRelativeAddress(func3 + 1, (uintptr_t)mPatch1Ptr);
        auto newRelRip2 = MemUtils::GetRelativeAddress(func3+18 + 1, (uintptr_t)mPatch2Ptr);
        auto newRelRip3 = MemUtils::GetRelativeAddress(func4 + 1, (uintptr_t)mPatch3Ptr);

        MemUtils::writeBytes(func3, "\xE9", 1);
        MemUtils::writeBytes(func3 + 18, "\xE9", 1);
        MemUtils::writeBytes(func4, "\xE9", 1);

        MemUtils::writeBytes(func3+1, &newRelRip1, sizeof(int32_t));
        MemUtils::writeBytes(func3+18+1, &newRelRip2, sizeof(int32_t));
        MemUtils::writeBytes(func4+1, &newRelRip3, sizeof(int32_t));
    }
}

void EditionFaker::eject() {
    {
        MemUtils::writeBytes(func, mOriginalData, sizeof(mOriginalData));
    }

    {
        MemUtils::writeBytes(func2, mOriginalDefaultInputMode, sizeof(mOriginalDefaultInputMode));
        MemUtils::writeBytes(func3, mOriginalData1, sizeof(mOriginalData1));
        MemUtils::writeBytes(func3+18, mOriginalData2, sizeof(mOriginalData2));
        MemUtils::writeBytes(func4, mOriginalData3, sizeof(mOriginalData3));

        FreeBuffer(mPatch1Ptr);
        FreeBuffer(mPatch2Ptr);
        FreeBuffer(mPatch3Ptr);
    }
}

void EditionFaker::spoofEdition() {

    {
        int32_t DeviceOS = mOs.as<int>();
        MemUtils::writeBytes(func + 27, &DeviceOS, sizeof(int32_t));
    }

    {
        int32_t DefaultInputMode = mInputMethod.as<int>();
        MemUtils::writeBytes(func2 + 27, &DefaultInputMode, sizeof(int32_t));
        MemUtils::writeBytes((uintptr_t)mPatch1Ptr+2, &DefaultInputMode, sizeof(int32_t));
        MemUtils::writeBytes((uintptr_t)mPatch3Ptr+2, &DefaultInputMode, sizeof(int32_t));
    }
}

void EditionFaker::onEnable() {
    inject();
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &EditionFaker::onPacketOutEvent, nes::event_priority::ABSOLUTE_LAST>(this);
    gFeatureManager->mDispatcher->listen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent, nes::event_priority::ABSOLUTE_LAST>(this);
}

void EditionFaker::onDisable() {
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &EditionFaker::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<ConnectionRequestEvent, &EditionFaker::onConnectionRequestEvent>(this);
    eject();
}

void EditionFaker::onConnectionRequestEvent(ConnectionRequestEvent& event) {
    spoofEdition();
    event.mInputMode = mInputMethod.as<int>();
}

void EditionFaker::onPacketOutEvent(PacketOutEvent &event) {
    if(event.mPacket->getId() == PacketID::PlayerAuthInput) {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        paip->mInputMode = (InputMode) mInputMethod.as<int>();
    }
} 
