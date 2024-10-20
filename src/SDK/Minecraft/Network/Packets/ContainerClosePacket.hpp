//
// Created by vastrakai on 7/13/2024.
//

#pragma once

#include "Packet.hpp"
#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>
#include <SDK/Minecraft/Inventory/NetworkItemStackDescriptor.hpp>


class InventoryContentPacket : public Packet
{
public:
    static const PacketID ID = PacketID::InventoryContent;

    ContainerID                             mInventoryId; // this+0x30
    std::vector<NetworkItemStackDescriptor> mSlots;       // this+0x38
};

class ContainerClosePacket : public ::Packet
{
public:
    static const PacketID ID = PacketID::ContainerClose;

    ContainerID mContainerId;          // this+0x30
    bool        mServerInitiatedClose; // this+0x31
};

class ContainerOpenPacket : public ::Packet
{
public:
    static const PacketID ID = PacketID::ContainerOpen;

    ContainerID          mContainerId;    // this+0x30
    ContainerType        mType;           // this+0x31
    glm::ivec3           mPos;            // this+0x34
    int64_t              mEntityUniqueID; // this+0x40
};