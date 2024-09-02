//
// Created by alteik on 01/09/2024.
//

#include "AutoLootbox.hpp"
#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Modules/Visual/Interface.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Actor/Components/RuntimeIDComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ActorOwnerComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ActorTypeComponent.hpp>
#include <Features/Events/PacketInEvent.hpp>

static glm::vec3 lastTreasurePos;
AABB AutoLootbox::mTargetedAABB = AABB();
bool AutoLootbox::mRotating = false;
uint64_t AutoLootbox::lastHit = 0;

void AutoLootbox::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoLootbox::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &AutoLootbox::onPacketOutEvent>(this);
}

void AutoLootbox::onDisable() {
    lastTreasurePos = {0.f,0.f,0.f};
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoLootbox::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &AutoLootbox::onPacketOutEvent>(this);
}

void AutoLootbox::RotateToTreasure(Actor* target) {
    if (!target) return;
    mTargetedAABB = target->getAABB();
    mRotating = true;
}

void AutoLootbox::onBaseTickEvent(BaseTickEvent& event) {
    auto player = event.mActor;
    if (!player) return;
    std::vector<struct Actor *> actors;

    for (auto &&[entId, moduleOwner, type, ridc, balls]: player->mContext.mRegistry->view<ActorOwnerComponent, ActorTypeComponent, RuntimeIDComponent, AABBShapeComponent>().each()) {

        if (!moduleOwner.mActor) {
            continue;
        };

        if(moduleOwner.mActor->mEntityIdentifier != "hivesky:death_crate")
            continue;

        if(player->distanceTo(moduleOwner.mActor) > 4)
            continue;

        actors.push_back(moduleOwner.mActor);
    }


    if (NOW - lastHit < 200) return;

    if (actors.empty()) return;

    class Actor *closestActor = nullptr;

    for (auto actor: actors) {
        if (player->distanceTo(actor) < 5) {
            closestActor = actor;
        }
    }

    if (!closestActor) return;
    if (lastTreasurePos == *closestActor->getPos()) return;

    lastHit = NOW;

    RotateToTreasure(closestActor);

    if (mMode.mValue == Mode::Open) {
        player->swing();
        player->getGameMode()->interact(closestActor, *closestActor->getPos());
        player->getGameMode()->buildBlock(*closestActor->getPos(), BlockUtils::getExposedFace(*closestActor->getPos()), 0);
    } else if (mMode.mValue == Mode::Break) {
        player->swing();
        player->getGameMode()->attack(closestActor);
    }

    lastTreasurePos = *closestActor->getPos();
}

void AutoLootbox::onPacketOutEvent(PacketOutEvent& event) {
    if (!mRotating) return;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput) {
        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        glm::vec2 rots = MathUtils::getRots(*player->getPos(), mTargetedAABB);
        pkt->mRot = rots;
        pkt->mYHeadRot = rots.y;
    } else if (event.mPacket->getId() == PacketID::MovePlayer) {
        auto pkt = event.getPacket<MovePlayerPacket>();
        glm::vec2 rots = MathUtils::getRots(*player->getPos(), mTargetedAABB);
        pkt->mRot = rots;
        pkt->mYHeadRot = rots.y;
    }

    mRotating = false;
}

void AutoLootbox::onPacketInEvent(class PacketInEvent& event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if(event.mPacket->getId() == PacketID::ChangeDimension)
    {
        lastTreasurePos = {0.f,0.f,0.f};
    }
}