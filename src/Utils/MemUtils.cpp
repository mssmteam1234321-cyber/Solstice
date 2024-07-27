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

uintptr_t MemUtils::getAddressByIndex(uintptr_t _this, int index)
{
    const auto vtable = *reinterpret_cast<uintptr_t**>(_this);
    return vtable[index];
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

void MemUtils::writeBytes(uintptr_t ptr, const std::vector<unsigned char>& bytes, size_t size)
{
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(ptr), size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(reinterpret_cast<void*>(ptr), bytes.data(), size);
    VirtualProtect(reinterpret_cast<void*>(ptr), size, oldProtect, &oldProtect);
}

void MemUtils::writeBytes(uintptr_t ptr, const void* bytes, size_t size)
{
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(ptr), size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(reinterpret_cast<void*>(ptr), bytes, size);
    VirtualProtect(reinterpret_cast<void*>(ptr), size, oldProtect, &oldProtect);
}

void MemUtils::writeBytes(uintptr_t ptr, const std::vector<unsigned char>& bytes)
{
    writeBytes(ptr, bytes, bytes.size());
}

void MemUtils::copyBytes(uintptr_t dest, uintptr_t src, size_t size)
{
    if (dest == 0 || src == 0) {
        spdlog::error("Failed to copy bytes from 0x{:X} to 0x{:X} [Invalid address]", src, dest);
        return;
    }
    DWORD oldprotect;
    VirtualProtect(reinterpret_cast<void*>(dest), size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy(reinterpret_cast<void*>(dest), reinterpret_cast<void*>(src), size);
    VirtualProtect(reinterpret_cast<void*>(dest), size, oldprotect, &oldprotect);
}

std::vector<unsigned char> MemUtils::readBytes(uintptr_t ptr, size_t size)
{
    std::vector<unsigned char> buffer(size);
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(ptr), size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(buffer.data(), reinterpret_cast<void*>(ptr), size);
    VirtualProtect(reinterpret_cast<void*>(ptr), size, oldProtect, &oldProtect);
    return buffer;
}

void MemUtils::setProtection(uintptr_t ptr, size_t size, DWORD protection)
{
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(ptr), size, protection, &oldProtect);
}

