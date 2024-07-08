//
// Created by vastrakai on 6/28/2024.
//

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Components/ActorOwnerComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ActorTypeComponent.hpp>
#include <SDK/Minecraft/Actor/Components/RuntimeIDComponent.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <entity/registry.hpp>

#include "ActorUtils.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Modules/Misc/AntiBot.hpp>

std::vector<struct Actor *> ActorUtils::getActorList(bool playerOnly, bool excludeBots) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player == nullptr) return {};
    static AntiBot* antibot = gFeatureManager->mModuleManager->getModule<AntiBot>();

    std::vector<struct Actor *> actors;
    for (auto &&[_, moduleOwner, type, ridc]: player->mContext.mRegistry->view<ActorOwnerComponent, ActorTypeComponent, RuntimeIDComponent>().each())
    {
        if (excludeBots && antibot->isBot(moduleOwner.actor)) continue;

        if (type.type == ActorType::Player && playerOnly)
            actors.push_back(moduleOwner.actor);
        else if (!playerOnly)
            actors.push_back(moduleOwner.actor);
    }


    return actors;
}
