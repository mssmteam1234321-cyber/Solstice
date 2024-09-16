//
// Created by Tozic on 9/16/2024.
//

#include "SkinBlinker.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/Actor/SerializedSkin.hpp>

void SkinBlinker::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &SkinBlinker::onBaseTickEvent>(this);
}

void SkinBlinker::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &SkinBlinker::onBaseTickEvent>(this);
}

// This is GOING to be ClientSided unless i figure out what packets are sent/recieved
void SkinBlinker::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    std::vector<Actor*> actors = ActorUtils::getActorList(true, true);

    for (Actor* actor : actors)
    {
        if (actor != player)
        {
            SerializedSkin* mLocalSkin = player->getSkin();
            SerializedSkin* mTargetSkin = actor[0].getSkin();

            if (timerMap.find("SkinBlinkerDelay") == timerMap.end())
            {
                timerMap.insert({ "SkinBlinkerDelay", std::chrono::high_resolution_clock::now() });
            }

            auto entry = timerMap.find("SkinBlinkerDelay");
            if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - entry->second).count() / 1000000.F >= 15000) // 15000 is the delay of ms I want it to delay
            {
                // My logic here

                mLocalSkin->skinWidth = mTargetSkin->skinWidth;
                mLocalSkin->skinHeight = mTargetSkin->skinHeight;
                mLocalSkin->skinData = mTargetSkin->skinData;

                // For later on :P
                // Uncomment for cape stealing logic (To make later)
                /*if (mStealCapes.mValue)
                {
                    int bytes = mTargetSkin->mCapeImage.mWidth * mTargetSkin->mCapeImage.mHeight * 4;

                    if (bytes > 0)
                    {
                        mLocalSkin->mCapeImage.mWidth = mTargetSkin->mCapeImage.mWidth;
                        mLocalSkin->mCapeImage.mHeight = mTargetSkin->mCapeImage.mHeight;
                        mLocalSkin->mCapeImage.mImageBytes = mTargetSkin->mCapeImage.mImageBytes;
                    }
                }*/

                if (timerMap.find("SkinBlinkerDelay") != timerMap.end()) {
                    timerMap.erase("SkinBlinkerDelay");
                }
            }
        }
    }
}