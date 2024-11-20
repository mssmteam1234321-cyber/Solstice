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
    uint64_t mId; // This is the ActorUniqueID
    mce::UUID mUuid;
    std::string mName, mXUID, mPlatformOnlineId;
    BuildPlatform mBuildPlatform;

    PlayerListEntry() {
        mId = 0;
        mUuid = mce::UUID();
        mName = "";
        mXUID = "";
        mPlatformOnlineId = "";
        mBuildPlatform = BuildPlatform::Unknown;
    }
};

class LevelData
{
public:
    CLASS_FIELD(uint64_t, mTick, OffsetProvider::LevelData_mTick);
};


class Level {
public:
    CLASS_FIELD(uintptr_t**, mVfTable, 0x0);
    //CLASS_FIELD(class BlockPalette*, mBlockPalette, 0x1C8);
    ///CLASS_FIELD(std::unordered_map<mce::UUID, PlayerListEntry>, pl, 0x1BC8); // class_field doesn't support std::unordered_map lol

    std::unordered_map<mce::UUID, PlayerListEntry>* getPlayerList();
    class HitResult* getHitResult();
    class SyncedPlayerMovementSettings* getPlayerMovementSettings();
    std::vector<Actor*> getRuntimeActorList();
    LevelData* getLevelData();
    class BlockPalette* getBlockPalette();
};