//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/Chunk/LevelChunk.hpp>
#include <SDK/Minecraft/World/Chunk/SubChunkBlockStorage.hpp>

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    /*gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<LookInputEvent, &TestModule::onLookInputEvent>(this);*/
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    /*gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &TestModule::onLookInputEvent>(this);
    */

}

Block* gDaBlock = nullptr;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    //player->setFlag<ActorMovementTickNeededFlag>(true);

    gDaBlock = ClientInstance::get()->getBlockSource()->getBlock(*player->getPos());
    ItemStack* item = player->getSupplies()->getContainer()->getItem(player->getSupplies()->mSelectedSlot);
    if (!item->mItem) return;
    if (!item->mBlock) return;

    BlockUtils::setBlock(*player->getPos() - PLAYER_HEIGHT_VEC - glm::vec3(0, 1, 0), item->mBlock->getRuntimeId());
    spdlog::debug("Attempted to place block [rtid: {}] at ({}, {}, {})", item->mBlock->getRuntimeId(), player->getPos()->x, player->getPos()->y - 1, player->getPos()->z);
    return;




    /*
    std::unordered_map<mce::UUID, PlayerListEntry>* playerList = player->getLevel()->getPlayerList();
    std::vector<std::string> playerNames;
    for (auto& [uuid, entry] : *playerList)
    {
        playerNames.emplace_back(entry.name);
    }
    static std::vector<std::string> lastPlayerNames = playerNames;

    // Check if the player list has changed
    // Use ChatUtils::displayClientMessageRaw to display a message in the chat, depending on the player list
    // example: "§a§l» §r§7player has joined." or "§c§l» §r§7player has left."

    for (auto& playerName : playerNames)
    {
        if (std::ranges::find(lastPlayerNames, playerName) == lastPlayerNames.end())
        {
            ChatUtils::displayClientMessageRaw("§a§l» §r§7" + playerName + " has joined.");
        }
    }

    for (auto& playerName : lastPlayerNames)
    {
        if (std::ranges::find(playerNames, playerName) == playerNames.end())
        {
            ChatUtils::displayClientMessageRaw("§c§l» §r§7" + playerName + " has left.");
        }
    }

    // Store the last player list
    lastPlayerNames = playerNames;*/

    glm::ivec3 blockPos = *player->getPos();
    blockPos.x = std::floor(blockPos.x);
    blockPos.y = std::floor(blockPos.y);
    blockPos.z = std::floor(blockPos.z);

    auto blockSource = ClientInstance::get()->getBlockSource();
    ChunkBlockPos chunkBlockPos = ChunkBlockPos(BlockPos(blockPos));
    auto chunkPos = ChunkPos(BlockPos(blockPos));
    LevelChunk* chunk = blockSource->getChunk(chunkPos);
    if (!chunk) return;

    int blockFound = 0;

    for (auto subchunk : chunk->subChunks)
    {
        auto readah = subchunk.blockReadPtr;
        if (!readah) continue;
        uint16_t searchRangeXZ = 16;
        uint16_t searchRangeY = (blockSource->mBuildHeight - blockSource->mBuildDepth) / chunk->subChunks.size();
        for(uint16_t x = 0; x < searchRangeXZ; x++)
        {
            for(uint16_t z = 0; z < searchRangeXZ; z++)
            {
                for(uint16_t y = 0; y < searchRangeY; y++)
                {
                    uint16_t elementId = (x * 0x10 + z) * 0x10 + (y & 0xf);
                    const Block* found = readah->getElement(elementId);
                    if (found->mLegacy->getBlockId() == 0) continue;
                    BlockPos pos;
                    pos.x = (chunkPos.x * 16) + x;
                    pos.z = (chunkPos.y * 16) + z;
                    pos.y = y + (subchunk.subchunkIndex * 16);
                    //spdlog::debug("Block at ({}, {}, {}) is {}", pos.x, pos.y, pos.z, found->mLegacy->mName);
                    blockFound++;
                }
            }
        }
    }

    spdlog::debug("Found {} blocks at chunkpos ({}, {})", blockFound, chunkPos.x, chunkPos.y);
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
    }
}

