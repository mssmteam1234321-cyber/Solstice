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
#include <SDK/Minecraft/Actor/Components/ActorOwnerComponent.hpp>
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

void TestModule::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    for (auto&& [id, cameraComponent] : player->mContext.mRegistry->view<CameraComponent>().each())
    {
        auto* camera = player->mContext.mRegistry->try_get<DebugCameraComponent>(id);
        if (!camera)
        {
            player->mContext.mRegistry->set_flag<CurrentInputCameraComponent>(id, false);
            continue;
        }

        player->mContext.mRegistry->set_flag<CurrentInputCameraComponent>(id, true);
        player->mContext.mRegistry->set_flag<RenderCameraComponent>(id, true);

        spdlog::info("CameraComponent: {:X}, DebugCameraComponent: {:X}", reinterpret_cast<uintptr_t>(&cameraComponent), reinterpret_cast<uintptr_t>(camera));
    }
}

void TestModule::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &TestModule::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<RenderEvent, &TestModule::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &TestModule::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &TestModule::onPacketOutEvent>(this);

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    for (auto&& [id, cameraComponent] : player->mContext.mRegistry->view<CameraComponent>().each())
    {
        auto* camera = player->mContext.mRegistry->try_get<DebugCameraComponent>(id);
        if (!camera)
        {
            if (player->mContext.mRegistry->has_flag<ActiveCameraComponent>(id))
                player->mContext.mRegistry->set_flag<CurrentInputCameraComponent>(id, true);
            continue;
        }

        player->mContext.mRegistry->set_flag<CurrentInputCameraComponent>(id, false);
        player->mContext.mRegistry->set_flag<RenderCameraComponent>(id, false);
    }
}


Block* gDaBlock = nullptr;

int lastFormId = 0;
bool formOpen = false;
AABB lastBlockAABB = AABB();

void TestModule::onBaseTickEvent(BaseTickEvent& event)
{

}

void TestModule::onPacketOutEvent(PacketOutEvent& event)
{

}

