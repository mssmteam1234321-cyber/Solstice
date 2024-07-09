//
// Created by vastrakai on 7/8/2024.
//

#include "Aura.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

void Aura::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Aura::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Aura::onPacketOutEvent>(this);
}

void Aura::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Aura::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Aura::onPacketOutEvent>(this);
}

void Aura::rotate(Actor* target)
{
    if (mRotateMode.as<RotateMode>() == RotateMode::None) return;
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    mTargetedAABB = target->getAABB();
    mRotating = true;
}

void Aura::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor; // Local player
    if (!player) return;

    static std::vector<std::shared_ptr<InventoryTransactionPacket>> queuedTransactions;

    for (auto& transaction : queuedTransactions)
    {
        ClientInstance::get()->getPacketSender()->sendToServer(transaction.get());
    }

    auto actors = ActorUtils::getActorList(false, true);

    static std::unordered_map<Actor*, int64_t> lastAttacks = {};

    // Sort actors by lastAttack if mode is switch
    if (mMode.as<Mode>() == Mode::Switch)
    {
        std::ranges::sort(actors, [&](Actor* a, Actor* b) -> bool
        {
            return lastAttacks[a] < lastAttacks[b];
        });
    }
    else if (mMode.as<Mode>() == Mode::Multi)
    {
        // Sort actors by distance if mode is multi
        std::ranges::sort(actors, [&](Actor* a, Actor* b) -> bool
        {
            return a->distanceTo(player) < b->distanceTo(player);
        });
    }

    static int64_t lastAttack = 0;
    int64_t now = NOW;
    float aps = mAPS.mValue;
    if (mRandomizeAPS.mValue)
    {
        aps = mAPSMin.mValue + (rand() % (int)(mAPSMax.mValue - mAPSMin.mValue));
    }
    int64_t delay = 1000 / aps;

    bool foundAttackable = false;

    for (auto actor : actors)
    {
        if (actor == player) continue;
        if (actor->distanceTo(player) > mRange.mValue) continue;

        rotate(actor);
        foundAttackable = true;

        if (now - lastAttack < delay) return;

        player->swing();
        if (mAttackMode.as<AttackMode>() == AttackMode::Synched)
        {
            std::shared_ptr<InventoryTransactionPacket> attackTransaction = ActorUtils::createAttackTransaction(actor);
            queuedTransactions.push_back(attackTransaction);
        } else {
            player->getGameMode()->attack(actor);
        }

        lastAttack = now;
        lastAttacks[actor] = now;
        if (mMode.as<Mode>() == Mode::Single || mMode.as<Mode>() == Mode::Switch) break;
    }

    if (!foundAttackable) mRotating = false;
}

void Aura::onPacketOutEvent(PacketOutEvent& event)
{
    if (!mRotating) return;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.packet->getId() == PacketID::PlayerAuthInput) {
        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        glm::vec2 rots = MathUtils::getRots(*player->getPos(), mTargetedAABB);
        pkt->mRot = rots;
        pkt->mYHeadRot = rots.y;
    } else if (event.packet->getId() == PacketID::MovePlayer) {
        auto pkt = event.getPacket<MovePlayerPacket>();
        glm::vec2 rots = MathUtils::getRots(*player->getPos(), mTargetedAABB);
        pkt->mRot = rots;
        pkt->mYHeadRot = rots.y;
    }

}
