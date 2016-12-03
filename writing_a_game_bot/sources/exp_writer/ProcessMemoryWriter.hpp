#pragma once

#include <Windows.h>
#include <Psapi.h>

class ProcessMemoryWriter
{
public:
    ProcessMemoryWriter(DWORD pid)
    {
        attachNewProcess(pid);
    }
    ProcessMemoryWriter() = default;
    ~ProcessMemoryWriter()
    {
        safeCloseAndNullHandle(processHandle);
    }

    ProcessMemoryWriter(ProcessMemoryWriter &&rval) = delete;
    ProcessMemoryWriter& operator=(ProcessMemoryWriter &&rval) = delete;
    ProcessMemoryWriter(const ProcessMemoryWriter&) = delete;
    ProcessMemoryWriter& operator=(const ProcessMemoryWriter&) = delete;

    template <typename T>
    void write(DWORD dwordAddress, T val) const
    {
        return write<T>(reinterpret_cast<LPVOID>(dwordAddress + baseAddress), val);
    }

    template <typename T>
    void write(LPVOID addressInProcess, T val) const
    {
        WriteProcessMemory(processHandle,
                           addressInProcess,
                           &val,
                           sizeof(T),
                           NULL);
    }

    void attachNewProcess(DWORD pid)
    {
        safeCloseAndNullHandle(processHandle);

        processHandle = OpenProcess(PROCESS_ALL_ACCESS,
                                    TRUE,
                                    pid);

        baseAddress = getModuleBase(pid);
    }

private:
    DWORD getModuleBase(DWORD pid, const char *sModuleName = "Tibia.exe")
    {
        auto hProc = OpenProcess(PROCESS_ALL_ACCESS,
                                 TRUE,
                                 pid);

        HMODULE *hModules = nullptr;
        char szBuf[50];
        DWORD cModules;
        DWORD dwBase = -1;
        //------

        EnumProcessModules(hProc, hModules, 0, &cModules);
        hModules = new HMODULE[cModules / sizeof(HMODULE)];

        if (EnumProcessModules(hProc, hModules, cModules / sizeof(HMODULE), &cModules))
        {
            for (DWORD i = 0; i < cModules / sizeof(HMODULE); ++i)
            {
                if (GetModuleBaseNameA(hProc, hModules[i], szBuf, sizeof(szBuf)))
                {
                    if (_strcmpi(szBuf, sModuleName) == 0)
                    {
                        dwBase = (DWORD)hModules[i];
                        break;
                    }
                }
            }
        }

        delete[] hModules;

        CloseHandle(hProc);

        return dwBase;
    }

    void closeAndNullHandle(HANDLE &handle)
    {
        CloseHandle(handle);
        handle = NULL;
    }

    void safeCloseAndNullHandle(HANDLE &handle)
    {
        if (handle != NULL)
            closeAndNullHandle(handle);
    }

private:
    HANDLE processHandle = NULL;
    DWORD baseAddress;
};
