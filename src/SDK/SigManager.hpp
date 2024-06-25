#pragma once
//
// Created by vastrakai on 6/24/2024.
//

#include <cstdint>
#include <future>
#include <include/libhat/include/libhat.hpp>
#include <include/libhat/include/libhat/Scanner.hpp>
#include <include/libhat/include/libhat/Signature.hpp>

#include "spdlog/spdlog.h"


/*#define DEFINE(name, str) const hat::signature_view name = ([]() {  \
    static constexpr auto sig = hat::compile_signature<str>();      \
    return hat::signature_view{sig};                                \
})();*/

enum class SigType {
    Sig,
    RefSig
};

/*
#define DEFINE_SIG(name, str) \
static inline uintptr_t name; \
static void name##_initializer() { name = reinterpret_cast<uintptr_t>(scanSig(hat::compile_signature<str>(), #name).get()); } \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());

#define DEFINE_REF_SIG(name, str, offset) \
static inline uintptr_t name; \
static void name##_initializer() { name = reinterpret_cast<uintptr_t>(scanSig(hat::compile_signature<str>(), #name, offset).rel(static_cast<int>(offset))); } \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());*/

// example: DEFINE_SIG(Reach, "F3 0F ? ? ? ? ? ? 44 0F ? ? 76 ? C6 44 24 64", SigType::Sig, 0)
#define DEFINE_SIG(name, str, sig_type, offset) \
static inline uintptr_t name; \
static void name##_initializer() { \
    auto result = scanSig(hat::compile_signature<str>(), #name, offset); \
    if (!result.has_result()) { \
        name = 0; \
        return; \
    } \
    if (sig_type == SigType::Sig) { \
        name = reinterpret_cast<uintptr_t>(result.get()); \
    } else { \
        name = reinterpret_cast<uintptr_t>(result.rel(offset)); \
    } \
} \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());