void TestModule::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerList)
    {

    }
}

void displayCopyableAddress(const std::string& name, void* address)
{
    std::string addressHex = "0x" + fmt::format("{:X}", reinterpret_cast<uintptr_t>(address));
    ImGui::Text(addressHex.c_str());
    ImGui::SameLine();
    if (ImGui::Button(("Copy " + name + " address").c_str()))
    {
        ImGui::SetClipboardText(addressHex.c_str());
    }


}

void TestModule::onLookInputEvent(LookInputEvent& event)
{

}

struct NameableComponent
{
    bool mIsNameable;
};

#define show_flag(flag) ImGui::Text(#flag ": %d", player->getFlag<flag>())

void TestModule::onRenderEvent(RenderEvent& event)
{
    FontHelper::pushPrefFont(false, false);
    auto player = ClientInstance::get()->getLocalPlayer();

    ImGui::Begin("TestModule");
    ImGui::Text("TestModule");
    auto blockSource = ClientInstance::get()->getBlockSource();
    auto ci = ClientInstance::get();
    if (player)
    {
        ImGui::Text("isOnGround: %d", player->getFlag<OnGroundFlagComponent, false>());
        ImGui::Text("wasOnGround: %d", player->getFlag<WasOnGroundFlag>());
        ImGui::Text("isInWater: %d", player->getFlag<InWaterFlag>());
        ImGui::Text("renderCameraFlag: %d", player->getFlag<RenderCameraFlag>());
        ImGui::Text("gameCameraFlag: %d", player->getFlag<GameCameraFlag>());
        ImGui::Text("cameraRenderFirstPersonObjects: %d", player->getFlag<CameraRenderFirstPersonObjects>());
        ImGui::Text("cameraRenderPlayerModel: %d", player->getFlag<CameraRenderPlayerModel>());
        ImGui::Text("isOnFire: %d", player->getFlag<OnFireComponent, false>());
        ImGui::Text("moveRequestComponent: %d", player->getFlag<MoveRequestComponent, false>());

        //ImGui::Text("ActorMovementTickNeededFlag: %d", player->getFlag<ActorMovementTickNeededFlag>());

        show_flag(ActorMovementTickNeededFlag);

        ImGui::Text("gameType: %d", player->getGameType());
        displayCopyableAddress("held item", player->getSupplies()->getContainer()->getItem(player->getSupplies()->mSelectedSlot));
        displayCopyableAddress("getPlayerList", player->getLevel()->mVfTable[273]);
        displayCopyableAddress("LocalPlayer", player);
        displayCopyableAddress("supplies", player->getSupplies());
        displayCopyableAddress("DebugCamera", player->getDebugCameraComponent());
        displayCopyableAddress("ActorWalkAnimationComponent", player->getWalkAnimationComponent());
        displayCopyableAddress("RawMoveInputComponent", player->mContext.getComponent<RawMoveInputComponent>());
        displayCopyableAddress("MobHurtTimeComponent", player->mContext.getComponent<MobHurtTimeComponent>());
        displayCopyableAddress("NameableComponent", player->mContext.getComponent<NameableComponent>());
        displayCopyableAddress("gDaBlock", gDaBlock);
        if (gDaBlock)
        {
            auto rtid = gDaBlock->getRuntimeId();
            spdlog::debug("gDaBlock->getRuntimeId(): {}", rtid);
            ImGui::Text("gDaBlock->getRuntimeId(): %d", rtid);
        }

    }

    displayCopyableAddress("Options", ci->getOptions());
    displayCopyableAddress("BlockSource", blockSource);
    displayCopyableAddress("LevelRenderer", ci->getLevelRenderer());
    displayCopyableAddress("MinecraftGame", ci->getMinecraftGame());
    displayCopyableAddress("MinecraftSimulation", ci->getMinecraftSim());

    // Display a button that sets D3DHook::forceFallback to true
    if (ImGui::Button("Force Fallback"))
    {
        D3DHook::forceFallback = true;
    }


    FontHelper::popPrefFont();
    ImGui::End();

}