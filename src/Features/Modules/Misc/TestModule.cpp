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
#include <SDK/Minecraft/Network/Packets/MovePlayerPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <SDK/Minecraft/World/BlockSource.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/Chunk/LevelChunk.hpp>
#include <SDK/Minecraft/World/Chunk/SubChunkBlockStorage.hpp>

float lastOnGroundY = 0.f;
uint64_t lastLagback = 0;

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    /*gFeatureManager->mDispatcher->listen<LookInputEvent, &TestModule::onLookInputEvent>(this);*/
    auto player = ClientInstance::get()->getLocalPlayer();
    if (player)
    {
        lastOnGroundY = player->getPos()->y - 0.01;
    }
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

    if (mMode.mValue == Mode::VerticalTest)
    {
        if (BlockUtils::isOverVoid(*player->getPos()))
        {
            spdlog::info("Over void");
            return;
        }

        glm::vec3 pos = *player->getPos();
        player->getStateVectorComponent()->mVelocity.y = mClipDistance.mValue;

        return;
    }

    if (mMode.mValue != Mode::ClipTest) return;


    if (lastLagback + 200 > NOW)
    {
        spdlog::info("Lagback detected, avoiding desync");
        return;
    }

    bool onGround = player->isOnGround();
    if (!onGround && mOnGroundOnly.mValue) return;

    glm::vec3 pos = *player->getPos();

    auto blockSource = ClientInstance::get()->getBlockSource();
    glm::vec3 highestBlock = glm::floor(pos);
    bool blockFound = false;
    for (int y = 0; y < 256; y++)
    {
        auto block = blockSource->getBlock(pos.x, y, pos.z);
        if (block->toLegacy()->getBlockId() != 0)
        {
            highestBlock.y = y;
            blockFound = true;
        }
    }

    if (!blockFound)
    {
        spdlog::info("No block found");
        return;
    }

    // if the y is too far away, log it and return
    if (highestBlock.y - pos.y > mMaxDistance.mValue)
    {
        spdlog::info("highestBlock.y - pos.y: {} [too far]", highestBlock.y - pos.y);
        return;
    }

    spdlog::info("highestBlock: {}", highestBlock.y);

    glm::vec3 newPos = glm::vec3(pos.x, highestBlock.y + 1.01 + PLAYER_HEIGHT, pos.z);

    if (!BlockUtils::isAirBlock(newPos - PLAYER_HEIGHT_VEC))
    {
        spdlog::info("Clip failed: block at new pos");
    }

    player->setPosition(newPos);
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    if (mMode.mValue != Mode::OnGroundSpeedTest) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();

        bool isMoving = paip->mMove != glm::vec2(0, 0);
        static int tick = 0;

        if (isMoving)
        {
            if (mManuallyApplyFlags.mValue)
            {
                paip->addInputData(AuthInputAction::JUMPING);
                paip->addInputData(AuthInputAction::JUMP_DOWN);
                paip->addInputData(AuthInputAction::WANT_UP);
            }

            bool jumped = false;
            if (tick > 10)
            {
                jumped = true;
                if (mManuallyApplyFlags.mValue)
                    paip->addInputData(AuthInputAction::START_JUMPING);
                tick = 0;
            }

            if (mManuallyApplyFlags.mValue)
                spdlog::info("tick: {} jumped: {}", tick, jumped ? "true" : "false");
            tick++;

        } else tick = 0;
        float oldY = paip->mPos.y;
        paip->mPos.y = lastOnGroundY - 0.01;

        // Check if we are over the void
        if (BlockUtils::isOverVoid(paip->mPos))
        {
            paip->mPos.y = oldY;
        }
    }
}

void TestModule::onPacketInEvent(PacketInEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::MovePlayer)
    {
        auto packet = event.getPacket<MovePlayerPacket>();
        if (packet->mResetPosition == PositionMode::Teleport && packet->mPlayerID == player->getRuntimeID())
        {
            lastLagback = NOW;
        }
    }
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