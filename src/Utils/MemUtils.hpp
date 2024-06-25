#pragma once
#include <cstdint>
//
// Created by vastrakai on 6/25/2024.
//


class MemUtils {
public:
    template<typename Ret, typename... Args>
    static auto GetFunc(void* func);
    template<typename Ret, typename... Args>
    static auto GetVFunc(void* _this, int index);
    template<typename Ret, typename... Args>
    static auto CallVFunc(int index, void* _this, Args... args)
    {
        using Fn = Ret(__thiscall*)(void*, Args...);
        auto vtable = *reinterpret_cast<uintptr_t**>(_this);
        return reinterpret_cast<Fn>(vtable[index])(_this, args...);
    }


#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
    // Usage: CLA
#define CLASS_FIELD(type, name, offset) \
    AS_FIELD(type, name, get##name); \
    type get##name() const { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); } \
    void set##name(type v) const { *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset) = v; }

};


