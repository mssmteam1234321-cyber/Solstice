//
// Created by vastrakai on 6/28/2024.
//

#include "TestModule.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/KeyboardMouseSettings.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/SyncedPlayerMovementSettings.hpp>
#include <SDK/Minecraft/Actor/Components/ItemUseSlowdownModifierComponent.hpp>
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
#include <SDK/Minecraft/Actor/Components/ComponentHashes.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/GameUtils/ItemUtils.hpp>
#include <Utils/MiscUtils/BlockUtils.hpp>
#include <Utils/MiscUtils/ImRenderUtils.hpp>

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
        clipPos = *player->getPos();
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
        auto currentPos = *player->getPos();
        clipPos.x = currentPos.x;
        clipPos.z = currentPos.z;
        player->setPosition(clipPos);
        hasSetClipPos = false;
        player->getStateVectorComponent()->mVelocity.y = -0.0784000015258789;
    }
}


Block* gDaBlock = nullptr;

int lastFormId = 0;
bool formOpen = false;
AABB lastBlockAABB = AABB();

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{
    if (!mWantedState) return;
    auto player = event.mActor;
    if (!player) return;

    if (mMode.mValue == Mode::TestingIdk)
    {
        // get the item value for the current item
        auto item = player->getSupplies()->getContainer()->getItem(player->getSupplies()->mSelectedSlot);
        if (item && item->mItem)
        {
            if (item->mItem)
            {
                int enchantVal = item->getEnchantValue(0);
                spdlog::info("Enchant Value: {}", enchantVal);

                auto enchants = item->gatherEnchants();
                for (auto& [id, lvl] : enchants)
                {
                    spdlog::info("Enchant: {} Level: {}", std::string(magic_enum::enum_name(static_cast<Enchant>(id))), lvl);
                }

            }
        }
        return;
    }

    /*
    auto mpp = MinecraftPackets::createPacket<MovePlayerPacket>();
    mpp->mPlayerID = player->getRuntimeID();
    mpp->mPos = *player->getPos();
    mpp->mResetPosition = PositionMode::Normal;
    auto actorRots = player->getActorRotationComponent();
    auto actorHeadRots = player->getActorHeadRotationComponent();
    mpp->mRot = { actorRots->mPitch, actorRots->mYaw };
    mpp->mYHeadRot = actorHeadRots->mHeadRot;
    mpp->mOnGround = true;
    mpp->mTick = 0;
    mpp->mCause = TeleportationCause::Unknown;
    mpp->mSourceEntityType = ActorType::Player;
    ClientInstance::get()->getPacketSender()->sendToServer(mpp.get());*/

    auto ballsSource = ClientInstance::get()->getBlockSource();
    gDaBlock = ballsSource->getBlock(glm::floor(*player->getPos() - PLAYER_HEIGHT_VEC));


    auto actorlist = ActorUtils::getActorList(false, false);

    if (mMode.mValue == Mode::DebugUi)
    {

        /*HitResult* hitResult = player->getLevel()->getHitResult();

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
        }*/

        HitResult result = ballsSource->checkRayTrace(*player->getPos(), glm::vec3(-4, 18, 0), player);
        spdlog::info("HitResult: Type: {}, Entity: {}, Block: {}/{}/{}", magic_enum::enum_name(result.mType).data(), result.mEntity.id, result.mBlockPos.x, result.mBlockPos.y, result.mBlockPos.z);

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
        if (player->getMoveInputComponent()->mIsJumping)
        {
            player->getStateVectorComponent()->mVelocity.y = mClipDistance.mValue;
        }
    }

    if (mMode.mValue != Mode::ClipTest && mMode.mValue != Mode::ClipVisualize && mMode.mValue != Mode::VerticalTest) return;


    if (mLastLagback + 200 > NOW)
    {
        spdlog::info("Lagback detected, avoiding desync");
        return;
    }

    bool onGround = player->isOnGround();
    if (!onGround && mOnGroundOnly.mValue && mMode.mValue == Mode::ClipTest) return;

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
        newPos.y -= 0.0099;
        clipPos = newPos;
        //gDaBlock = blockSource->getBlock(newPos.x, newPos.y, newPos.z);
        hasSetClipPos = true;
        return;
    }

    if (mMode.mValue == Mode::ClipVisualize)
    {
        // floor da x z
        newPos.x = glm::floor(newPos.x);
        newPos.z = glm::floor(newPos.z);
        newPos.y -= 0.0099;
        clipPos = newPos;
        //gDaBlock = blockSource->getBlock(newPos.x, newPos.y, newPos.z);
        hasSetClipPos = true;
        return;
    }

    player->setPosition(newPos);
    if (mDisableAfterClip.mValue) setEnabled(false);
}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::MovePlayer) event.cancel();

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
    /*AABB& blockAABB = lastBlockAABB;
    std::vector<glm::vec2> points = MathUtils::getBoxPoints(blockAABB);
    std::vector<ImVec2> imPoints = {};
    for (auto point : points)
    {
        imPoints.emplace_back(point.x, point.y);
    }

    ImColor color = ColorUtils::getThemedColor(0);
    auto drawList = ImGui::GetBackgroundDrawList();

    drawList->AddPolyline(imPoints.data(), imPoints.size(), color, 0, 2.0f);
    drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(color.Value.x, color.Value.y, color.Value.z, 0.25f));
    */


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

    if (ClientInstance* ci = ClientInstance::get())
    {
        ImGui::Text("ScreenName: %s", ci->getScreenName().c_str());

        if (ImGui::CollapsingHeader("ClientInstance"))
        {
            displayCopyableAddress("ClientInstance", ci);
            if (auto options = ci->getOptions())
            {
                displayCopyableAddress("Options", options);
                displayCopyableAddress("GfxGamma", options->mGfxGamma);
            }
            displayCopyableAddress("LevelRenderer", ci->getLevelRenderer());
            displayCopyableAddress("MinecraftGame", ci->getMinecraftGame());
            displayCopyableAddress("MinecraftSimulation", ci->getMinecraftSim());
            displayCopyableAddress("PacketSender", ci->getPacketSender());
        }

        if (auto blockSource = ci->getBlockSource(); blockSource && ImGui::CollapsingHeader("BlockSource"))
        {
            displayCopyableAddress("BlockSource", blockSource);
        }

        if (player)
        {
            if (ImGui::CollapsingHeader("Player"))
            {
                ImGui::Text("isOnGround: %d", player->getFlag<OnGroundFlagComponent>());
                ImGui::Text("wasOnGround: %d", player->getFlag<WasOnGroundFlagComponent>());
                ImGui::Text("renderCameraFlag: %d", player->getFlag<RenderCameraComponent>());
                ImGui::Text("gameCameraFlag: %d", player->getFlag<GameCameraComponent>());
                ImGui::Text("cameraRenderPlayerModel: %d", player->getFlag<CameraRenderPlayerModelComponent>());
                ImGui::Text("isOnFire: %d", player->getFlag<OnFireComponent>());
                ImGui::Text("moveRequestComponent: %d", player->getFlag<MoveRequestComponent>());
                ImGui::Text("gameType: %d", player->getGameType());
                displayCopyableAddress("LocalPlayer", player);

                if (auto supplies = player->getSupplies())
                {
                    displayCopyableAddress("PlayerInventory", supplies);
                    displayCopyableAddress("GameMode", player->getGameMode());
                    displayCopyableAddress("Level", player->getLevel());
                }

                if (ImGui::CollapsingHeader("Player Components"))
                {
                    for (auto& [ent, typehashes] : player->mContext.mRegistry->entities_with(static_cast<EntityId>(player->mContext.mEntityId)))
                    {
                        std::string check = "EntityId: " + std::to_string(static_cast<uint32_t>(ent));
                        for (auto typehash : typehashes)
                        {
                            std::string name = "Unknown";
                            if (Component::hashes.contains(typehash))
                            {
                                name = Component::hashes[typehash];
                            }
                            ImGui::Text("Entity: %d TypeHash: %s, Name: %s", ent, fmt::format("0x{:X}", typehash).c_str(), name.c_str());
                        }
                    }
                }

                if (ImGui::Button("Remove NameableComponent")) player->setFlag<NameableComponent>(false);
                if (ImGui::Button("Add NameableComponent")) player->setFlag<NameableComponent>(true);
            }

            if (ImGui::CollapsingHeader("Movement Settings"))
            {
                if (auto keyMouseSettings = ci->getKeyboardSettings())
                {
                    int forward = (*keyMouseSettings)["key.forward"];
                    int back = (*keyMouseSettings)["key.back"];
                    int left = (*keyMouseSettings)["key.left"];
                    int right = (*keyMouseSettings)["key.right"];

                    bool isForward = Keyboard::mPressedKeys[forward];
                    bool isBack = Keyboard::mPressedKeys[back];
                    bool isLeft = Keyboard::mPressedKeys[left];
                    bool isRight = Keyboard::mPressedKeys[right];

                    ImGui::Text("Forward: %s", isForward ? "true" : "false");
                    ImGui::Text("Back: %s", isBack ? "true" : "false");
                    ImGui::Text("Left: %s", isLeft ? "true" : "false");
                    ImGui::Text("Right: %s", isRight ? "true" : "false");
                }
            }

            if (auto supplies = player->getSupplies(); supplies && ImGui::CollapsingHeader("Inventory"))
            {
                int selectedSlot = supplies->mSelectedSlot;
                ImGui::Text("SelectedSlot: %d", selectedSlot);
                ItemStack* stack = supplies->getContainer()->getItem(selectedSlot);

                if (stack && stack->mItem)
                {
                    ImGui::Text("Item: %s", stack->getItem()->mName.c_str());
                    ImGui::Text("Item Value: %d", ItemUtils::getItemValue(stack));
                    ImGui::Text("Item Type: %s", magic_enum::enum_name(stack->getItem()->getItemType()).data());
                    ImGui::Text("Item Tier: %d", stack->getItem()->getItemTier());
                    ImGui::Text("Armor Slot: %d", stack->getItem()->getArmorSlot());
                    displayCopyableAddress("Item", stack->getItem());
                }

                displayCopyableAddress("Item1", supplies->getContainer()->getItem(0));
                displayCopyableAddress("Item2", supplies->getContainer()->getItem(1));
                ImGui::Text("ItemAddress Diff: %d", reinterpret_cast<uintptr_t>(supplies->getContainer()->getItem(1)) - reinterpret_cast<uintptr_t>(supplies->getContainer()->getItem(0)));
            }

            if (gDaBlock && ImGui::CollapsingHeader("Block"))
            {
                displayCopyableAddress("Block", gDaBlock);

                if (auto leg = gDaBlock->toLegacy())
                {
                    displayCopyableAddress("BlockLegacy", leg);
                    ImGui::Text("BlockName: %s", leg->mName.c_str());

                    if (auto mat = leg->mMaterial)
                    {
                        displayCopyableAddress("Material", mat);
                        ImGui::Text("Material Type: %d", mat->mType);
                        ImGui::Text("Material Flammable: %s", mat->mIsFlammable ? "true" : "false");
                        ImGui::Text("Material Never Buildable: %s", mat->mIsNeverBuildable ? "true" : "false");
                        ImGui::Text("Material Liquid: %s", mat->mIsLiquid ? "true" : "false");
                        ImGui::Text("Material Blocking Motion: %s", mat->mIsBlockingMotion ? "true" : "false");
                        ImGui::Text("Material Super Hot: %s", mat->mIsSuperHot ? "true" : "false");
                    }
                }
            }

            if (ImGui::CollapsingHeader("Components"))
            {
                displayCopyableAddress("MaxAutoStepComponent", player->getMaxAutoStepComponent());
                displayCopyableAddress("ActorWalkAnimationComponent", player->getWalkAnimationComponent());
                displayCopyableAddress("MoveInputComponent", player->mContext.getComponent<MoveInputComponent>());
                displayCopyableAddress("RawMoveInputComponent", player->mContext.getComponent<RawMoveInputComponent>());
                displayCopyableAddress("MobHurtTimeComponent", player->mContext.getComponent<MobHurtTimeComponent>());
                displayCopyableAddress("ShadowOffsetComponent", player->mContext.getComponent<ShadowOffsetComponent>());
                displayCopyableAddress("SubBBsComponent", player->mContext.getComponent<SubBBsComponent>());
                displayCopyableAddress("NameableComponent", player->mContext.getComponent<NameableComponent>());
                displayCopyableAddress("ActorStateVectorComponent", player->getStateVectorComponent());
                displayCopyableAddress("ItemUseSlowdownModifierComponent", player->mContext.getComponent<ItemUseSlowdownModifierComponent>());
            }

            if (auto settings = player->getLevel() ? player->getLevel()->getPlayerMovementSettings() : nullptr; settings && ImGui::CollapsingHeader("Player Movement Settings"))
            {
                std::string authority = std::string(magic_enum::enum_name(settings->AuthorityMode)) + " (" + std::to_string(static_cast<int>(settings->AuthorityMode)) + ")";
                ImGui::Text("Authority Movement: %s", authority.c_str());
                ImGui::Text("Rewind History Size: %d", static_cast<int>(settings->mRewindHistorySize));
                ImGui::Text("Server Auth Block Breaking: %s", settings->ServerAuthBlockBreaking ? "true" : "false");
                displayCopyableAddress("SyncedPlayerMovementSettings", settings);
            }
        }
    }

    if (ImGui::Button("Force Fallback"))
    {
        D3DHook::forceFallback = true;
    }

    FontHelper::popPrefFont();
    ImGui::End();


#endif


    /*if (ImGui::Button("Clear chunk storage"))
    {
        auto blockSource = ClientInstance::get()->getBlockSource();
        auto chunkSource = blockSource->mChunkSource;
        if (chunkSource)
        {
            // Expire each chunk
            for (auto& [pos, chunk] : chunkSource->getStorage())
            {

            }
        }
    }*/
}