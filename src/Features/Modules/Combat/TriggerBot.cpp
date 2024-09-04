//
// Created by alteik on 04/09/2024.
//

#include "TriggerBot.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <Features/Modules/Misc/AntiBot.hpp>

void TriggerBot::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TriggerBot::onBaseTickEvent>(this);
}

void TriggerBot::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TriggerBot::onBaseTickEvent>(this);
}

Actor* TriggerBot::getActorFromEntityId(EntityId entityId)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto antiBot = gFeatureManager->mModuleManager->getModule<AntiBot>();
    if (antiBot == nullptr)
    {
        ChatUtils::displayClientMessage("§cERROR: AntiBot module not found.");
        return nullptr;
    }

    std::vector<Actor*> actors= ActorUtils::getActorList(false, false);
    for (auto actor : actors)
    {
        if(mUseAntibot.mValue && antiBot->isBot(actor)) continue;
        if(mHive.mValue && actor->getmEntityIdentifier() == "hivecommon:shadow" || actor->getmEntityIdentifier() == "minecraft:pig") continue;
        if(actor == player) continue;

        return actor;
    }

    return nullptr;
}

void TriggerBot::onBaseTickEvent(class BaseTickEvent &event) {
    auto player = event.mActor;
    if(!player) return;

    if (mAPSMin.mValue > mAPSMax.mValue) {
        mAPSMin.mValue = mAPSMax.mValue;
    }

    int avgAps = MathUtils::random(mAPSMin.mValue, mAPSMax.mValue);

    HitResult* hitResult = player->getLevel()->getHitResult();

    if(hitResult->mType == HitType::ENTITY)
    {
        Actor* actor = getActorFromEntityId(hitResult->mEntity.id);
        if(!actor) return;

        static uint64_t lastAttack = 0;
        if (NOW - lastAttack < 1000 / avgAps) return;

        player->swing();
        player->getGameMode()->attack(actor);
    }
}