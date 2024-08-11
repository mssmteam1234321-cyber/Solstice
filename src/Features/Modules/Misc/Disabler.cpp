//
// Created by vastrakai on 8/9/2024.
//

#include "Disabler.hpp"

#include <Features/Events/SendImmediateEvent.hpp>

void Disabler::onEnable()
{
    gFeatureManager->mDispatcher->listen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
}

void Disabler::onDisable()
{
    gFeatureManager->mDispatcher->deafen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
}

static constexpr unsigned char newTimestamp[17] = { 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };

void Disabler::onSendImmediateEvent(SendImmediateEvent& event)
{

    if (mMode.mValue != Mode::FlareonOld) return;
    if (event.send[0] == 3)
    {
        event.send = (char*)&newTimestamp;
        event.a2 = 136;
        event.mModified = true;
        // spdlog::info("Updated disabler");
    }
}

