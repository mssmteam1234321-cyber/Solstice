#pragma once
//
// Created by vastrakai on 6/28/2024.
//


#include <complex.h>
#include <vector>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>

class ActorUtils {
public:
    static std::vector<class Actor*> getActorList(bool playerOnly = true, bool excludeBots = true);
    static std::shared_ptr<InventoryTransactionPacket> createAttackTransaction(Actor* actor, int slot = -1);
    static Actor* getActorFromUniqueId(int64_t uniqueId);
    static Actor* getActorFromRuntimeID(int64_t runtimeId);
};
