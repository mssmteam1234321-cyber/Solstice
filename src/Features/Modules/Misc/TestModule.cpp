//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/Level.hpp>

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<LookInputEvent, &TestModule::onLookInputEvent>(this);
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &TestModule::onLookInputEvent>(this);

}

Block* gDaBlock = nullptr;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;


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
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    /*player->setFlag<CameraRenderFirstPersonObjects>(false);
    player->setFlag<CameraRenderPlayerModel>(true);*/

    auto firstPersonCamera = event.mFirstPersonCamera;
    auto thirdPersonCamera = event.mThirdPersonCamera;
    auto thirdPersonFrontCamera = event.mThirdPersonFrontCamera;

    glm::vec2 radRot = event.mCameraDirectLookComponent->mRotRads;
    int perspectiveOption = ClientInstance::get()->getOptions()->game_thirdperson->value;

    /*if (perspectiveOption == 0)
    {
        glm::vec3 origin = firstPersonCamera->mOrigin;
        float distance = mNumber.mValue;

        // Flip the pitch
        radRot.y = -radRot.y;

        // Calculate direction vector from yaw and pitch
        float cosYaw = cos(radRot.x);
        float sinYaw = sin(radRot.x);
        float cosPitch = cos(radRot.y);
        float sinPitch = sin(radRot.y);

        // Calculate the new position using spherical coordinates
        glm::vec3 offset;
        offset.x = distance * cosPitch * sinYaw;
        offset.y = distance * sinPitch;
        offset.z = distance * cosPitch * cosYaw;

        // Calculate the new origin by subtracting the offset from the original position
        glm::vec3 newOrigin = origin + offset;

        firstPersonCamera->mOrigin = newOrigin;
    } else if (perspectiveOption == 1)
    {
        glm::vec3 origin = thirdPersonCamera->mOrigin;
        float distance = mNumber.mValue;

        // Flip the pitch
        radRot.y = -radRot.y;

        // Calculate direction vector from yaw and pitch
        float cosYaw = cos(radRot.x);
        float sinYaw = sin(radRot.x);
        float cosPitch = cos(radRot.y);
        float sinPitch = sin(radRot.y);

        // Calculate the new position using spherical coordinates
        glm::vec3 offset;
        offset.x = distance * cosPitch * sinYaw;
        offset.y = distance * sinPitch;
        offset.z = distance * cosPitch * cosYaw;

        // Calculate the new origin by subtracting the offset from the original position
        glm::vec3 newOrigin = origin + offset;

        thirdPersonCamera->mOrigin = newOrigin;
    } else if (perspectiveOption == 2)
    {
        glm::vec3 origin = thirdPersonFrontCamera->mOrigin;
        float distance = mNumber.mValue;

        // Flip the pitch
        radRot.y = -radRot.y;

        // Calculate direction vector from yaw and pitch
        float cosYaw = cos(radRot.x);
        float sinYaw = sin(radRot.x);
        float cosPitch = cos(radRot.y);
        float sinPitch = sin(radRot.y);

        // Calculate the new position using spherical coordinates
        glm::vec3 offset;
        offset.x = distance * cosPitch * sinYaw;
        offset.y = distance * sinPitch;
        offset.z = distance * cosPitch * cosYaw;

        // Calculate the new origin by subtracting the offset from the original position
        glm::vec3 newOrigin = origin + offset;

        thirdPersonFrontCamera->mOrigin = newOrigin;
    }*/

    spdlog::info("firstPersonCamera: 0x{:X}", reinterpret_cast<uintptr_t>(firstPersonCamera));
    spdlog::info("thirdPersonCamera: 0x{:X}", reinterpret_cast<uintptr_t>(thirdPersonCamera));
    spdlog::info("thirdPersonFrontCamera: 0x{:X}", reinterpret_cast<uintptr_t>(thirdPersonFrontCamera));


}

void TestModule::onRenderEvent(RenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    ImGui::Begin("TestModule");
    ImGui::Text("TestModule");
    auto blockSource = ClientInstance::get()->getBlockSource();
    if (player)
    {
        ImGui::Text("isOnGround: %d", player->getFlag<OnGroundFlagComponent, false>());
        ImGui::Text("wasOnGround: %d", player->getFlag<WasOnGroundFlag>());
        ImGui::Text("isInWater: %d", player->getFlag<InWaterFlag>());
        ImGui::Text("renderCameraFlag: %d", player->getFlag<RenderCameraFlag>());
        ImGui::Text("gameCameraFlag: %d", player->getFlag<GameCameraFlag>());
        ImGui::Text("cameraRenderFirstPersonObjects: %d", player->getFlag<CameraRenderFirstPersonObjects>());
        ImGui::Text("cameraRenderPlayerModel: %d", player->getFlag<CameraRenderPlayerModel>());
        ImGui::Text("gameType: %d", player->getGameType());
        displayCopyableAddress("getPlayerList", player->getLevel()->mVfTable[273]);
        displayCopyableAddress("LocalPlayer", player);
        displayCopyableAddress("supplies", player->getSupplies());
        displayCopyableAddress("DebugCamera", player->getDebugCameraComponent());
        displayCopyableAddress("ActorWalkAnimationComponent", player->getWalkAnimationComponent());
    }

    // Display a button that sets D3DHook::forceFallback to true
    if (ImGui::Button("Force Fallback"))
    {
        D3DHook::forceFallback = true;
    }


    ImGui::End();
}