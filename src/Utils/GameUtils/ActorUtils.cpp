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
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
static AntiBot* antibot = nullptr;

std::vector<struct Actor *> ActorUtils::getActorList(bool playerOnly, bool excludeBots)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player == nullptr)
    {
        //spdlog::warn("ActorUtils::getActorList called when player is nullptr");
        return {};
    }

    if (!antibot) antibot = gFeatureManager->mModuleManager->getModule<AntiBot>();
    std::vector<struct Actor *> actors;

    try
    {
        for (auto &&[entId, moduleOwner, type, ridc]: player->mContext.mRegistry->view<ActorOwnerComponent, ActorTypeComponent, RuntimeIDComponent>().each())
        {
            // Check if the entity id exists
            if (!player->mContext.mRegistry->valid(entId))
            {
                spdlog::critical("Found invalid entity id [{}]", entId);
#ifdef __DEBUG__
                ChatUtils::displayClientMessage("§cCHECK CONSOLE");
#endif
                return actors;
            }

            // Continue if the actor is null
            if (!moduleOwner.actor)
            {
                spdlog::critical("Found null actor pointer for entity!");
                continue;
            };
            // Continue if the actor is a bot and we want to exclude bots
            if (excludeBots && antibot->isBot(moduleOwner.actor)) continue;

            if (type.type == ActorType::Player && playerOnly || !playerOnly)
                actors.push_back(moduleOwner.actor);
        }
    } catch (std::exception &e)
    {
        spdlog::error("Error in ActorUtils::getActorList: {}", e.what());
        return {};
    } catch (...)
    {
        spdlog::error("Error in ActorUtils::getActorList: Unknown error");
        return {};
    }


    return actors;
}

std::vector<Actor*> ActorUtils::getActorsOfType(ActorType type)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player == nullptr)
    {
        //spdlog::warn("ActorUtils::getActorList called when player is nullptr");
        return {};
    }

    if (!antibot) antibot = gFeatureManager->mModuleManager->getModule<AntiBot>();
    std::vector<Actor*> actors;

    try
    {
        for (auto &&[_, moduleOwner, typeComponent]: player->mContext.mRegistry->view<ActorOwnerComponent, ActorTypeComponent>().each())
        {
            if (!moduleOwner.actor)
            {
                spdlog::debug("Found null actor pointer for entity!");
                continue;
            };
            // do NOT exclude bots as we are specifically requesting a type

            if (typeComponent.type == type)
                actors.push_back(moduleOwner.actor);
        }
    } catch (std::exception &e)
    {
        spdlog::error("Error in ActorUtils::getActorsOfType: {}", e.what());
        return {};
    } catch (...)
    {
        spdlog::error("Error in ActorUtils::getActorsOfType: Unknown error");
        return {};
    }

    return actors;
}

bool ActorUtils::isBot(Actor* actor)
{
    if (!antibot) antibot = gFeatureManager->mModuleManager->getModule<AntiBot>();
    return antibot->isBot(actor);
}

std::shared_ptr<InventoryTransactionPacket> ActorUtils::createAttackTransaction(Actor* actor, int slot)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    if (slot == -1) slot = player->getSupplies()->mSelectedSlot;
    auto pkt = MinecraftPackets::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ItemUseOnActorInventoryTransaction>();
    cit->slot = slot;
    cit->itemInHand = NetworkItemStackDescriptor(*player->getSupplies()->getContainer()->getItem(slot));
    cit->actorId = actor->getActorUniqueIDComponent()->mUniqueID;
    cit->actionType = ItemUseOnActorInventoryTransaction::ActionType::Attack;
    cit->clickPos = actor->getAABB().getClosestPoint(*player->getPos());
    cit->playerPos = *player->getPos();

    pkt->mTransaction = std::move(cit);

    return pkt;
}

Actor* ActorUtils::getActorFromUniqueId(const int64_t uniqueId)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player == nullptr) return nullptr;

    for (auto &&[_, moduleOwner, ridc, uidc]: player->mContext.mRegistry->view<ActorOwnerComponent, RuntimeIDComponent, ActorUniqueIDComponent>().each())
    {
        if (uidc.mUniqueID == uniqueId && moduleOwner.actor) return moduleOwner.actor;
    }

    return nullptr;
}

Actor* ActorUtils::getActorFromRuntimeID(int64_t runtimeId)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player == nullptr) return nullptr;

    for (auto &&[_, moduleOwner, ridc]: player->mContext.mRegistry->view<ActorOwnerComponent, RuntimeIDComponent>().each())
    {
        if (ridc.runtimeID == runtimeId && moduleOwner.actor) return moduleOwner.actor;
    }

    return nullptr;
}
