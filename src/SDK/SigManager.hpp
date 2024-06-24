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

#define DEFINE(name, str) const hat::signature_view name = hat::compile_signature<str>()

// defines a uintptr_t member with the signature's result address
#define DEFINE_MEMBER_SIG(name, str) static inline uint64_t name = reinterpret_cast<uintptr_t>(scanSig(hat::compile_signature<str>(), #name).get());

class SigManager {
    static hat::scan_result scanSig(hat::signature_view sig, const std::string& name);
public:
    static inline std::unordered_map<std::string, uintptr_t> mSigs;

    DEFINE_MEMBER_SIG(MainView_instance, "48 8B 05 ? ? ? ? C6 40 ? ? 0F 95 C0");

    static void initialize();
};