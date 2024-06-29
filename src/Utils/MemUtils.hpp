#pragma once
#include <cstdint>
#include <string>
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

#define PADDING(size) char TOKENPASTE2(padding_, __LINE__) [size]


    static const std::string getMbMemoryString(uintptr_t addr);

    static uintptr_t GetVTableFunction(void *_this, int index);
};


