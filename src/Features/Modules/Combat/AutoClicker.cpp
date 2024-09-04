//
// Created by alteik on 04/09/2024.
//

#include "AutoClicker.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <Features/Modules/Misc/AntiBot.hpp>

void AutoClicker::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoClicker::onBaseTickEvent>(this);
}

void AutoClicker::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoClicker::onBaseTickEvent>(this);
}

Actor* AutoClicker::GetActorFromEntityId(EntityId entityId)
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
        if(antiBot->isBot(actor)) continue;
        if(actor->getmEntityIdentifier() == "hivecommon:shadow" || actor->getmEntityIdentifier() == "minecraft:pig") continue;
        if(actor == player) continue;

        return actor;
    }

    return nullptr;
}

static uint64_t lastAttack;

void AutoClicker::onBaseTickEvent(class BaseTickEvent &event) {

    if(!ImGui::IsMouseDown(0)) return;

    auto player = event.mActor;
    if(!player) return;

    if (NOW - lastAttack < 1000 / mCPS.mValue) return;

    player->swing();
    HitResult* hitResult = player->getLevel()->getHitResult();

    if(hitResult->mType == HitType::ENTITY)
    {
        Actor* actor = GetActorFromEntityId(hitResult->mEntity.id);
        if(!actor) return;

        lastAttack = NOW;
        player->getGameMode()->attack(actor);
    }
}