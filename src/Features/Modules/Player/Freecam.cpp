//
// Created by vastrakai on 7/22/2024.
//

#include "Freecam.hpp"

#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/CameraInstructionPacket.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Hook/Hooks/NetworkHooks/PacketReceiveHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>

void Freecam::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Freecam::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<LookInputEvent, &Freecam::onLookInputEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &Freecam::onPacketInEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    player->setFlag<OverrideCameraFlag>(true);
}

void Freecam::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Freecam::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &Freecam::onLookInputEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    player->setFlag<OverrideCameraFlag>(false);
    /*auto pkt = MinecraftPackets::createPacket<CameraInstructionPacket>();
    CameraInstruction::SetInstruction instruction;
    instruction.mPreset = -1;
    pkt->mData.mSetInstruction = instruction;

    PacketUtils::sendToSelf(std::reinterpret_pointer_cast<Packet>(pkt));*/
}

void Freecam::onPacketInEvent(PacketInEvent& event)
{
    /*if (event.mPacket->getId() == PacketID::CameraInstruction)
    {
        auto pkt = event.getPacket<CameraInstructionPacket>();
        spdlog::info("CameraInstructionPacket addr: 0x{:X}", reinterpret_cast<uintptr_t>(pkt.get()));
        if (pkt->mData.mSetInstruction.has_value() && pkt->mData.mSetInstruction.value().mPreset.has_value())
            spdlog::info("Preset: {}" + std::to_string(pkt->mData.mSetInstruction.value().mPreset.value()));

        {
            if (pkt->mData.mSetInstruction.has_value())
            {
                spdlog::info("SetInstruction valid");
                auto instruction = pkt->mData.mSetInstruction.value();
                if (instruction.mPreset.has_value())
                {
                    spdlog::info("Preset: {}", instruction.mPreset.value());
                }
                if (instruction.mPos.has_value())
                {
                    spdlog::info("Pos: {}, {}, {}", instruction.mPos.value().mPos.x, instruction.mPos.value().mPos.y, instruction.mPos.value().mPos.z);
                }
                if (instruction.mRot.has_value())
                {
                    spdlog::info("Rot: {}, {}", instruction.mRot.value().mRot.x, instruction.mRot.value().mRot.y);
                }
                if (instruction.mFacing.has_value())
                {
                    spdlog::info("Facing: {}, {}, {}", instruction.mFacing.value().mFacing.x, instruction.mFacing.value().mFacing.y, instruction.mFacing.value().mFacing.z);
                }
                if (instruction.mEase.has_value())
                {
                    spdlog::info("Ease: {}, {}", instruction.mEase.value().mType, instruction.mEase.value().mTime);
                }
                if (instruction.mDefaul.has_value())
                {
                    spdlog::info("Default: {}", instruction.mDefaul.value());
                }
            }
            if (pkt->mData.mFadeInstruction.has_value())
            {
                spdlog::info("FadeInstruction valid");
            }
        }
    }

    if (event.mPacket->getId() == PacketID::Text) {
        auto pkt = event.getPacket<TextPacket>();
        spdlog::info("Text: {}", pkt->mMessage);
    }*/
}

void Freecam::onBaseTickEvent(BaseTickEvent& event)
{
    if (!mTest.mValue) return;
    /*auto pkt = MinecraftPackets::createPacket<CameraInstructionPacket>();
    CameraInstruction::SetInstruction instruction = {};
    pkt->mData.mSetInstruction = instruction;
    PacketReceiveHook::handlePacket(std::reinterpret_pointer_cast<Packet>(pkt));*/
}

void Freecam::onLookInputEvent(LookInputEvent& event)
{
}
