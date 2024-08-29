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
#include <SDK/Minecraft/Actor/SyncedPlayerMovementSettings.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/ModalFormResponsePacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
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
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    /*gFeatureManager->mDispatcher->listen<LookInputEvent, &TestModule::onLookInputEvent>(this);*/
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    /*gFeatureManager->mDispatcher->deafen<LookInputEvent, &TestModule::onLookInputEvent>(this);*/
}


Block* gDaBlock = nullptr;

int lastFormId = 0;
bool formOpen = false;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    /*if (formOpen && mMode.mValue == Mode::Mode3)
    {
        auto packet = MinecraftPackets::createPacket<ModalFormResponsePacket>();
        packet->mFormId = lastFormId;
        MinecraftJson::Value json;
        json.mType = MinecraftJson::ValueType::Int;
        json.mValue.mInt = 0;
        packet->mJSONResponse = json;
        ClientInstance::get()->getPacketSender()->send(packet.get());
    }*/

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
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    /*if (event.mPacket->getId() == PacketID::ModalFormResponse)
    {
        auto packet = event.getPacket<ModalFormResponsePacket>();
        formOpen = false;
        spdlog::info("ModalFormResponsePacket: FormId: {}, ValueType: {}", packet->mFormId, std::string(magic_enum::enum_name(packet->mJSONResponse->mType)));
    }*/
}

void TestModule::onPacketInEvent(PacketInEvent& event)
{
    /*if (event.mPacket->getId() == PacketID::ModalFormRequest)
    {
        auto packet = event.getPacket<ModalFormRequestPacket>();
        lastFormId = packet->mFormId;
        formOpen = true;
        if (mMode.mValue == Mode::Mode3) event.cancel();
    }*/
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
        // begin flag group
        ImGui::BeginGroup();
        ImGui::Text("isOnGround: %d", player->getFlag<OnGroundFlagComponent>());
        ImGui::Text("wasOnGround: %d", player->getFlag<WasOnGroundFlagComponent>());
        ImGui::Text("renderCameraFlag: %d", player->getFlag<RenderCameraComponent>());
        ImGui::Text("gameCameraFlag: %d", player->getFlag<GameCameraComponent>());
        ImGui::Text("cameraRenderPlayerModel: %d", player->getFlag<CameraRenderPlayerModelComponent>());
        ImGui::Text("isOnFire: %d", player->getFlag<OnFireComponent>());
        ImGui::Text("moveRequestComponent: %d", player->getFlag<MoveRequestComponent>());
        ImGui::EndGroup();

        //ImGui::Text("ActorMovementTickNeededFlag: %d", player->getFlag<ActorMovementTickNeededFlag>());

        ImGui::BeginGroup();
        ImGui::Text("gameType: %d", player->getGameType());
        displayCopyableAddress("LocalPlayer", player);
        displayCopyableAddress("PlayerInventory", player->getSupplies());
        displayCopyableAddress("GameMode", player->getGameMode());
        displayCopyableAddress("Level", player->getLevel());
        if (player->getLevel()) displayCopyableAddress("SyncedPlayerMovementSettings", player->getLevel()->getPlayerMovementSettings());
        if (player->getLevel()->getPlayerMovementSettings())
        {
            auto settings = player->getLevel()->getPlayerMovementSettings();
            std::string authority = std::string(magic_enum::enum_name(settings->AuthorityMode));
            int rewindHistorySize = static_cast<int>(settings->mRewindHistorySize);
            bool serverAuthBlockBreaking = settings->ServerAuthBlockBreaking;
            ImGui::Text("Authority Movement: %s", authority.c_str());
            ImGui::Text("Rewind History Size: %d", rewindHistorySize);
            ImGui::Text("Server Auth Block Breaking: %s", serverAuthBlockBreaking ? "true" : "false");
        }
        displayCopyableAddress("Item1", player->getSupplies()->getContainer()->getItem(0));
        displayCopyableAddress("Item2", player->getSupplies()->getContainer()->getItem(1));
        ImGui::Text("ItemAddress Diff: %d", reinterpret_cast<uintptr_t>(player->getSupplies()->getContainer()->getItem(1)) - reinterpret_cast<uintptr_t>(player->getSupplies()->getContainer()->getItem(0)));
        displayCopyableAddress("ContainerManagerModel", player->getContainerManagerModel());
        displayCopyableAddress("ActorWalkAnimationComponent", player->getWalkAnimationComponent());
        displayCopyableAddress("RawMoveInputComponent", player->mContext.getComponent<RawMoveInputComponent>());
        displayCopyableAddress("MobHurtTimeComponent", player->mContext.getComponent<MobHurtTimeComponent>());;
        displayCopyableAddress("AABBShapeComponent", player->mContext.try_get<AABBShapeComponent>());
        ImGui::EndGroup();
    }

    displayCopyableAddress("Options", ci->getOptions());
    displayCopyableAddress("BlockSource", blockSource);
    displayCopyableAddress("LevelRenderer", ci->getLevelRenderer());
    displayCopyableAddress("MinecraftGame", ci->getMinecraftGame());
    displayCopyableAddress("MinecraftSimulation", ci->getMinecraftSim());
    displayCopyableAddress("PacketSender", ci->getPacketSender());


    // Display a button that sets D3DHook::forceFallback to true
    if (ImGui::Button("Force Fallback"))
    {
        D3DHook::forceFallback = true;
    }


    FontHelper::popPrefFont();
    ImGui::End();

}