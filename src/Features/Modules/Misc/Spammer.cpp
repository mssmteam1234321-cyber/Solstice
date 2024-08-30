//
// 8/30/2024.
//

#include "Spammer.hpp"

#include <Features/Events/BaseTickEvent.hpp>

void Spammer::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Spammer::onBaseTickEvent>(this);
    mLastMessageSent = NOW;
}

void Spammer::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Spammer::onBaseTickEvent>(this);
}

void Spammer::onBaseTickEvent(BaseTickEvent& event)
{
    if (NOW <= mLastMessageSent + mDelayMs.mValue) return;

    PacketUtils::sendChatMessage(mSpammerMessageTemplate.getEntry());
    mLastMessageSent = NOW;
}