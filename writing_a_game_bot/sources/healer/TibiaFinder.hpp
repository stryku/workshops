#pragma once

#include <Windows.h>

#include <mutex>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>

struct TibiaClientInfo
{
    std::string windowName;
    DWORD pid;
    HWND hwnd;
};

class TibiaFinder
{
public:
    struct PidToHwndData
    {
        DWORD pid;
        HWND hwnd;
    };

    std::vector<std::wstring> findAllClientsTitles()
    {
        std::vector<std::wstring> results;

        EnumWindows(checkWindowTitle,
                    reinterpret_cast<LPARAM>(&results));

        return results;
    }

    HWND pidToHwnd(DWORD pid)
    {
        PidToHwndData pidToHwndData;

        pidToHwndData.pid = pid;

        EnumWindows(callbackPidToHwnd, reinterpret_cast<LPARAM>(&pidToHwndData));

        return pidToHwndData.hwnd;
    }

    DWORD findProcessId(const std::wstring &tibiaWindowTitle)
    {
        //auto str = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(tibiaWindowTitle);
        HWND hwnd = FindWindow(0, tibiaWindowTitle.c_str());
        DWORD pid;

        if (hwnd == NULL)
            return NULL;

        GetWindowThreadProcessId(hwnd, &pid);

        return pid;
    }

private:
    static std::wstring getWindowTitle(HWND hwnd)
    {
        const size_t maxTitleLength = 256;

        wchar_t title[maxTitleLength];

        GetWindowTextW(hwnd,
                       reinterpret_cast<LPWSTR>(title),
                       maxTitleLength);

        return title;
    }

    static BOOL CALLBACK checkWindowTitle(HWND hwnd, LPARAM lParam)
    {
        auto &resultVector = *reinterpret_cast<std::vector<std::wstring>*>(lParam);
        auto windowTitle = getWindowTitle(hwnd);
        std::wstring pattern(L"Tibia - ");

        if (windowTitle.find(pattern) != std::wstring::npos)
            resultVector.push_back(windowTitle);

        return TRUE;
    }

    static BOOL CALLBACK callbackPidToHwnd(HWND hwnd, LPARAM lParam)
    {
        DWORD lpdwProcessId;

        GetWindowThreadProcessId(hwnd, &lpdwProcessId);

        auto &data = *(reinterpret_cast<PidToHwndData*>(lParam));

        if (lpdwProcessId == data.pid)
        {
            data.hwnd = hwnd;
            return FALSE;
        }

        return TRUE;
    }
};
