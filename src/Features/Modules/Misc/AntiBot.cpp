//
// Created by vastrakai on 7/8/2024.
//

#include "AntiBot.hpp"

#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <Features/Modules/Player/Teams.hpp>

void AntiBot::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AntiBot::onBaseTickEvent>(this);
}

void AntiBot::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AntiBot::onBaseTickEvent>(this);
}

void AntiBot::onBaseTickEvent(BaseTickEvent& event)
{
    if (mMode.mValue == Mode::Simple)
    {
        // Set settings to preset values
        mHitboxCheck.mValue = true;
        mPlayerCheck.mValue = true;
        mInvisibleCheck.mValue = true;
        mNameCheck.mValue = true;
    }

    /*auto actors = ActorUtils::getActorList(true);

    for (auto actor : actors)
    {
        if (mPlayerCheck.mValue && actor->isPlayer()) continue;
    }*/
}

/*float normalPlayerHeightMax = 1.81f;
float normalPlayerHeightMin = 1.35f;
float normalPlayerWidthMin = 0.54f;
float normalPlayerWidthMax = 0.66f;*/

#define NORMAL_PLAYER_HEIGHT_MAX 1.81f
#define NORMAL_PLAYER_HEIGHT_MIN 1.35f
#define NORMAL_PLAYER_WIDTH_MIN 0.54f
#define NORMAL_PLAYER_WIDTH_MAX 0.66f

bool AntiBot::isBot(Actor* actor) const {
    if (!mEnabled) return false;
    if (mPlayerCheck.mValue && !actor->isPlayer()) return true;
    auto aabbShapeComponent = actor->getAABBShapeComponent();
    if (!aabbShapeComponent) return true;

    float hitboxWidth = aabbShapeComponent->mWidth;
    float hitboxHeight = aabbShapeComponent->mHeight;
    // Return if the hitbox dimensions are incorrect
    if (mHitboxCheck.mValue && (hitboxWidth < NORMAL_PLAYER_WIDTH_MIN || hitboxWidth > NORMAL_PLAYER_WIDTH_MAX ||
                                hitboxHeight < NORMAL_PLAYER_HEIGHT_MIN || hitboxHeight > NORMAL_PLAYER_HEIGHT_MAX))
        return true;

    if (mInvisibleCheck.mValue && actor->getStatusFlag(ActorFlags::Invisible)) return true;

    if (mNameCheck.mValue) {
        std::string nameTagString = actor->getNameTag();
        if (std::ranges::count(nameTagString, '\n') > 0) {
            return true;
        }
    }

    return false;
}
