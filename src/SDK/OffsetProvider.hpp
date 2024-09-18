#pragma once
//
// Created by vastrakai on 6/25/2024.
//

enum class OffsetType {
    Index,
    FieldOffset,
};

// kinda aids ik stfu
#define DEFINE_INDEX_FIELD_TYPED(type, name, str, index_offset, offset_type) \
public: \
static inline type (name) = 0; \
private: \
static void name##_initializer() { \
    static bool initialized = false; \
    if (initialized) return; \
    initialized = true; \
    auto result = scanSig(hat::compile_signature<str>(), #name, index_offset); \
    if (!result.has_result()) return; \
    if ((offset_type) == OffsetType::Index) name = *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset) / 8; \
    else (name) = *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset); \
} \
private: \
static inline std::function<void()> name##_function = (mSigInitializers.emplace_back(name##_initializer), std::function<void()>()); \
public:


#define DEFINE_INDEX_FIELD(name, str, index_offset, offset_type) \
public: \
static inline int (name) = 0; \
private: \
static void name##_initializer() { \
    static bool initialized = false; \
    if (initialized) return; \
    initialized = true; \
    auto result = scanSig(hat::compile_signature<str>(), #name, index_offset); \
    if (!result.has_result()) return; \
    if ((offset_type) == OffsetType::Index) (name) = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(result.get()) + (index_offset)) / 8; \
    else (name) = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(result.get()) + (index_offset)); \
} \
private: \
static inline std::function<void()> name##_function = (mSigInitializers.emplace_back(name##_initializer), std::function<void()>()); \
public:

// Defines a direct offset for a field (for offsets/indexes that can't be sigged)
#define DEFINE_FIELD(name, offset) \
public: \
static constexpr inline int (name) = offset;

#include <future>
#include <libhat/Scanner.hpp>

