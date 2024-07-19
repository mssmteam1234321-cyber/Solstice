#pragma once
#include <cstdint>
#include <string>
#include <atomic>
#define _AMD64_
#include <minwindef.h>
#include <unordered_map>
//
// Created by vastrakai on 6/25/2024.
//

class MemUtils {
public:
    template<typename Ret, typename... Args>
    static auto getFunc(void* func);
    template<typename Ret, typename... Args>
    static auto getVirtualFunc(void* _this, int index);

    template <typename TRet, typename... TArgs>
    static TRet callFastcall(void* func, TArgs... args)
    {
        using Fn = TRet(__fastcall*)(TArgs...);
        Fn f = reinterpret_cast<Fn>(func);
        return f(args...);
    }

    template <typename TRet, typename... TArgs>
    static TRet callFastcall(uintptr_t func, TArgs... args)
    {
        using Fn = TRet(__fastcall*)(TArgs...);
        Fn f = reinterpret_cast<Fn>(func);
        return f(args...);
    }

    template<typename Ret, typename... Args>
    static auto callVirtualFunc(int index, void* _this, Args... args)
    {
        using Fn = Ret(__thiscall*)(void*, Args...);
        auto vtable = *reinterpret_cast<uintptr_t**>(_this);
        return reinterpret_cast<Fn>(vtable[index])(_this, args...);
    }

    /// <summary>
    /// Defines a function to byte-patch an address with a given byte array
    /// </summary>
    /// <remarks>
    /// This should only be used in .cpp files
    /// </remarks>
    /// <param name="name">The name of the function</param>
    /// <param name="addr">The address to patch</param>
    /// <param name="bytes">The bytes to replace the address with</param>
    #define DEFINE_PATCH_FUNC(name, addr, bytes) \
        void name(bool patch) { \
            static std::vector<unsigned char> ogBytes = { bytes }; \
            static bool wasPatched = false; \
            if (addr == 0) { \
                spdlog::error("Failed to patch {} at 0x{:X} [Invalid address]", #name, addr); \
                return; \
            } \
            if (patch) { \
                 if (!wasPatched) { \
                     ogBytes = MemUtils::readBytes(addr, sizeof(bytes)); \
                     MemUtils::writeBytes(addr, bytes); \
                     spdlog::info("Patched {} at 0x{:X}", #name, addr); \
                     wasPatched = true; \
                } \
            } else { \
                if (wasPatched) { \
                    MemUtils::writeBytes(addr, ogBytes); \
                    spdlog::info("Unpatched {} at 0x{:X}", #name, addr); \
                    wasPatched = false; \
                } \
            } \
        }

    /// <summary>
    /// Defines a function to NOP-patch an address with a given size
    /// </summary>
    /// <remarks>
    /// This should only be used in .cpp files
    /// </remarks>
    /// <param name="name">The name of the function</param>
    /// <param name="addr">The address to patch</param>
    /// <param name="size">The amount of bytes to replace with NOP instructions</param>
    #define DEFINE_NOP_PATCH_FUNC(name, addr, size) \
        void name(bool patch) { \
            static std::vector<unsigned char> ogBytes; \
            static bool wasPatched = false; \
            if (addr == 0) { \
                spdlog::error("Failed to patch {} at 0x{:X} [Invalid address]", #name, addr); \
                return; \
            } \
            if (patch) { \
                if (!wasPatched) { \
                    ogBytes = MemUtils::readBytes(addr, size); \
                    std::vector<unsigned char> bytes(size, 0x90); \
                    MemUtils::writeBytes(addr, bytes); \
                    spdlog::info("Patched {} at 0x{:X}", #name, addr); \
                    wasPatched = true; \
                } \
            } else { \
                if (wasPatched) { \
                    MemUtils::writeBytes(addr, ogBytes); \
                    spdlog::info("Unpatched {} at 0x{:X}", #name, addr); \
                    wasPatched = false; \
                } \
            } \
        }




    #define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name

    #define CLASS_FIELD(type, name, offset) \
        AS_FIELD(type, name, get##name); \
        type get##name() const { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); } \
        void set##name(type v) const { *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset) = std::move(v); }

    #define TOKENPASTE(x, y) x ## y
    #define TOKENPASTE2(x, y) TOKENPASTE(x, y)
    #define PAD(size) \
        private: \
            char TOKENPASTE2(padding_, __LINE__) [size]; \
        public:
    /// <summary>
    /// Defines a padding field of a given size
    /// </summary>
    /// <param name="size">The size of the padding</param>
    #define PADDING(size) char TOKENPASTE2(padding_, __LINE__) [size]

    static uintptr_t getAddressByIndex(uintptr_t _this, int index);

    static const std::string getMbMemoryString(uintptr_t addr);

    static uintptr_t GetVTableFunction(void *_this, int index);
    static std::string getModulePath(HMODULE handle);
    static void writeBytes(uintptr_t ptr, const std::vector<unsigned char>& bytes, size_t size);
    static void writeBytes(uintptr_t ptr, const void* bytes, size_t size);
    static void writeBytes(uintptr_t ptr, const std::vector<unsigned char>& bytes);
    static std::vector<unsigned char> readBytes(uintptr_t ptr, size_t size);
    static void setProtection(uintptr_t ptr, size_t size, DWORD protection);
};


