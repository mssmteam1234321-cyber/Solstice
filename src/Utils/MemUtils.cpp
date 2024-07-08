//
// Created by vastrakai on 6/25/2024.
//

#include "MemUtils.hpp"
#include <libhat/Access.hpp>
#include <windows.h>
#include <Psapi.h>
#include <libhat/Process.hpp>

#include <spdlog/spdlog.h>


template <typename Ret, typename ... Args>
auto MemUtils::getFunc(void* func)
{
    return reinterpret_cast<Ret(__cdecl*)(Args...)>(func);
}

template <typename Ret, typename ... Args>
auto MemUtils::getVirtualFunc(void* _this, int index)
{
    auto vtable = *reinterpret_cast<uintptr_t**>(_this);
    return reinterpret_cast<Ret(__thiscall*)(void*, Args...)>(vtable[index]);
}

const std::string MemUtils::getMbMemoryString(uintptr_t addr)
{
    MEMORY_BASIC_INFORMATION mbi;
    if(VirtualQuery(reinterpret_cast<void*>(addr), &mbi, sizeof(mbi)))
    {
        char moduleName[MAX_PATH];
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(mbi.AllocationBase), moduleName, MAX_PATH)) {
            // use fmt or something here
            std::string addrHex = fmt::format("{:X}", addr - reinterpret_cast<uintptr_t>(mbi.AllocationBase));
            std::string result = std::string(moduleName) + "+" + addrHex;
            return result.substr(result.find_last_of('\\') + 1);
        }
    }

    return "unknown";
}

uintptr_t MemUtils::GetVTableFunction(void *_this, int index) {
    uintptr_t *vtable = *reinterpret_cast<uintptr_t **>(_this);
    return vtable[index];
}

std::string MemUtils::getModulePath(HMODULE handle)
{
    char path[MAX_PATH];
    if (GetModuleFileNameExA(GetCurrentProcess(), handle, path, MAX_PATH))
    {
        return path;
    }
    return "";
}
