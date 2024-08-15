#pragma once
//
// Created by vastrakai on 7/10/2024.
//
#include <Utils/MemUtils.hpp>
#include <SDK/Minecraft/mce.hpp>
#include <unordered_map>
#include <SDK/Minecraft/Network/Packets/PlayerListPacket.hpp>

class PlayerListEntry {
public:
    uint64_t id; // This is the ActorUniqueID
    mce::UUID uuid;
    std::string name, XUID, platformOnlineId;
    BuildPlatform buildPlatform;

    PlayerListEntry() {
        id = 0;
        uuid = mce::UUID();
        name = "";
        XUID = "";
        platformOnlineId = "";
        buildPlatform = BuildPlatform::Unknown;
    }
};


class Level {
public:
    CLASS_FIELD(uintptr_t**, mVfTable, 0x0);
    ///CLASS_FIELD(std::unordered_map<mce::UUID, PlayerListEntry>, pl, 0x1BC8); // class_field doesn't support std::unordered_map lol

    std::unordered_map<mce::UUID, PlayerListEntry>* getPlayerList();
    class HitResult* getHitResult();
    class SyncedPlayerMovementSettings* getPlayerMovementSettings();

};