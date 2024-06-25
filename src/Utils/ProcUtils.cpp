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