class OffsetProvider {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::function<void()>> mSigInitializers;
    static inline int mSigScanCount;
public:
    static inline bool mIsInitialized = false;
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    DEFINE_INDEX_FIELD(ClientInstance_getLocalPlayer, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 8B 50 ? 48 8B ? ? 89 94 ? ? ? ? ? 48 8D ? ? ? ? ? ? E8", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getBlockSource, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 85 ? 75 ? 48 83 C4 ? 5B C3 48 8B", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getOptions, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 3C ? 75 ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 3C", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getScreenName, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 90 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 90 48 89 ? ? ? 48 8B ? ? ? 48 89 ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 8B ? ? ? 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 8B ? ? ? 48 8B ? ? ? 48 83 C4 ? 5F C3 E8 ? ? ? ? 90 CC CC CC CC CC 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? 48 83 C1", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getMouseGrabbed, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 75 ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 83 39 ? 0F 84 ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? ? ? ? 0F 28 ? ? ? ? ? 66 0F ? ? ? ? ? ? BF", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_setDisableInput, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? 48 8B", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_grabMouse, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? 48 8B", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_releaseMouse, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? 48 8B", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_mMinecraftSim, "48 8B ? ? ? ? ? E8 ? ? ? ? 88 87 ? ? ? ? 48 8B ? ? ? ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mLevelRenderer, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? ? ? ? ? 48 05 ? ? ? ? C3", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mPacketSender, "48 8B ? ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B ? ? ? ? ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mGuiData, "48 8B ? ? ? ? ? 48 8B ? 48 85 ? 74 ? 33 C9 48 89 ? 48 89 ? ? 48 8B ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? 48 8B ? ? 48 8B ? ? 48 89 ? 48 89 ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 83 38 ? 74 ? 48 8B ? 48 83 C4 ? 5B C3 E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC 48 8B ? ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 ? ? ? 57", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftGame_playUi, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 83 C4 ? C3 40", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MinecraftGame_mClientInstances, "48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? 80 78 19 ? 75 ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftGame_mProfanityContext, "48 8B ? ? ? ? ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 90 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 90 48 C7 44 24 20", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftGame_mMouseGrabbed, "88 9F ? ? ? ? 48 8D ? ? ? E8 ? ? ? ? 90", 2, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MainView_bedrockPlatform, "? 8B ? ? ? ? ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_baseTick, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 40 84 ? 75", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MinecraftSim_mGameSim, "48 8B ? ? ? ? ? 8B 5A ? C1 EB", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftSim_mRenderSim, "49 8B ? ? ? ? ? 8B 48 ? 89 4C", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftSim_mGameSession, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? ? 48 85 ? 74 ? 48 83 39 ? 74 ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, GameSession_mEventCallback, "48 8B ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 CC CC 48 89", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, BedrockPlatformUWP_mcGame, "48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(bgfx_d3d12_RendererContextD3D12_m_commandQueue, "48 8B ? ? ? ? ? 48 8B ? ? ? ? ? E8 ? ? ? ? 8B CF", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(bgfx_context_m_renderCtx, "48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 89 ? ? ? ? ? 48 63", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(BlockLegacy_mBlockId, "44 0F ? ? ? ? ? ? B8 ? ? ? ? 48 8B ? 48 8B", 4, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(BlockLegacy_mayPlaceOn, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 0F B6 ? E8 ? ? ? ? 48 85 ? 75 ? 48 8B ? ? 48 85 ? 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Actor_mContainerManagerModel, "49 8B ? ? ? ? ? 48 89 ? ? ? 48 89 ? ? ? F0 FF ? ? 66 0F ? ? ? ? EB ? 0F 57 ? 66 0F 73 D8 ? 66 48 ? ? ? 0F 57 ? 66 0F ? ? ? ? ? ? 48 85", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mGameMode, "4C 8B ? ? ? ? ? 49 8B ? 4C 8B ? ? ? ? ? 49 8B ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mSupplies, "48 8B ? ? ? ? ? 80 BA B0 00 00 00 ? 75 ? 48 8B ? ? ? ? ? 8B 52 ? 48 8B ? 48 8B ? ? 48 FF ? ? ? ? ? 48 8D ? ? ? ? ? C3 48 89", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(PlayerInventory_mContainer, "48 8B ? ? ? ? ? 8B 52 ? 48 8B ? 48 8B ? ? 48 FF ? ? ? ? ? 48 8D", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, Container_getItem, "48 8B ? ? FF 15 ? ? ? ? 4C 8B ? 41 80 7E 23", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(LevelRenderer_mRendererPlayer, "48 8B ? ? ? ? ? 48 89 ? ? 48 8D ? ? ? ? ? 48 85 ? 0F 84", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(LevelRendererPlayer_mCameraPos, "F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? 0F 2F ? ? ? ? ? 0F 83", 4, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mSwinging, "88 ? ? ? ? ? EB ? 33 ? 89", 2, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_swing, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 44 39 ? ? ? 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Actor_mLevel, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? ? ? ? ? 48 83 C4 ? 48 FF ? ? ? ? ? 65 48", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Level_getHitResult, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 4C 8B ? 48 8B ? 4C 8B ? 49 8B ? 4C 8B", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Level_getPlayerList, "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 8B 48 ? 39 8B ? ? ? ? 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Actor_mDestroying, "44 38 ? ? ? ? ? 74 ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? FF 15", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, UIProfanityContext_mEnabled, "80 79 ? ? 74 ? 80 79 18 ? 74 ? 80 7C 24 60", 2, OffsetType::FieldOffset);
    //DEFINE_INDEX_FIELD(Bone_mPartModel, "8B 81 ? ? ? ? 89 82 ? ? ? ? F3 0F ? ? ? ? ? ? 0F 57", 2, OffsetType::FieldOffset); // this isn't even correct lmao
    DEFINE_INDEX_FIELD(Actor_mHurtTimeComponent, "48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 89 ? ? ? 48 8B ? 8B 38", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(BlockSource_getChunk, "48 8B ? ? ? ? ? 44 89 ? ? ? FF 15 ? ? ? ? 48 83 C4", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(BlockSource_setBlock, "48 8B ? ? ? ? ? FF 15 ? ? ? ? EB ? 41 83 FF", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(LevelChunk_mSubChunks, "48 8B ? ? ? ? ? 48 8B ? ? ? ? ? 48 2B ? 48 C1 F8 ? 48 0F ? ? 48 85 ? 74 ? 48 8B ? 66 0F", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, BlockSource_mBuildHeight, "0F BF ? ? 3B C8 0F 8D ? ? ? ? 8B 0F", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, ContainerManagerModel_getContainerType, "48 8B ? ? FF 15 ? ? ? ? 84 C0 75 ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? B0", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, ContainerManagerModel_getSlot, "48 8B 40 ? FF 15 ? ? ? ? 48 8B D8 8B 56 ? 0F B6 4E ? 45 33 FF 44 89 BD ? ? ? ? 88 8D ? ? ? ? 0F B7 4C 24 ? 66 89 8D ? ? ? ? 0F B6 4C 24 ? 88 8D ? ? ? ? 44 89 BD ? ? ? ? 89 95 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 90 48 8D 95 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 90 48 8B D3 48 8D 8D ? ? ? ? E8 ? ? ? ? 4C 8D 2D ? ? ? ? 4C 89 AD ? ? ? ? 44 89 BD ? ? ? ? 44 88 BD ? ? ? ? 48 8D 93 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 90 48 8D 95 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 4C 89 AD ? ? ? ? 44 89 BD ? ? ? ? 44 88 BD ? ? ? ? 48 8D 95 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 90 48 8D 95", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Level_getPlayerMovementSettings, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 49 8B ? ? 44 38", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(Actor_mSerializedSkin, "48 8B ? ? ? ? ? 4C 8B ? 48 8B ? ? 89 44 ? ? E8", 3, OffsetType::FieldOffset);

    // Just in case if its needed, these arent actually implemented yet
    /*DEFINE_INDEX_FIELD(ClientInstance_mHMDState, "48 8D ? ? ? ? ? 48 8B ? E8 ? ? ? ? 90 48 8D ? ? ? ? ? E8 ? ? ? ? 90 48 8D ? ? ? ? ? E8 ? ? ? ? 90 C6 86 38 08 00 00", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientHMDState_mLastLevelViewMatrix, "4C 8D ? ? ? ? ? 4D 8B ? 48 8D ? ? ? ? ? 4C 2B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientHMDState_mLastLevelViewMatrixAbsolute, "F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? F3 0F ? ? ? ? ? ? 48 85", 4, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientHMDState_mLastLevelProjMatrix, "4C 8D ? ? ? ? ? 49 8B ? 48 8D ? ? ? ? ? 48 2B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientHMDState_mLastLevelWorldMatrix, "89 88 ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 83 38 ? 0F 84 ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? ? ? ? 48 89 ? ? ? 48 8B ? ? ? ? ? 48 89 ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 41 B9", 2, OffsetType::FieldOffset);
    */

    // TODO: Move all CLASS_FIELD declarations and hat::member_at offsets to here
    DEFINE_FIELD(ContainerScreenController_tryExit, 12);
    DEFINE_FIELD(ItemStack_reInit, 3);
    DEFINE_FIELD(MinecraftUIRenderContext_drawImage, 7);
    DEFINE_FIELD(Actor_mEntityIdentifier, 0x240);
    DEFINE_FIELD(Actor_mLocalName, 0xCA8);
    DEFINE_FIELD(ClientInstance_mViewMatrix, 0x358);
    DEFINE_FIELD(ClientInstance_mFovX, 0x6F0);
    DEFINE_FIELD(ClientInstance_mFovY, 0x704);
    DEFINE_FIELD(Block_mRuntimeId, 0xC0);
    DEFINE_FIELD(Block_mLegacy, 0x30);

    static void initialize();
    static void deinitialize();
};
