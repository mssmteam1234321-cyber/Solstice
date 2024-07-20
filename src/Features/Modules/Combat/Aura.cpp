//
// Created by vastrakai on 7/8/2024.
//

#include "Aura.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/BobHurtEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Features/Modules/Misc/Friends.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Utils/GameUtils/ActorUtils.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void Aura::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Aura::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Aura::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &Aura::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<BobHurtEvent, &Aura::onBobHurtEvent, nes::event_priority::FIRST>(this);
}

bool chargingBow = false;

void Aura::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Aura::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Aura::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &Aura::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<BobHurtEvent, &Aura::onBobHurtEvent>(this);
    mHasTarget = false;
    mRotating = false;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
}

void Aura::rotate(Actor* target)
{
    if (mRotateMode.mValue == RotateMode::None) return;
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    mTargetedAABB = target->getAABB();
    mRotating = true;
}

void Aura::shootBow(Actor* target)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (!mAutoBow.mValue) return;

    int bowSlot = -1;
    int arrowSlot = -1;
    for (int i = 0; i < 36; i++)
    {
        auto item = player->getSupplies()->getContainer()->getItem(i);
        if (!item->mItem) continue;
        if (item->getItem()->mName.contains("bow"))
        {
            bowSlot = i;
        }
        if (item->getItem()->mName.contains("arrow"))
        {
            arrowSlot = i;
        }
        if (bowSlot != -1 && arrowSlot != -1) break;
    }

    if (mHotbarOnly.mValue && bowSlot > 8) return;

    if (bowSlot == -1 || arrowSlot == -1) return;

    static int useTicks = 0;
    constexpr int maxUseTicks = 17;

    if (useTicks == 0)
    {
        spdlog::info("Starting to use bow");
        player->getSupplies()->getContainer()->startUsingItem(bowSlot);
        chargingBow = true;
        useTicks++;
    }
    else if (useTicks < maxUseTicks)
    {
        useTicks++;
    }
    else if (useTicks >= maxUseTicks)
    {
        spdlog::info("Releasing bow");
        player->getSupplies()->getContainer()->releaseUsingItem(bowSlot);
        chargingBow = false;
        useTicks = 0;
    }

}

void Aura::throwProjectiles(Actor* target)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    static uint64_t lastThrow = 0;
    int64_t throwDelay = mThrowDelay.mValue * 50.f;

    if (NOW - lastThrow < throwDelay) return;

    int snowballSlot = -1;
    if (mThrowProjectiles.mValue)
        for (int i = 0; i < 36; i++)
        {
            auto item = player->getSupplies()->getContainer()->getItem(i);
            if (!item->mItem) continue;
            if (item->getItem()->mName.contains("snowball"))
            {
                snowballSlot = i;
                break;
            }
        }

    if (mHotbarOnly.mValue && snowballSlot > 8) return;

    if (snowballSlot == -1)
    {
        return;
    }

    int oldSlot = player->getSupplies()->mSelectedSlot;
    player->getSupplies()->mSelectedSlot = snowballSlot;
    player->getGameMode()->baseUseItem(player->getSupplies()->getContainer()->getItem(snowballSlot));
    player->getSupplies()->mSelectedSlot = oldSlot;

    lastThrow = NOW;

}

void Aura::onRenderEvent(RenderEvent& event)
{
    if (mAPSMin.mValue < 0) mAPSMin.mValue = 0;
    if (mAPSMax.mValue < mAPSMin.mValue + 1) mAPSMax.mValue = mAPSMin.mValue + 1;

    if (mStrafe.mValue)
    {
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        if (mRotating)
        {
            glm::vec2 rots = MathUtils::getRots(*player->getPos(), mTargetedAABB);
            auto rot = player->getActorRotationComponent();
            rot->mPitch = rots.x;
            rot->mYaw = rots.y;
            rot->mOldPitch = rots.x;
            rot->mOldYaw = rots.y;

        }
    }
}

