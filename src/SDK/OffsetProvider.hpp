#pragma once
//
// Created by vastrakai on 6/25/2024.
//

enum class OffsetType {
    Index,
    FieldOffset,
};

#define DEFINE_SIG(name, str) \
static inline uintptr_t name; \
static void name##_initializer() { name = reinterpret_cast<uintptr_t>(scanSig(hat::compile_signature<str>(), #name).get()); } \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());

// define index field: gets the result of
/*#define DEFINE_INDEX_FIELD(name, str, index_offset) \
static inline int name; \
static void name##_initializer() { name = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(scanSig(hat::compile_signature<str>(), #name, index_offset).get()) + index_offset) / 8; } \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());*/
// If the offset_type is OffetType::Index, then the offset is divided by 8
// Else, the offset is added to the result
#define DEFINE_INDEX_FIELD(name, str, index_offset, offset_type) \
static inline int name; \
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
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());

// Same as above but you can pick the type of the field
#define DEFINE_INDEX_FIELD_TYPED(type, name, str, index_offset, offset_type) \
static inline type name; \
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
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>());



#include <future>
#include <libhat/Scanner.hpp>

class OffsetProvider {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::future<void>> futures;
    static inline int mSigScanCount;
    static inline uint64_t mSigScanStart;
public:
    static inline std::unordered_map<std::string, uintptr_t> mSigs;
    DEFINE_INDEX_FIELD(ClientInstance_getLocalPlayer, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 8B 50 ? 48 8B ? ? 89 94 ? ? ? ? ? 48 8D ? ? ? ? ? ? E8", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getBlockSource, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 3C ? 75 ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 3C", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getOptions, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 90 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 90 48 89 ? ? ? 48 8B ? ? ? 48 89 ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 8B ? ? ? 48 85 ? 74 ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 8B ? ? ? 48 8B ? ? ? 48 83 C4 ? 5F C3 E8 ? ? ? ? 90 CC CC CC CC CC 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? 48 83 C1", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getScreenName, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 75 ? 48 8B ? ? ? ? ? 48 85 ? 0F 84 ? ? ? ? 48 83 39 ? 0F 84 ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? F0 FF ? ? 48 8B ? ? ? ? ? 0F 28 ? ? ? ? ? 66 0F ? ? ? ? ? ? BF", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_getMouseGrabbed, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 8B 50 ? 48 8B ? ? 89 94 ? ? ? ? ? 48 8D", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_setDisableInput, "48 8B ? ? ? ? ? 48 8B ? FF 15 ? ? ? ? 84 DB 74", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(ClientInstance_grabMouse, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? ? ? 48 83 C4 ? 5F C3 CC CC CC 48 8D", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MinecraftGame_playUi, "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 83 C4 ? C3 40", 3, OffsetType::Index);
    DEFINE_INDEX_FIELD(MainView_bedrockPlatform, "? 8B ? ? ? ? ? 48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B", 3, OffsetType::FieldOffset);
    DEFINE_INDEX_FIELD_TYPED(uint8_t, BedrockPlatformUWP_mcGame, "48 8B ? ? 48 8B ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? 48 8B ? 48 8B", 3, OffsetType::FieldOffset);

    static void initialize();
};
