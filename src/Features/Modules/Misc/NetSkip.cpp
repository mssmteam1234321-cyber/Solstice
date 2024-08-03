//
// Created by vastrakai on 8/3/2024.
//

#include "NetSkip.hpp"

int64_t NetSkip::getDelay() const
{
    return mRandomizeDelay.mValue ? MathUtils::random(mRandomizeMin.as<int>(), mRandomizeMax.as<int>()) : mDelay.as<int64_t>();
}

void NetSkip::onEnable()
{
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &NetSkip::onRunUpdateCycleEvent>(this);
}

void NetSkip::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &NetSkip::onRunUpdateCycleEvent>(this);
}

void NetSkip::onRunUpdateCycleEvent(RunUpdateCycleEvent& event)
{
    Sleep(getDelay());
}
