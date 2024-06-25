//
// Created by vastrakai on 6/25/2024.
//

#include "MemUtils.hpp"

template <typename Ret, typename ... Args>
auto MemUtils::GetFunc(void* func)
{
    return reinterpret_cast<Ret(__cdecl*)(Args...)>(func);
}

template <typename Ret, typename ... Args>
auto MemUtils::GetVFunc(void* _this, int index)
{
    auto vtable = *reinterpret_cast<uintptr_t**>(_this);
    return reinterpret_cast<Ret(__thiscall*)(void*, Args...)>(vtable[index]);
}


