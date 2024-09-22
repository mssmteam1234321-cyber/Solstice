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
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <SDK/Minecraft/World/Chunk/LevelChunk.hpp>
#include <SDK/Minecraft/World/Chunk/SubChunkBlockStorage.hpp>

glm::vec3 clipPos = glm::vec3(0, 0, 0);
bool hasSetClipPos = false;

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
        mLastOnGroundY = player->getPos()->y - 0.01;
    }
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);
    /*gFeatureManager->mDispatcher->deafen<LookInputEvent, &TestModule::onLookInputEvent>(this);*/

    auto player = ClientInstance::get()->getLocalPlayer();
    if (player && mMode.mValue == Mode::VerticalTest && hasSetClipPos)
    {
        player->setPosition(clipPos);
        hasSetClipPos = false;
        player->getStateVectorComponent()->mVelocity.y = -0.0784000015258789;
    }
}


Block* gDaBlock = nullptr;

int lastFormId = 0;
bool formOpen = false;

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    if (!mWantedState) return;
    auto player = event.mActor;
    if (!player) return;

    auto actorlist = ActorUtils::getActorList(false, false);

    if (mMode.mValue == Mode::DebugUi)
    {

        HitResult* hitResult = player->getLevel()->getHitResult();

        if(hitResult->mType == HitType::ENTITY)
        {
            Actor* actor = nullptr;
            for (auto a : actorlist)
            {
                if (a->mContext.mEntityId == hitResult->mEntity.id)
                {
                    actor = a;
                    break;
                }
            }

            if (actor)
            {
                spdlog::info("Hit actor: {}, RenderPositionComponent: {:X}", actor->getRawName(), reinterpret_cast<uintptr_t>(actor->getRenderPositionComponent()));
            }
        }

        return;
    }


    if (mMode.mValue == Mode::VerticalTest)
    {
        if (BlockUtils::isOverVoid(*player->getPos()))
        {
            player->setPosition(clipPos);
            spdlog::info("Over void");
            return;
        }

        glm::vec3 pos = *player->getPos();
        player->getStateVectorComponent()->mVelocity.y = mClipDistance.mValue;
    }

    if (mMode.mValue != Mode::ClipTest && mMode.mValue != Mode::ClipVisualize && mMode.mValue != Mode::VerticalTest) return;


    if (mLastLagback + 200 > NOW)
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
    if (mMode.mValue == Mode::ClipTest || mMode.mValue == Mode::ClipVisualize)
        for (int y = 0; y < 256; y++)
        {
            // floor x and z
            highestBlock.x = glm::floor(highestBlock.x);
            highestBlock.z = glm::floor(highestBlock.z);
            pos.x = glm::floor(pos.x);
            pos.z = glm::floor(pos.z);
            auto block = blockSource->getBlock(pos.x, y, pos.z);
            if (block->toLegacy()->getBlockId() != 0)
            {
                highestBlock.y = y;
                blockFound = true;
            }
        }
    else
    {
        // Get the highest block by going from the players current y down
        for (int y = pos.y; y >= 0; y--)
        {
            // floor x and z
            highestBlock.x = glm::floor(highestBlock.x);
            highestBlock.z = glm::floor(highestBlock.z);
            pos.x = glm::floor(pos.x);
            pos.z = glm::floor(pos.z);
            auto block = blockSource->getBlock(pos.x, y, pos.z);
            if (block->toLegacy()->getBlockId() != 0)
            {
                highestBlock.y = y;
                blockFound = true;
                break;
            }
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

    if (mMode.mValue == Mode::VerticalTest)
    {
        // floor da x z
        newPos.x = glm::floor(newPos.x);
        newPos.z = glm::floor(newPos.z);
        clipPos = newPos;
        gDaBlock = blockSource->getBlock(newPos.x, newPos.y, newPos.z);
        hasSetClipPos = true;
        return;
    }

    if (mMode.mValue == Mode::ClipVisualize)
    {
        // floor da x z
        newPos.x = glm::floor(newPos.x);
        newPos.z = glm::floor(newPos.z);
        clipPos = newPos;
        gDaBlock = blockSource->getBlock(newPos.x, newPos.y, newPos.z);
        hasSetClipPos = true;
        return;
    }

    player->setPosition(newPos);
    if (mDisableAfterClip.mValue) setEnabled(false);
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    if (mMode.mValue == Mode::VerticalTest)
    {
        if (event.mPacket->getId() == PacketID::PlayerAuthInput && mLastLagback + 100 < NOW && hasSetClipPos)
        {
            auto paip = event.getPacket<PlayerAuthInputPacket>();
            paip->mPos.y = clipPos.y;
            return;
        }
    }
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
        paip->mPos.y = mLastOnGroundY - 0.01;

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
            mLastLagback = NOW;
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

enum class Tab
{
    ClickGui,
    HudEditor,
    Scripting
};

void TestModule::onRenderEvent(RenderEvent& event)
{
    if (mMode.mValue == Mode::ClipVisualize || mMode.mValue == Mode::VerticalTest)
    {
        BlockInfo blockInfo = BlockInfo(gDaBlock, clipPos);
        AABB aabb = blockInfo.getAABB();

        std::vector<ImVec2> imPoints = MathUtils::getImBoxPoints(aabb);

        auto drawList = ImGui::GetBackgroundDrawList();

        ImColor themeColor = ColorUtils::getThemedColor(0);

        drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(themeColor.Value.x, themeColor.Value.y, themeColor.Value.z, 0.25f));
        drawList->AddPolyline(imPoints.data(), imPoints.size(), themeColor, 0, 2.0f);
        return;
    }
    auto player = ClientInstance::get()->getLocalPlayer();

#ifdef __DEBUG__
if (mMode.mValue == Mode::Concepts)
{
    FontHelper::pushPrefFont(false, false);

    // Render tabs at the top
    auto drawList = ImGui::GetBackgroundDrawList();
    std::vector<std::pair<Tab, std::string>> tabs = {
        {Tab::ClickGui, "ClickGui"},
        {Tab::HudEditor, "HudEditor"},
        {Tab::Scripting, "Scripting"}
    };

    static Tab selectedTab = Tab::ClickGui;
    float paddingBetween = 20.f;  // Increased padding for a better look
    float fontSize = 25.f;
    static ImVec2 underlinePos = ImVec2(0, 0);
    static ImVec2 underlineSize = ImVec2(0, 0);

    // Calculate the text sizes for each tab
    std::map<Tab, ImVec2> tabTextSizes;
    for (auto& tab : tabs)
    {
        tabTextSizes[tab.first] = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0, tab.second.c_str());
    }

    // Calculate total width of all tabs + padding
    auto windowSize = ImGui::GetIO().DisplaySize;
    float totalWidth = paddingBetween * (tabs.size() - 1);
    for (auto& tab : tabs)
    {
        totalWidth += tabTextSizes[tab.first].x;
    }

    // Calculate the starting X position to center tabs horizontally
    float x = (windowSize.x - totalWidth) / 2;
    float y = 10;

    // Render background behind the tabs
    ImVec4 bg = ImVec4(x - paddingBetween, y, x + totalWidth, y + tabTextSizes[selectedTab].y + 5);
    drawList->AddRectFilled(ImVec2(bg.x, bg.y), ImVec2(bg.z, bg.w), IM_COL32(30, 30, 30, 180), 5.f);

    std::map<Tab, ImVec2> tabPositions;

    for (auto& tab : tabs)
    {
        ImVec2 textSize = tabTextSizes[tab.first];
        ImVec2 textPos = ImVec2(x, y);

        // Center text vertically
        textPos.y += (bg.w - bg.y - textSize.y) / 2;

        // Center text horizontally by adjusting x for the tab's text size
        float centeredX = x + (textSize.x / 2) - (tabTextSizes[tab.first].x / 2);

        // Update position to center horizontally
        textPos.x = centeredX;

        // Check if the mouse is hovering over the tab
        if (ImRenderUtils::isMouseOver(ImVec4(x, y, x + textSize.x, y + textSize.y)))
        {
            if (ImGui::IsMouseClicked(0))
            {
                selectedTab = tab.first;
            }
        }

        tabPositions[tab.first] = textPos;

        // Render the tab text
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), tab.second.c_str());

        // Move to the next tab position
        x += textSize.x + paddingBetween;
    }

    // Animate the underline position and size
    ImVec2 underlineTargetPos = ImVec2(tabPositions[selectedTab].x, tabPositions[selectedTab].y + tabTextSizes[selectedTab].y);
    underlinePos = MathUtils::lerp(underlinePos, underlineTargetPos, ImGui::GetIO().DeltaTime * 10);

    ImVec2 underlineTargetSize = ImVec2(tabTextSizes[selectedTab].x, 2);
    underlineSize = MathUtils::lerp(underlineSize, underlineTargetSize, ImGui::GetIO().DeltaTime * 10);

    // Render the underline
    drawList->AddLine(underlinePos, ImVec2(underlinePos.x + underlineSize.x, underlinePos.y), IM_COL32(255, 255, 255, 255), underlineSize.y);

    FontHelper::popPrefFont();
    return;
}
#endif


#ifdef __DEBUG__
    if (mMode.mValue != Mode::DebugUi) return;

    FontHelper::pushPrefFont(false, false);

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

        int item = player->getSupplies()->mSelectedSlot;
        ImGui::Text("SelectedSlot: %d", item);
        ItemStack* stack = player->getSupplies()->getContainer()->getItem(item);
        if (stack && stack->mItem)
        {
            ImGui::Text("Item: %s", stack->getItem()->mName.c_str());
            int itemVal = ItemUtils::getItemValue(stack);
            ImGui::Text("Item Value: %d", itemVal);
            ImGui::Text("Item Type: %s", magic_enum::enum_name(stack->getItem()->getItemType()).data());
            ImGui::Text("Item Tier: %d", stack->getItem()->getItemTier());
            ImGui::Text("Armor Slot: %d", stack->getItem()->getArmorSlot());
            displayCopyableAddress("Item", stack->getItem());
        }

        displayCopyableAddress("ClientInstance", ci);
        displayCopyableAddress("GfxGamma", ci->getOptions()->mGfxGamma);
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
        displayCopyableAddress("MoveInputComponent", player->mContext.getComponent<MoveInputComponent>());
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

#endif

}