class SigManager {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::future<void>> futures;
    static inline int mSigScanCount;
    static inline uint64_t mSigScanStart;
public:
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    // only here for testing scan speed lol
    /*DEFINE_SIG(Reach, "F3 0F ? ? ? ? ? ? 44 0F ? ? 76 ? C6 44 24 64");
    DEFINE_REF_SIG(MobEquipmentPacket_MobEquipmentPacket, "E8 ? ? ? ? 90 48 8B ? E8 ? ? ? ? 45 33 ? 4C 8D ? ? ? ? ? 48 8B ? 48 8B ? E8 ? ? ? ? 45 8D", 1); // MobEquipmentPacket::MobEquipmentPacket
    DEFINE_SIG(SplashTextRenderer_render, "48 8B ? 55 53 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 4D 8B ? 4D 8B ? 48 89"); // SplashTextRenderer::render
    DEFINE_REF_SIG(MinecraftPackets_createPacket, "E8 ? ? ? ? 90 48 83 BD ? ? 00 00 ? 0F 84 ? ? ? ? ff", 1); // MinecraftPackets::createPacket. alternative sig:
    DEFINE_SIG(inventory_dropSlot, "85 D2 0F 88 ? ? ? ? 48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? ? 48 81 EC"); // inventory::dropSlot
    DEFINE_SIG(RakNet_RakPeer_runUpdateCycle, "48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B ? 0F 29 ? ? ? ? ? ? 0F 29 ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 4C 8B ? 48 89 ? ? 4C 8B ? 48 89"); // RakNet::RakPeer::runUpdateCycle
    DEFINE_SIG(Gamemode_getDestroyRate, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 0F 29 ? ? ? 48 8B ? ? E8"); // Gamemode::getDestroyRate
    DEFINE_SIG(ActorRenderDispatcher_render, "48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 4D 8B ? 49 8B ? 48 8B ? 4C 8B ? 41 8B"); // ActorRenderDispatcher::render
    DEFINE_REF_SIG(ContainerScreenController_tick, "E8 ? ? ? ? 48 8B 8B ? ? ? ? 48 8D 93 ? ? ? ? 41 B8 ? ? ? ? 8B F8", 1); // ContainerScreenControllerHook
    DEFINE_SIG(Actor_shouldRenderNametagBp, "0F 84 ? ? ? ? 4C 8B ? ? 4D 3B ? ? ? ? ? ? ? 49 8B"); // Actor::shouldRenderNametagBp
    DEFINE_SIG(Actor_shouldRenderNametag, "0F 84 ? ? ? ? 49 8B ? 48 8B ? E8 ? ? ? ? 84 C0 ? ? ? ? ? ? 49 8D"); // Actor::shouldRenderNametag
    DEFINE_REF_SIG(renderCtxAddr, "E8 ? ? ? ? 48 8B 44 24 ? 48 8D 4C 24 ? 48 8B 80", 1); // Setupandrender
    DEFINE_REF_SIG(keymap, "E8 ? ? ? ? 33 D2 0F B6 CB", 1); // keymap
    DEFINE_SIG(CameraClipFunc, "40 ? 56 57 48 81 EC ? ? ? ? 44 0F"); // CameraClipFunc
    DEFINE_REF_SIG(Mouse, "E8 ? ? ? ? 40 88 6C 1F", 1); // Mouse
    DEFINE_REF_SIG(Gamemode_baseUseItem, "E8 ? ? ? ? 84 C0 74 ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 85", 1); // Gamemode::baseUseItem
    DEFINE_REF_SIG(Mob_getCurrentSwingDuration, "E8 ? ? ? ? 80 BB ? ? ? ? ? 74 ? 8b ? ? ? ? ? ff", 1); // Mob::getCurrentSwingDuration
    DEFINE_SIG(bobHurt, "40 53 56 48 83 EC 78 ? ? 7C"); // bobHurt thing
    DEFINE_SIG(fluxSwing, "E8 ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? 48 8B ? F3 0F ? ? 48 8B"); // flux swing
    DEFINE_REF_SIG(Mob_getJumpComponent, "E8 ? ? ? ? 48 85 C0 74 ? C6 40 ? ? 48 83 C4 ? 5B", 1); // Mob::getJumpComponent (alternative sig: "40 53 48 83 EC ? 48 8B DA BA 15 CF 00 41")
    DEFINE_REF_SIG(ItemStack_getCustomName, "E8 ? ? ? ? 48 8D ? ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? ? ? 48 83 FA ? 0F 82", 1); // ItemStack::getCustomName
    DEFINE_REF_SIG(ContainerScreenController_handleAutoPlace, "E8 ? ? ? ? 66 ? ? ? ? ? ? ? 0F 8C", 1); // ContainerScreenController::handleAutoPlace
    DEFINE_SIG(ItemUseSlowdownSystem_isSlowedByItemUse, "40 ? 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 48 8B ? 4D 85 ? 74 ? 8B 01 48 8D ? ? ? 49 8D ? ? 48 89")
    DEFINE_SIG(AppPlatform_getDeviceId, "48 89 ? ? ? 48 89 ? ? ? 56 57 41 ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 48 8B ? 48 89 ? ? ? 33 ED")
    DEFINE_SIG(ConnectionRequest_create, "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B D9 48 89 55")
    DEFINE_SIG(TapSwingAnim, "F3 44 ? ? ? ? ? ? ? 41 0F ? ? F3 0F ? ? ? ? C6 40 38 ? 48 8B ? ? 41 0F ? ? E8 ? ? ? ? 48 8B");
    DEFINE_REF_SIG(Actor_canSee, "E8 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? E9", 1);
    DEFINE_REF_SIG(Actor_getAttribute, "E8 ? ? ? ? F3 0F 10 B0 ? ? ? ? 48 8D 4D", 1);
    DEFINE_REF_SIG(Actor_getNameTag, "E8 ? ? ? ? 48 8B ? 48 8B ? 48 83 78 18 ? 48 8B", 1);
    DEFINE_SIG(Actor_setNameTag, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 8B ? 4C 8B");
    DEFINE_REF_SIG(Actor_setPosition, "E8 ? ? ? ? 48 8D ? ? 66 0F ? ? ? ? 48 8B ? 49 8B", 1);
    DEFINE_SIG(EnchantUtils_getEnchantLevel, "48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? 0F B6 ? 33 FF");
    DEFINE_SIG(RenderItemInHandDescription_ctor, "48 89 ? ? ? 48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 83 EC ? 4D 8B ? 4D 8B ? 4C 8B ? 48 8B ? 45 33");
    DEFINE_SIG(SimulatedPlayer_simulateJump, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 47");
    DEFINE_SIG(SneakMovementSystem_tickSneakMovementSystem, "32 C0 88 43 ? 84 C0");
    DEFINE_SIG(Player_applyTurnDelta, "48 8B ? 48 89 ? ? 48 89 ? ? 55 57 41 ? 41 ? 41 ? 48 8D ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 4C 8B");
    DEFINE_SIG(NoRenderParticles, "E8 ? ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? E8");
    DEFINE_SIG(LevelRendererPlayer_renderOutlineSelection, "48 8B ? 55 53 56 57 41 ? 41 ? 48 8D ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F");
    DEFINE_SIG(LevelRendererPlayer_renderOutlineSelectionOpcode, "0F 10 ? ? ? ? ? 0F 11 ? C6 40 10 ? 0F 57 ? 0F 11 ? ? 0F 11 ? ? 48 8D");
    DEFINE_REF_SIG(PlayerMovement_clearInputState, "E8 ? ? ? ? 48 89 ? ? 88 9F ? ? ? ? 48 8D", 1);
    DEFINE_SIG(ItemInHandRenderer_renderBp, "F3 0F ? ? EB ? 0F 57 ? F3 41");
    DEFINE_SIG(OverworldDimension_getBrightnessDependentFogColor, "41 0F 10 08 48 8B C2 0F");
    DEFINE_SIG(InventoryTransactionManager_addAction, "40 ? 55 41 ? 48 83 EC ? 48 8B ? 48 8B");
    DEFINE_REF_SIG(NetworkStackItemDescriptor_ctor, "E8 ? ? ? ? 48 8D ? ? 4C 3B ? 74 ? 48 8B ? ? 48 C7 45 8F", 1);
    DEFINE_REF_SIG(InventoryTransaction_addAction, "E8 ? ? ? ? 48 8B 17 48 8B 42 ? 48 8B ? ? 48 ? ? 74 ?", 1);
    DEFINE_SIG(ComplexInventoryTransaction_vtable, "48 8D 05 ? ? ? ? 48 89 01 8B 42 ? 89 41 ? 48 83 C1 ? 48 83 C2 ? E8 ? ? ? ? 90 48 8D 05");
    DEFINE_SIG(ItemUseInventoryTransaction_vtable, "48 8D 05 ? ? ? ? 48 89 07 8B 43 ? 89 47 ? 8B 53");
    DEFINE_SIG(ItemUseOnActorInventoryTransaction_vtable, "48 8D 05 ? ? ? ? 48 89 4B ? 0F 57 C0 48 89 4B ? 48 89 4B ? 48 8B 5C 24 ? 48 89 07 48 8D 05 ? ? ? ? 48 89 4F ? 89 4F");
    DEFINE_SIG(ItemReleaseInventoryTransaction_vtable, "48 8D ? ? ? ? ? 48 8B ? 48 89 ? 8B FA 48 83 C1 ? E8 ? ? ? ? 48 8D ? ? E8 ? ? ? ? 48 8D ? ? E8");
    DEFINE_SIG(ItemStack_EMPTY_ITEM, "48 8D 1D ? ? ? ? 48 8B D3 48 8D 4D ? E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 45 ? 89 B5");
    DEFINE_REF_SIG(ItemStack_vtable, "48 8D 05 ? ? ? ? 48 89 85 ? ? ? ? 89 BD ? ? ? ? C6 85 ? ? ? ? ? 48 8D 95", 3);
    DEFINE_REF_SIG(ItemStack_fromDescriptor, "E8 ? ? ? ? 90 F6 86 ? ? ? ? ?", 1);
    DEFINE_SIG(RakPeer_sendImmediate, "40 ? 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 48 8D ? ? ? 48 89 ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 48 8B");
    DEFINE_REF_SIG(ActorCollision_setOnGround, "E8 ? ? ? ? EB ? 0F B6 ? ? 48 8D ? ? E8", 1); // ActorCollision::setOnGround
    DEFINE_REF_SIG(ActorCollision_isOnGround, "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F3 0F ? ? F3 0F ? ? ? F3 0F", 1); // ActorCollision::isOnGround*/
    DEFINE_SIG(MainView_instance, "48 8B 05 ? ? ? ? C6 40 ? ? 0F 95 C0", SigType::RefSig, 3);
    DEFINE_SIG(GuiData_displayClientMessage, "40 ? 53 56 57 41 ? 48 8D ? ? ? ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 41 0F", SigType::Sig, 0);

    static void initialize();
};