void TestModule::onPacketInEvent(PacketInEvent& event)
{

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
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
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

                auto lpid = player->mContext.mEntityId;

                if (ImGui::CollapsingHeader("All EntityId Components"))
                {
                    static bool showTypeHashes = false;
                    ImGui::Checkbox("Show TypeHashes", &showTypeHashes);

                    std::unordered_map<EntityId, std::map<std::uint32_t, void*>> unsortedMap = player->mContext.mRegistry->get_all_entity_components();

                    for (auto& [ent, typehashes] : unsortedMap)
                    {
                        bool isActor = player->mContext.mRegistry->has_flag<ActorOwnerComponent>(ent);
                        if (isActor && lpid != ent)
                        {
                            unsortedMap.erase(ent);
                            continue;
                        }
                    }

                    std::unordered_map<EntityId, std::map<std::uint32_t, void*>> flagMap = {};

                    // For each entity in unsortedMap, if they have a 0x0 address component in their map, add it to the flagMap and remove it from the unsortedMap
                    for (auto& [ent, typehashes] : unsortedMap)
                    {
                        if (typehashes.empty()) continue;

                        std::map<std::uint32_t, void*> flagComponents = {};
                        for (auto& [typehash, address] : typehashes)
                        {
                            if (address == nullptr)
                            {
                                flagComponents[typehash] = nullptr;
                            }
                        }

                        if (!flagComponents.empty())
                        {
                            flagMap[ent] = flagComponents;
                            for (auto& [typehash, address] : flagComponents)
                            {
                                typehashes.erase(typehash);
                            }
                        }
                    }

                    static std::map<std::uint32_t, int> componentSizes;
                    static bool dumpedSizes = false;

                    if (!dumpedSizes)
                    {
                        // Can be used for dumping component sizes by looking at pseudocode
                        /*for (auto& [typeHash, componentName] : Component::hashes)
                        {
                            entt::basic_storage<void*, EntityId>* storage = player->mContext.assureWithHash(typeHash);
                            if (!storage) continue;
                            int index = 1;

                            void** vtable = *reinterpret_cast<void***>(storage);
                            uintptr_t func = reinterpret_cast<uintptr_t>(vtable[index]);

                            uintptr_t funcEnd = func;
                            // Find the end of the function by looking for a CC or C3 opcode
                            while (*reinterpret_cast<uint8_t*>(funcEnd) != 0xCC && *reinterpret_cast<uint8_t*>(funcEnd) != 0xC3)
                            {
                                funcEnd++;
                                // if we have exceeded 1K bytes, break
                                if (funcEnd - func > 1024)
                                {
                                    funcEnd = 0;
                                    break;
                                }
                            }

                            if (funcEnd == 0)
                            {
                                spdlog::error("Failed to find end of function for component: {}", componentName);
                                continue;
                            }

                            spdlog::info("TypeHash: 0x{:X}, Name: {}, VTable: {:X} Func: {:X}", typeHash, componentName, reinterpret_cast<uintptr_t>(vtable), func);
                        }*/

                        dumpedSizes = true;
                    }


                    auto local = player->mContext.mEntityId;

                    static std::map<EntityId, std::set<std::uint32_t>> entityComponentMap = {};
                    static std::map<EntityId, std::set<std::uint32_t>> entityFlagMap = {};

                    for (auto& [ent, typehashes] : flagMap)
                    {
                        if (typehashes.empty()) continue;

                        std::string id = "Flags for EntityId: " + std::to_string(static_cast<uint32_t>(ent)) + (ent == local ? " (LocalPlayer)" : "");
                        if (ImGui::CollapsingHeader(id.c_str()))
                        {
                            auto componentSet = std::set<std::uint32_t>();

                            for (auto& [typehash, address] : typehashes)
                            {
                                componentSet.insert(typehash);
                                std::string name = "Unknown";
                                if (Component::hashes.contains(typehash))
                                {
                                    name = Component::hashes[typehash];
                                }
                                /*ImGui::Text("TypeHash: %s, Name: %s, State: TRUE",
                                    fmt::format("0x{:X}", typehash).c_str(), name.c_str());*/
                                if (showTypeHashes)
                                    ImGui::Text("TypeHash: %s, Name: %s, State: TRUE", fmt::format("0x{:X}", typehash).c_str(), name.c_str());
                                else
                                    ImGui::Text("Name: %s, State: TRUE", name.c_str());
                            }

                            for (auto& typehash : entityFlagMap[ent])
                            {
                                std::string name = Component::hashes.contains(typehash) ? Component::hashes[typehash] : "Unknown";

                                if (!componentSet.contains(typehash))
                                {
                                    if (showTypeHashes)
                                        ImGui::Text("TypeHash: %s, Name: %s, State: FALSE", fmt::format("0x{:X}", typehash).c_str(), name.c_str());
                                    else
                                        ImGui::Text("Name: %s, State: FALSE", name.c_str());
                                }
                            }

                            for (auto& [typehash, address] : typehashes)
                            {
                                entityFlagMap[ent].insert(typehash);
                            }
                        }
                    }

                    for (auto& [ent, typehashes] : unsortedMap)
                    {
                        if (typehashes.empty()) continue;

                        std::string id = "Components for EntityId: " + std::to_string(static_cast<uint32_t>(ent)) + (ent == local ? " (LocalPlayer)" : "");
                        if (ImGui::CollapsingHeader(id.c_str()))
                        {
                            auto componentSet = std::set<std::uint32_t>();

                            for (auto& [typehash, address] : typehashes)
                            {
                                componentSet.insert(typehash);
                                std::string name = "Unknown";
                                if (Component::hashes.contains(typehash))
                                {
                                    name = Component::hashes[typehash];
                                }
                                if (showTypeHashes)
                                    ImGui::Text("TypeHash: %s, Name: %s, Address: %s",
                                       fmt::format("0x{:X}", typehash).c_str(), name.c_str(), fmt::format("0x{:X}", reinterpret_cast<uintptr_t>(address)).c_str());
                                else
                                    ImGui::Text("Name: %s, Address: %s", name.c_str(), fmt::format("0x{:X}", reinterpret_cast<uintptr_t>(address)).c_str());
                            }

                            for (auto& typehash : entityComponentMap[ent])
                            {
                                std::string name = Component::hashes.contains(typehash) ? Component::hashes[typehash] : "Unknown";

                                if (!componentSet.contains(typehash))
                                {
                                    if (showTypeHashes)
                                        ImGui::Text("TypeHash: %s, Name: %s, Address: %s",
                                            fmt::format("0x{:X}", typehash).c_str(), name.c_str(), "NULL");
                                    else
                                       ImGui::Text("Name: %s, Address: %s", name.c_str(), "NULL");
                                }
                            }

                            for (auto& [typehash, address] : typehashes)
                            {
                                entityComponentMap[ent].insert(typehash);
                            }
                        }
                    }
                }
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
}