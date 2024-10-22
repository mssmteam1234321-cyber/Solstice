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

enum class SigType {
    Sig,
    RefSig
};

#define DEFINE_SIG(name, str, sig_type, offset) \
public: \
static inline uintptr_t name; \
private: \
static void name##_initializer() { \
    auto result = scanSig(hat::compile_signature<str>(), #name, offset); \
    if (!result.has_result()) { \
        name = 0; \
        return; \
    } \
    if (sig_type == SigType::Sig) name = reinterpret_cast<uintptr_t>(result.get()); \
    else name = reinterpret_cast<uintptr_t>(result.rel(offset)); \
} \
static inline std::function<void()> name##_function = (mSigInitializers.emplace_back(name##_initializer), std::function<void()>()); \
public:



class SigManager {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::function<void()>> mSigInitializers;
    static inline int mSigScanCount;
public:
    static inline bool mIsInitialized = false;
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    DEFINE_SIG(Actor_setPosition, "E8 ? ? ? ? 48 8B ? 48 8B ? E8 ? ? ? ? 48 8B ? 33 D2 48 8B ? 48 8B ? ? FF 15 ? ? ? ? EB", SigType::RefSig, 1);
    DEFINE_SIG(Actor_getNameTag, "E8 ? ? ? ? 48 8B ? 48 8B ? 48 83 78 18 ? 48 8B", SigType::RefSig, 1);
    DEFINE_SIG(Actor_setNameTag, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 8B ? 4C 8B", SigType::Sig, 0);
    DEFINE_SIG(ActorRenderDispatcher_render, "E8 ? ? ? ? 44 0F 28 54 24 ? 4C 8D 9C 24", SigType::RefSig, 1);
    DEFINE_SIG(ClientInstance_mBgfx, "48 8B ? ? ? ? ? 66 44 ? ? 75 ? 0F B7 ? ? ? 4C 8B", SigType::RefSig, 3);
    DEFINE_SIG(ClientInstance_grabMouse, "40 ? 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 40", SigType::Sig, 0);
    DEFINE_SIG(ClientInstance_releaseMouse, "40 ? 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 48 89", SigType::Sig, 0);
    DEFINE_SIG(ContainerScreenController_tick, "E8 ? ? ? ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? 41 B8 ? ? ? ? 8B F8", SigType::RefSig, 1);
    DEFINE_SIG(ContainerScreenController_handleAutoPlace, "E8 ? ? ? ? 66 ? ? ? ? ? ? ? 0F 8C", SigType::RefSig, 1);
    DEFINE_SIG(ComplexInventoryTransaction_vtable, "48 8D ? ? ? ? ? 48 89 ? ? ? 48 89 ? 48 8B ? 48 8B ? E8 ? ? ? ? 33 C9 48 8D ? ? ? ? ? 48 89 ? ? 0F 57 ? 48 89 ? ? 48 89 ? ? 48 8B ? ? ? 48 89 ? 48 8D ? ? ? ? ? 48 89", SigType::RefSig, 3);
    DEFINE_SIG(EnchantUtils_getEnchantLevel, "48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? 0F B6 ? 33 FF", SigType::Sig, 0);
    DEFINE_SIG(GameMode_getDestroyRate, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 0F 29 ? ? ? 48 8B ? ? E8", SigType::Sig, 0);
    DEFINE_SIG(GameMode_baseUseItem, "E8 ? ? ? ? 84 C0 74 ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 85", SigType::RefSig, 1);
    DEFINE_SIG(GuiData_displayClientMessage, "40 ? 53 56 57 41 ? 41 ? 48 8D ? ? ? ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 45 0F ? ? 49 8B", SigType::Sig, 0);
    DEFINE_SIG(InventoryTransaction_addAction, "E8 ? ? ? ? 48 81 C3 ? ? ? ? 48 3B ? 75 ? BE", SigType::RefSig, 1);
    DEFINE_SIG(ItemStack_vTable, "48 8D 05 ? ? ? ? 48 89 05 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C6 05 ? ? ? ? ? 48 8D 0D", SigType::RefSig, 3);
    DEFINE_SIG(ItemStack_getCustomName, "E8 ? ? ? ? 48 8B ? 48 8D ? ? ? 48 8B ? E8 ? ? ? ? 90 4C 8B ? 4C 8B ? 0F B6", SigType::RefSig, 1);
    DEFINE_SIG(ItemUseInventoryTransaction_vtable, "48 8D ? ? ? ? ? 48 8B ? 48 89 ? ? 48 81 C1 ? ? ? ? E8 ? ? ? ? 48 8D ? ? ? ? ? 48 8D", SigType::RefSig, 3);
    DEFINE_SIG(ItemUseOnActorInventoryTransaction_vtable, "48 8D 05 ? ? ? ? 48 89 4B ? 0F 57 C0 48 89 4B ? 48 89 4B ? 48 8B 5C 24 ? 48 89 07 48 8D 05 ? ? ? ? 48 89 4F ? 89 4F", SigType::RefSig, 3);
    DEFINE_SIG(ItemReleaseInventoryTransaction_vtable, "48 8D 05 ? ? ? ? 48 89 4B ? 0F 57 C0 48 89 4B ? 48 89 4B ? 48 8B 5C 24 ? 48 89 07 48 8D 05 ? ? ? ? 89 4F ? C7 47", SigType::RefSig, 3);
    DEFINE_SIG(ItemUseSlowdownSystem_isSlowedByItemUse, "40 ? 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 48 8B ? 4D 85 ? 74 ? 8B 01 48 8D ? ? ? 49 8D ? ? 48 89", SigType::Sig, 0);
    DEFINE_SIG(Keyboard_feed, "E8 ? ? ? ? 33 D2 0F B6", SigType::RefSig, 1);
    DEFINE_SIG(MainView_instance, "48 8B 05 ? ? ? ? C6 40 ? ? 0F 95 C0", SigType::RefSig, 3);
    DEFINE_SIG(MinecraftPackets_createPacket, "E8 ? ? ? ? 90 48 83 BD ? ? 00 00 ? 0F 84 ? ? ? ? ff", SigType::RefSig, 1);
    DEFINE_SIG(Mob_getJumpControlComponent, "E8 ? ? ? ? 48 85 C0 74 ? C6 40 ? ? 48 83 C4 ? 5B", SigType::RefSig, 1);
    DEFINE_SIG(Mob_getCurrentSwingDuration, "E8 ? ? ? ? 80 BB 84 04 00 00", SigType::RefSig, 1);
    DEFINE_SIG(MouseDevice_feed, "E8 ? ? ? ? 40 88 ? ? ? EB ? 40 84", SigType::RefSig, 1);
    DEFINE_SIG(NetworkStackItemDescriptor_ctor, "E8 ? ? ? ? 90 48 8B ? 48 8D ? ? ? ? ? E8 ? ? ? ? 4C 8D ? ? ? ? ? 4C 89 ? ? ? ? ? 48 8D", SigType::RefSig, 1);
    DEFINE_SIG(PlayerMovement_clearInputState, "E8 ? ? ? ? 48 89 ? ? 88 9F ? ? ? ? 48 8D", SigType::RefSig, 1);
    DEFINE_SIG(RakNet_RakPeer_runUpdateCycle, "48 8B ? 48 89 ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 48 8B ? 48 89 ? ? 4C 8B ? 48 89 ? ? ? 45 33", SigType::Sig, 0);
    DEFINE_SIG(RakNet_RakPeer_sendImmediate, "40 ? 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 48 8D ? ? ? 48 89 ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 48 8B", SigType::Sig, 0);
    DEFINE_SIG(ScreenView_setupAndRender, "E8 ? ? ? ? 48 8B 44 24 ? 48 8D 4C 24 ? 48 8B 80", SigType::RefSig, 1);
    DEFINE_SIG(SimulatedPlayer_simulateJump, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 47", SigType::Sig, 0);
    DEFINE_SIG(ItemInHandRenderer_render_bytepatch, "F3 0F ? ? ? ? ? ? 48 8B ? F3 41 ? ? ? 0F 57", SigType::Sig, 0);
    DEFINE_SIG(SneakMovementSystem_tickSneakMovementSystem, "32 C0 88 43 ? 84 C0", SigType::Sig, 0);
    DEFINE_SIG(ConnectionRequest_create, "40 ? 53 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 49 8B ? 48 89 ? ? 48 89", SigType::Sig, 0);
    DEFINE_SIG(CameraDirectLookSystemUtil_handleLookInput, "48 89 ? ? ? 57 48 83 EC ? F3 41 ? ? ? 49 8B ? F3 41", SigType::Sig, 0) ;
    DEFINE_SIG(ItemRenderer_render, "48 8B ? 48 89 ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 49 8B", SigType::Sig, 0);
    DEFINE_SIG(ItemPositionConst, "F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? 0F B7", SigType::Sig, 0);
    DEFINE_SIG(glm_rotate, "40 53 48 83 EC ? F3 0F 59 0D ? ? ? ? 4C 8D 4C 24", SigType::Sig, 0);
    DEFINE_SIG(glm_rotateRef, "E8 ? ? ? ? 0F 28 ? ? ? ? ? 48 8B ? C6 40 38", SigType::Sig, 0);
    DEFINE_SIG(glm_translateRef, "E8 ? ? ? ? E9 ? ? ? ? 40 84 ? 0F 84 ? ? ? ? 83 FF", SigType::Sig, 0);
    DEFINE_SIG(glm_translateRef2, "E8 ? ? ? ? C6 46 ? ? F3 0F 11 74 24 ? F3 0F 10 1D", SigType::Sig, 0);
    DEFINE_SIG(BlockSource_fireBlockChanged, "4C 8B ? 45 89 ? ? 49 89 ? ? 53", SigType::Sig, 0);
    DEFINE_SIG(ActorAnimationControllerPlayer_applyToPose, "E8 ? ? ? ? 48 81 C3 ? ? ? ? 48 3B ? 75 ? 48 8B ? ? ? EB", SigType::RefSig, 1);
    DEFINE_SIG(JSON_parse, "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 0F 57 ? 0F 11 ? ? ? ? ? 0F BE ? ? ? ? ? 44 8B", SigType::RefSig, 1);
    DEFINE_SIG(Actor_getStatusFlag, "E8 ? ? ? ? 84 C0 74 ? 48 8B 93 ? ? ? ? 80 BA ? ? ? ? ? 75 ? 48 8B 8A ? ? ? ? 8B 52 ? 48 8B 01 48 8B 40 ? FF 15 ? ? ? ? 48 8B F8", SigType::RefSig, 1);
    DEFINE_SIG(Level_getRuntimeActorList, "40 ? 48 83 EC ? 48 81 C1 ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 83 C4 ? 5B C3 CC CC 48 83 EC ? 48 8B ? ? 48 85", SigType::Sig, 0);
    DEFINE_SIG(ConcreteBlockLegacy_getCollisionShapeForCamera, "40 ? 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 4C 8B ? 4D 8B ? 49 8B", SigType::Sig, 0);
    DEFINE_SIG(WaterBlockLegacy_getCollisionShapeForCamera, "0F 10 ? ? ? ? ? 48 8B ? F2 0F ? ? ? ? ? ? 0F 11 ? F2 0F ? ? ? C3 CC", SigType::Sig, 0);
    DEFINE_SIG(mce_framebuilder_RenderItemInHandDescription_ctor, "48 89 ? ? ? 48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 83 EC ? 4D 8B ? 4D 8B ? 4C 8B ? 48 8B ? 45 33", SigType::Sig, 0);

    //DEFINE_SIG(JSON_toStyledString, "E8 ? ? ? ? 90 0F B7 ? ? ? ? ? 66 89 ? ? ? ? ? 48 8D ? ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 0F 57 ? 0F 11 ? ? ? ? ? 48 8D ? ? ? ? ? 48 89 ? ? ? ? ? 8B 85 ? ? ? ? 89 85 ? ? ? ? 4C 8B ? ? ? ? ? 49 8B ? 48 8D ? ? ? ? ? E8 ? ? ? ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 48 8B ? 48 8D ? ? ? ? ? E8 ? ? ? ? 90 48 8D ? ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 90 0F 57 ? 33 C0 0F 11 ? ? ? ? ? 48 89 ? ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 90 48 8D", SigType::RefSig, 1);

    // TODO: Identify proper function names for these and refactor them accordingly
    DEFINE_SIG(checkBlocks, "48 8D 05 ? ? ? ? 4C 8B CD 48 89 44 24 ? 4C 8D 44 24", SigType::Sig, 0);
    DEFINE_SIG(Reach, "F3 0F ? ? ? ? ? ? 44 0F ? ? 76 ? C6 44 24 64", SigType::Sig, 0);
    DEFINE_SIG(BlockReach, "F3 0F ? ? ? ? ? ? 48 8B ? ? ? 48 83 C4 ? 5F C3 83 C0", SigType::Sig, 0);
    DEFINE_SIG(GetSpeedInAirWithSprint, "41 C7 40 ? ? ? ? ? F6 02", SigType::Sig, 0);
    DEFINE_SIG(ConnectionRequest_create_DeviceModel, "48 8B 11 48 83 C2 ? EB", SigType::Sig, 0);
    DEFINE_SIG(ConnectionRequest_create_DeviceOS, "74 ? 41 80 3F ? 74 ? 49 8B ? 49 8B ? 48 8B ? ? ? ? ? FF 15", SigType::Sig, 0);
    DEFINE_SIG(ConnectionRequest_create_DefaultInputMode, "74 1B 49 83 3E 00", SigType::Sig, 0);
    DEFINE_SIG(GetInputModeBypass, "8B D7 48 8B CE 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 49 8B 06", SigType::Sig, 0);
    DEFINE_SIG(MobileRotations, "48 8b 5c 24 ? 89 07 48 83 c4 ? 5f c3 f2 0f 10 02", SigType::Sig, 0);
    DEFINE_SIG(TapSwingAnim, "F3 44 ? ? ? ? ? ? ? 41 0F ? ? F3 0F ? ? ? ? C6 40 38 ? 48 8B ? 41 0F ? ? E8 ? ? ? ? 45 84 ? 0F 85 ? ? ? ? 48 8B", SigType::RefSig, 5);
    DEFINE_SIG(FluxSwing, "E8 ? ? ? ? 48 8B ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? C6 40 38 ? 48 8B ? EB", SigType::Sig, 0);
    DEFINE_SIG(BobHurt, "40 ? 56 48 83 EC ? 0F 29 ? ? ? 48 81 C1", SigType::Sig, 0);
    DEFINE_SIG(CameraComponent_applyRotation, "66 0F ? ? 0F 5B ? 0F 2F ? 76 ? F3 0F ? ? F3 0F", SigType::Sig, 0); // Guessed func name
    DEFINE_SIG(FireRender, "48 8B ? 55 53 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 49 8B ? 4D 8B ? 4C 89 ? ? ? 4C 8B ? 48 89 ? ? 4C 8B", SigType::Sig, 0);
    DEFINE_SIG(Actor_canSee, "E8 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? E9", SigType::RefSig, 1);

    // whatever it is, it takes a uint64_t as first param (unknown) then takes an Actor* in the second param
    DEFINE_SIG(Unknown_canShowNameTag, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 32 C0 48 8B ? ? ? 48 83 C4 ? 5F C3 8B 43", SigType::Sig, 0);

    DEFINE_SIG(ItemInHandRenderer_renderItem_bytepatch, "F3 0F ? ? ? ? ? ? 0F 57 ? F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 2F ? 73 ? F3 41", SigType::Sig, 0);
    DEFINE_SIG(ItemInHandRenderer_renderItem_bytepatch2, "8B 52 ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? EB ? 48 8D ? ? ? ? ? 48 8B ? ? ? ? ? 48 8B", SigType::Sig, 0);


    static void initialize();
    static void deinitialize();
};

