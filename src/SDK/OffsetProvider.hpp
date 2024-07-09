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
static inline type name; \
private: \
static void name##_initializer() { \
    auto result = scanSig(hat::compile_signature<str>(), #name, index_offset); \
    if (!result.has_result()) { \
        name = 0; \
        return; \
    } \
    if (offset_type == OffsetType::Index) { \
        name = *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset) / 8; \
    } else { \
        name = *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset); \
    } \
} \
private: \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>()); \
public:


#define DEFINE_INDEX_FIELD(name, str, index_offset, offset_type) \
public: \
static inline int name; \
private: \
static void name##_initializer() { \
    auto result = scanSig(hat::compile_signature<str>(), #name, index_offset); \
    if (!result.has_result()) { \
        name = 0; \
        return; \
    } \
    if (offset_type == OffsetType::Index) { \
        name = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset) / 8; \
    } else { \
        name = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(result.get()) + index_offset); \
    } \
} \
private: \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>()); \
public:

#include <future>
#include <libhat/Scanner.hpp>

class OffsetProvider {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::future<void>> futures;
    static inline int mSigScanCount;
    static inline uint64_t mSigScanStart;
public:
    static inline bool mIsInitialized = false;
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    DEFINE_INDEX_FIELD(ClientInstance_getLocalPlayer, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 8B 50 ? 48 8B ? ? 89 94 ? ? ? ? ? 48 8D ? ? ? ? ? ? E8", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getBlockSource, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 8B ? 48 83 C4 ? 5B E9 ? ? ? ? CC CC CC 48 89", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getOptions, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 3C ? 75 ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 3C", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getScreenName, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 90 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 90 48 89 ? ? ? 48 8B ? ? ? 48 89 ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 8B ? ? ? 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 8B ? ? ? 48 8B ? ? ? 48 83 C4 ? 5F C3 E8 ? ? ? ? 90 CC CC CC CC CC 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? 48 83 C1", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getMouseGrabbed, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 75 ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 83 39 ? 0F 84 ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? ? ? ? 0F 28 ? ? ? ? ? 66 0F ? ? ? ? ? ? BF", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_setDisableInput, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 84 DB 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_grabMouse, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 84 DB 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_releaseMouse, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 84 DB 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_mMinecraftSim, "48 8B ? ? ? ? ? E8 ? ? ? ? 88 87 ? ? ? ? 48 8B ? ? ? ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mLevelRenderer, "48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? ? ? ? ? 48 05 ? ? ? ? C3", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mPacketSender, "48 8B ? ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 8B ? ? ? ? ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(ClientInstance_mGuiData, "48 8B ? ? ? ? ? 48 8B ? 48 85 ? 74 ? 33 C9 48 89 ? 48 89 ? ? 48 8B ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? 48 8B ? ? 48 8B ? ? 48 89 ? 48 89 ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 83 38 ? 74 ? 48 8B ? 48 83 C4 ? 5B C3 E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC 48 8B ? ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 ? ? ? 57", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftGame_playUi, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 83 C4 ? C3 40", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MinecraftGame_mClientInstances, "48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? 80 78 19 ? 75 ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MainView_bedrockPlatform, "? 8B ? ? ? ? ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_baseTick, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 45 84", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MinecraftSim_mGameSim, "48 8B ? ? ? ? ? 8B 5A ? C1 EB", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftSim_mRenderSim, "49 8B ? ? ? ? ? 44 8B ? ? 44 89 ? ? ? 41 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(MinecraftSim_mGameSession, "49 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? ? 0F 57", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(GameSession_mEventCallback, "48 8B ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? ? 48 85 ? 74 ? 48 8B ? 48 8B ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 CC CC 48 89", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, BedrockPlatformUWP_mcGame, "48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(bgfx_d3d12_RendererContextD3D12_m_commandQueue, "48 8B ? ? ? ? ? 48 8B ? ? ? ? ? E8 ? ? ? ? 8B CF", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(bgfx_context_m_renderCtx, "48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 89 ? ? ? ? ? 48 63", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(BlockLegacy_mBlockId, "44 0F ? ? ? ? ? ? B8 ? ? ? ? 48 8B ? 48 8B", 4, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mContainerManagerModel, "49 8B ? ? ? ? ? 48 89 ? ? ? 48 89 ? ? ? F0 FF ? ? 66 0F ? ? ? ? EB ? 0F 57 ? 66 0F 73 D8 ? 66 48 ? ? ? 0F 57 ? 66 0F ? ? ? ? ? ? 48 85", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mGameMode, "4C 8B ? ? ? ? ? 49 8B ? 4C 8B ? ? ? ? ? 49 8B ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mSupplies, "48 8B ? ? ? ? ? 80 BA B8 00 00 00 ? 75 ? 48 8B ? ? ? ? ? 8B 52 ? 48 8B ? 48 8B ? ? 48 FF ? ? ? ? ? 48 8D", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(PlayerInventory_mContainer, "48 8B ? ? ? ? ? 8B 52 ? 48 8B ? 48 8B ? ? 48 FF ? ? ? ? ? 48 8D", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, Container_getItem, "48 8B ? ? FF 15 ? ? ? ? 4C 8B ? EB ? 4C 8D ? ? ? ? ? 48 C7 C7", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(LevelRenderer_mRendererPlayer, "48 8B ? ? ? ? ? 48 89 ? ? 48 8D ? ? ? ? ? 48 85 ? 0F 84", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(LevelRendererPlayer_mCameraPos, "F3 0F ? ? ? ? ? ? 0F 28 ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? F3 0F ? ? 0F 2F", 4, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_mSwinging, "88 ? ? ? ? ? EB ? 33 ? 89", 2, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD(Actor_swing, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 44 39 ? ? ? 74", 3, OffsetType::Index);
    // 44 0F ? ? ? ? ? ? B8 ? ? ? ? 48 8B ? 48 8B
    static void initialize();
};
