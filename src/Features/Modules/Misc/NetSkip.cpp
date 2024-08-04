//
// Created by vastrakai on 8/3/2024.
//

#include "NetSkip.hpp"

#include <Features/Events/RunUpdateCycleEvent.hpp>

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
    if (mMode.mValue == Mode::Milliseconds)
    {
        Sleep(mRandomizeDelayMs.mValue ? MathUtils::random(mRandomizeMinMs.as<int>(), mRandomizeMaxMs.as<int>()) : mDelayMs.as<int64_t>());
        return;
    }

    if (mCurrentTick < mTickDelay)
    {
        mCurrentTick++;
        event.cancel();
        return;
    }

    mCurrentTick = 0;
    mTickDelay = mRandomizeTicks.mValue ? MathUtils::random(mTicksMin.as<int>(), mTicksMax.as<int>()) : mTicks.as<int>();

}
