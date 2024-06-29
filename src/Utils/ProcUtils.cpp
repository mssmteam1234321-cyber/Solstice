//
// Created by vastrakai on 6/24/2024.
//

#include "ProcUtils.hpp"

#include <Windows.h>
#include <tlhelp32.h>


int ProcUtils::getModuleCount()
{
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);
        return 0;
    }

    int count = 1;
    while (Module32Next(hModuleSnap, &me32))
    {
        count++;
    }

    CloseHandle(hModuleSnap);
    return count;
}

HWND ProcUtils::getMinecraftWindow()
{
    static HWND window = nullptr;
    if (!window)
    {
        std::map<HWND, std::string> titles;
        auto callback = [](HWND hwnd, LPARAM lParam) -> BOOL {

            char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
            std::string titleStr = title;
            auto *titles = reinterpret_cast<std::map<HWND, std::string> *>(lParam);
            titles->insert({hwnd, titleStr});
            return TRUE;
        };

        EnumWindows(callback, reinterpret_cast<LPARAM>(&titles));

        for (auto& [hwnd, title] : titles)
        {
            if (title.find("Minecraft") != std::string::npos)
            {
                window = hwnd;
                break;
            }
        }

        spdlog::info("Minecraft window handle: 0x{:X}", reinterpret_cast<uintptr_t>(window));
    }


    return window;
}
