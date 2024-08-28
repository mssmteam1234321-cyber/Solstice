//
// Created by vastrakai on 8/9/2024.
//

#include "Disabler.hpp"
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <Features/Events/SendImmediateEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void Disabler::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Disabler::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Disabler::onRunUpdateCycleEvent, nes::event_priority::VERY_FIRST>(this);
    gFeatureManager->mDispatcher->listen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &Disabler::onPingUpdateEvent, nes::event_priority::FIRST>(this);

    mShouldUpdateClientTicks = true;
}

void Disabler::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Disabler::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Disabler::onRunUpdateCycleEvent>(this);
    gFeatureManager->mDispatcher->deafen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &Disabler::onPingUpdateEvent>(this);
}

void Disabler::onPacketOutEvent(PacketOutEvent& event) {
    if (mMode.mValue != Mode::Sentinel) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput) {
        static bool desyncY = false;
        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        if (mShouldUpdateClientTicks) mClientTicks = pkt->mClientTick;
        pkt->mInputData |= AuthInputAction::START_GLIDING | AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN | AuthInputAction::NORTH_JUMP_DEPRECATED | AuthInputAction::START_JUMPING;
        pkt->mInputData &= ~AuthInputAction::STOP_GLIDING;
        glm::vec3 pos = pkt->mPos;
        glm::vec3 deltaPos = pkt->mPosDelta;
        glm::vec3 lastPos = pkt->mPos - deltaPos;

        // send tampared packet
        pkt->mClientTick = mClientTicks;
        pkt->mPos = { INT_MAX, INT_MAX, INT_MAX };
        ClientInstance::get()->getPacketSender()->sendToServer(pkt);
        mClientTicks++;

        pkt->mClientTick = mClientTicks;
        pkt->mPos = { -INT_MAX, -INT_MAX, -INT_MAX };
        ClientInstance::get()->getPacketSender()->sendToServer(pkt);
        mClientTicks++;

        // send normal packet
        pkt->mClientTick = mClientTicks;
        pkt->mPos = pos;
        mClientTicks++;
    }
    else if (event.mPacket->getId() == PacketID::NetworkStackLatency) {
        event.mCancelled = true;
    }
}

void Disabler::onRunUpdateCycleEvent(RunUpdateCycleEvent& event) {
    if (mMode.mValue != Mode::Sentinel) return;

    if (event.isCancelled() || event.mApplied) return;

    Sleep(101);
}

static constexpr unsigned char newTimestamp[9] = { 0x3, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF };

int64_t Disabler::getDelay() const
{
    if (mRandomizeDelay.mValue)
    {
        return MathUtils::random(mMinDelay.mValue, mMaxDelay.mValue);
    }
    return mDelay.mValue;
}

void Disabler::onPingUpdateEvent(PingUpdateEvent& event)
{
    if (mMode.mValue != Mode::FlareonOld) return;
    if (mDisablerType.mValue == DisablerType::PingSpoof)
    {
        //event.mPing = 0; // hide ping visually
    }
}

void Disabler::onSendImmediateEvent(SendImmediateEvent& event)
{

    if (mMode.mValue != Mode::FlareonOld) return;
    uint8_t packetId = event.send[0];
    if (packetId == 0)
    {
        if (mDisablerType.mValue == DisablerType::PingHolder) {
            event.cancel();
            return;
        }

        uint64_t timestamp = *reinterpret_cast<uint64_t*>(&event.send[1]);
        timestamp = _byteswap_uint64(timestamp);
        timestamp -= getDelay();
        uint64_t now = NOW;
        timestamp = _byteswap_uint64(timestamp);
        *reinterpret_cast<uint64_t*>(&event.send[1]) = timestamp;

        event.mModified = true;
        spdlog::info("Updated disabler");
    }
}