void Aura::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor; // Local player
    if (!player) return;
    auto supplies = player->getSupplies();

    static std::vector<std::shared_ptr<InventoryTransactionPacket>> queuedTransactions;

    for (auto& transaction : queuedTransactions)
    {
        player->getLevel()->getHitResult()->mType = HitType::ENTITY;
        ClientInstance::get()->getPacketSender()->send(transaction.get());
    }

    queuedTransactions.clear();

    auto actors = ActorUtils::getActorList(false, true);

    static std::unordered_map<Actor*, int64_t> lastAttacks = {};

    // Sort actors by lastAttack if mode is switch
    if (mMode.mValue == Mode::Switch)
    {
        std::ranges::sort(actors, [&](Actor* a, Actor* b) -> bool
        {
            return lastAttacks[a] < lastAttacks[b];
        });
    }
    else if (mMode.mValue == Mode::Multi)
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
        // Validate min and max APS
        if (mAPSMin.mValue < 0) mAPSMin.mValue = 0;
        if (mAPSMax.mValue < mAPSMin.mValue + 1) mAPSMax.mValue = mAPSMin.mValue + 1;

        aps = MathUtils::random(mAPSMin.mValue, mAPSMax.mValue);
    }
    int64_t delay = 1000 / aps;

    bool foundAttackable = false;

    for (auto actor : actors)
    {
        if (actor == player) continue;
        if (actor->distanceTo(player) > mRange.mValue) continue;

        if (actor->isPlayer() && gFriendManager->mEnabled)
        {
            if (gFriendManager->isFriend(actor))
            {
                continue;
            }
        }

        rotate(actor);
        foundAttackable = true;

        throwProjectiles(actor);
        shootBow(actor);

        if (now - lastAttack < delay) return;

        player->swing();
        int slot = -1;
        int bestWeapon = ItemUtils::getBestItem(SItemType::Sword);
        mHotbarOnly.mValue ? slot = bestWeapon : slot = player->getSupplies()->mSelectedSlot;

        actor = findObstructingActor(player, actor);

        if (mSwitchMode.mValue == SwitchMode::Full)
        {
            supplies->mSelectedSlot = bestWeapon;
        }

        if (mAttackMode.mValue == AttackMode::Synched)
        {
            std::shared_ptr<InventoryTransactionPacket> attackTransaction = ActorUtils::createAttackTransaction(actor, mSwitchMode.mValue == SwitchMode::Spoof ? bestWeapon : -1);
            queuedTransactions.push_back(attackTransaction);
        } else {
            int oldSlot = supplies->mSelectedSlot;
            if (mSwitchMode.mValue == SwitchMode::Spoof)
            {
                supplies->mSelectedSlot = bestWeapon;
            }
            player->getLevel()->getHitResult()->mType = HitType::ENTITY;
            player->getGameMode()->attack(actor);
            supplies->mSelectedSlot = oldSlot;
        }

        lastAttack = now;
        lastAttacks[actor] = now;
        if (mMode.mValue == Mode::Single || mMode.mValue == Mode::Switch) break;
    }

    if (!foundAttackable) mRotating = false;
    mHasTarget = foundAttackable;
}

void Aura::onPacketOutEvent(PacketOutEvent& event)
{
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

}

void Aura::onBobHurtEvent(BobHurtEvent& event)
{
    if (mHasTarget)
    {
        event.mDoBlockAnimation = true;
    }
}

Actor* Aura::findObstructingActor(Actor* player, Actor* target)
{
    auto actors = ActorUtils::getActorList(false, false);
    std::ranges::sort(actors, [&](Actor* a, Actor* b) -> bool
    {
        return a->distanceTo(player) < b->distanceTo(player);
    });

    for (auto actor : actors)
    {
        if (actor == player || actor == target) continue;
        float distance = actor->distanceTo(target);
        if (distance > 3.f) continue;

        std::string id = actor->mEntityIdentifier;
        if (id == "hivecommon:shadow" && distance < 1.5f && mAnticheatMode.mValue == AnticheatMode::FlareonV2)
        {
            return actor;
        }

        if (id == "minecraft:pig" && distance < 3.f && mAnticheatMode.mValue == AnticheatMode::FlareonV1)
        {
            return actor;
        }
    }

    return target;
}
