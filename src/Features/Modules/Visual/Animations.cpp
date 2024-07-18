//
// Created by vastrakai on 7/18/2024.
//

#include "Animations.hpp"
#include <Features/Events/SwingDurationEvent.hpp>

void Animations::onEnable()
{
    gFeatureManager->mDispatcher->listen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
}

void Animations::onDisable()
{
    gFeatureManager->mDispatcher->deafen<SwingDurationEvent, &Animations::onSwingDurationEvent>(this);
}

void Animations::onSwingDurationEvent(SwingDurationEvent& event)
{
    event.mSwingDuration = mSwingTime.as<int>();
}
