//
// Created by vastrakai on 8/9/2024.
//

#include "Disabler.hpp"

#include <Features/Events/PingUpdateEvent.hpp>
#include <Features/Events/SendImmediateEvent.hpp>

void Disabler::onEnable()
{
    gFeatureManager->mDispatcher->listen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &Disabler::onPingUpdateEvent, nes::event_priority::FIRST>(this);
}

void Disabler::onDisable()
{
    gFeatureManager->mDispatcher->deafen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &Disabler::onPingUpdateEvent>(this);
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

