#pragma once

#include <Windows.h>
#include <Psapi.h>

class ProcessMemoryReader
{
public:
    ProcessMemoryReader(DWORD pid)
    {
        attachNewProcess(pid);
    }
    ProcessMemoryReader() = default;
    ~ProcessMemoryReader()
    {
        safeCloseAndNullHandle(processHandle);
    }

    ProcessMemoryReader(ProcessMemoryReader &&rval) = delete;
    ProcessMemoryReader& operator=(ProcessMemoryReader &&rval) = delete;
    ProcessMemoryReader(const ProcessMemoryReader&) = delete;
    ProcessMemoryReader& operator=(const ProcessMemoryReader&) = delete;

    void attachNewProcess(DWORD pid)
    {
        safeCloseAndNullHandle(processHandle);

        processHandle = OpenProcess(PROCESS_ALL_ACCESS,
                                    TRUE,
                                    pid);

        baseAddress = getModuleBase(pid);
    }

    template <typename T>
    T read(DWORD offset) const
    {
        return read<T>(reinterpret_cast<LPCVOID>(offset + baseAddress));
    }

    template <typename T>
    T read(LPCVOID addressInProcess) const
    {
        T value;

        if (processHandle == NULL)
            return T{};

        ReadProcessMemory(processHandle,
                          addressInProcess,
                          &value,
                          sizeof(T),
                          NULL);
        return value;
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

    HANDLE processHandle = NULL;
    DWORD baseAddress;
};
