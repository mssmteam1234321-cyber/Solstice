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

// ik this is kinda aids lol
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
    if (sig_type == SigType::Sig) { \
        name = reinterpret_cast<uintptr_t>(result.get()); \
    } else { \
        name = reinterpret_cast<uintptr_t>(result.rel(offset)); \
    } \
} \
static inline std::future<void> name##_future = (futures.push_back(std::async(std::launch::async, name##_initializer)), std::future<void>()); \
public:



class SigManager {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name, int offset = 0);

    static inline std::vector<std::future<void>> futures;
    static inline int mSigScanCount;
    static inline uint64_t mSigScanStart;
public:
    static inline bool mIsInitialized = false;
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    DEFINE_SIG(ConnectionRequest_create, "40 ? 53 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 49 8B ? 48 89 ? ? 48 89", SigType::Sig, 0);
    DEFINE_SIG(Actor_setPosition, "E8 ? ? ? ? 48 8D ? ? 66 0F ? ? ? ? 48 8B ? 49 8B", SigType::RefSig, 1);
    DEFINE_SIG(MinecraftPackets_createPacket, "E8 ? ? ? ? 90 48 83 BD ? ? 00 00 ? 0F 84 ? ? ? ? ff", SigType::RefSig, 1);
    DEFINE_SIG(MainView_instance, "48 8B 05 ? ? ? ? C6 40 ? ? 0F 95 C0", SigType::RefSig, 3);
    DEFINE_SIG(GuiData_displayClientMessage, "40 ? 53 56 57 41 ? 48 8D ? ? ? ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? 41 0F", SigType::Sig, 0);
    DEFINE_SIG(Keyboard_feed, "E8 ? ? ? ? 33 D2 0F B6", SigType::RefSig, 1);
    DEFINE_SIG(ClientInstance_mBgfx, "48 8B ? ? ? ? ? 66 44 ? ? ? ? E8 ? ? ? ? 0F B7 ? 66 89 ? E9 ? ? ? ? 8B 53 ? 49 2B ? 8B C2 41 0F ? ? 48 3B ? 75 ? 48 83 F9 ? 0F 84 ? ? ? ? 4D 85 ? 75 ? 48 85 ? 0F 85 ? ? ? ? EB ? 48 83 F9 ? 0F 83 ? ? ? ? 8B C1 48 3B ? 0F 85 ? ? ? ? 8B D1 49 8B ? E8 ? ? ? ? 8B 53 ? 45 33", SigType::RefSig, 3);
    DEFINE_SIG(MouseDevice_feed, "E8 ? ? ? ? 40 88 ? ? ? EB ? 40 84", SigType::RefSig, 1);
    DEFINE_SIG(ClientInstance_grabMouse, "40 ? 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 40", SigType::Sig, 0);
    DEFINE_SIG(ClientInstance_releaseMouse, "40 ? 48 83 EC ? 48 8B ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 84 C0 74 ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? 48 83 C4 ? 5B 48 FF ? ? ? ? ? 48 83 C4 ? 5B C3 48 89", SigType::Sig, 0);
    DEFINE_SIG(ItemUseSlowdownSystem_isSlowedByItemUse, "40 ? 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 48 8B ? 4D 85 ? 74 ? 8B 01 48 8D ? ? ? 49 8D ? ? 48 89", SigType::Sig, 0);
    DEFINE_SIG(BlockLegacy_mayPlaceOn, "48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 41 ? 48 83 EC ? 48 8B ? 41 0F ? ? 48 8B ? ? 49 8B", SigType::Sig, 0);
    DEFINE_SIG(ContainerScreenController_tick, "E8 ? ? ? ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? 41 B8 ? ? ? ? 8B F8", SigType::RefSig, 1);
    DEFINE_SIG(ContainerScreenController_handleAutoPlace, "E8 ? ? ? ? 66 ? ? ? ? ? ? ? 0F 8C", SigType::RefSig, 1);

    static void initialize();
};